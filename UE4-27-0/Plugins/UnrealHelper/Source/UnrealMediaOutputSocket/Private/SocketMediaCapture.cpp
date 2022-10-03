#include "SocketMediaCapture.h"
#include "Async/Async.h"
#include "Misc/Paths.h"
#include "MediaIOCoreFileWriter.h"
#include "Modules/ModuleManager.h"
#include "Containers/CircularQueue.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif
// UnrealAudioUtilities seems to have the nicer queue code
// #include "RenderUtils.h"
#include "RenderingThread.h"

/**
 * Write to a target pipe/socket as a consumer thread.
 */
/* Short-hand seemed a lot shorter - but harder to manage async task
	Semaphore = FPlatformProcess::GetSynchEventFromPool(false);
	DrainTask = FPlatformProcess::GetSynchEventFromPool(true); // called once
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, bHasTcpEndpoint]() {
		UE_LOG(LogTemp, Warning, TEXT("Ffmpeg async thread started"));
		LockedSendQueue(
			bHasTcpEndpoint ? Socket : nullptr,
			bHasTcpEndpoint ? nullptr : StdinWritePipe,
			StopTaskFlag, PendingFrames, Queue, Semaphore, DrainTask, PacketSize);
		UE_LOG(LogTemp, Warning, TEXT("Ffmpeg async thread done"));
	});
*/

class UNREALMEDIAOUTPUTSOCKET_API SocketMediaCaptureSink : public FRunnable, FSingleThreadRunnable
{

public:
	FThreadSafeBool StopTaskFlag;
	FThreadSafeCounter PendingFrames;
	FEvent *Semaphore = nullptr;
	TCircularQueue<TArray<uint8, FDefaultAllocator>> *Queue;
	FRunnableThread *Thread = nullptr;
	uint8 *SingleThreadData = nullptr;
	// TQueue<TArray<uint8, FDefaultAllocator>> Queue; //  = TQueue<TArray<uint8, FDefaultAllocator>>(121);

private:
	int32 PacketSize;
	FSocket *WriteSocket;
	void *WritePipe = nullptr;

public:
	SocketMediaCaptureSink(FSocket *TargetSocket, void *TargetPipe, int32 FramePacketSize)
	{
		WriteSocket = TargetSocket;
		WritePipe = TargetPipe;
		PacketSize = FramePacketSize;
	}

	~SocketMediaCaptureSink()
	{
		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue destructor"));
		StopTaskFlag = true;
		WriteSocket = nullptr;
		WritePipe = nullptr;
		StopListening();
		UE_LOG(LogTemp, Warning, TEXT("Media Capture Sink Destroyed"));
	}

	FSingleThreadRunnable *GetSingleThreadInterface()
	{
		return this;
	}

	bool Init()
	{
		Semaphore = FPlatformProcess::GetSynchEventFromPool(true);
		Queue = new TCircularQueue<TArray<uint8, FDefaultAllocator>>(511 + 1);
		return true;
	}

	void Exit()
	{
		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue exiting"));
		StopTaskFlag = true;
		Queue->Empty();
		Semaphore->Trigger();
		// Is this destructor implemented so return synch does not need to?
		// delete Semaphore;
		FPlatformProcess::ReturnSynchEventToPool(Semaphore);
		Semaphore = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue exit"));
	}

	void Stop()
	{
		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue stop early"));
		if (StopTaskFlag)
		{
			return;
		}
		// FIXME: May already be handled via Exit()
		StopTaskFlag = true;
		Queue->Empty();
		if (Semaphore != nullptr)
		{
			Semaphore->Trigger();
		}
	}

	bool EnqueuePacket(void *InBuffer, bool asAsync)
	{
		if (StopTaskFlag)
		{
			return false;
		}
		if (!asAsync)
		{
			SingleThreadData = (uint8 *)InBuffer;
			Tick();
		}
		else
		{
			// InBuffer is on main thread and needs to be copied out before getting clobbered.
			TArray<uint8> FrameData;
			FrameData.AddUninitialized(PacketSize);
			FMemory::Memcpy(FrameData.GetData(), InBuffer, PacketSize);
			Queue->Enqueue(FrameData);
			PendingFrames.Increment();
			int32 MAX_BUFFER_SIZE = 1024 * 1024 * 1024; // 1 gigabyte ram buffer
			Semaphore->Trigger();
			if (PendingFrames.GetValue() * PacketSize > MAX_BUFFER_SIZE)
			{
				UE_LOG(LogTemp, Warning, TEXT("Reached max buffer size %d with pending frames %d"), MAX_BUFFER_SIZE, PendingFrames.GetValue());
				return false;
			}
			// Seems suspend/unsuspend would be bad form:
			// https://docs.microsoft.com/en-us/dotnet/api/system.threading.thread.suspend?view=net-5.0
			// Thread->Suspend(false);
		}
		return !StopTaskFlag;
	}

	// This is convoluted, using EnqueuePacket and Tick.
	void Tick()
	{
		// Single-thread interface
		if (WriteSocket != nullptr)
		{
			int32 BytesLeft = PacketSize;
			double SocketTimeoutMs = 100; // 15; // 60fps requires a low timeout
			if (!SocketMediaCaptureSink::SocketSend(WriteSocket, ((const uint8 *)SingleThreadData), BytesLeft, SocketTimeoutMs))
			{
				UE_LOG(LogTemp, Warning, TEXT("Unexpected connection issue to ffmpeg"));
				StopTaskFlag = true;
			}
			else if (BytesLeft > 0)
			{
				if (BytesLeft == PacketSize)
				{
					UE_LOG(LogTemp, Warning, TEXT("Partial packet on pipe"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Timeout on pipe: %d"), BytesLeft);
				}
			}
		}
		else if (WritePipe != nullptr)
		{
			int32 BytesLeft = PacketSize;
			FThreadSafeBool AlwaysFalse;
			if (!SocketMediaCaptureSink::PipeSend(WritePipe, ((const uint8 *)SingleThreadData), BytesLeft, AlwaysFalse)) // , StopTaskFlag))
			{
				UE_LOG(LogTemp, Warning, TEXT("Unexpected connection issue to ffmpeg"));
				StopTaskFlag = true;
				// StopCaptureImpl(false);
			}
		}
	}

	// This is a single frame writer; for debugging we want multiple frames (and a very large file)
	// MediaIOCoreFileWriter::WriteRawFile(OutputFilename, reinterpret_cast<uint8*>(InBuffer), Stride * Height);
	static bool SocketSend(FSocket *TargetSocket, const uint8 *InBuffer, int32 &BytesLeft, double ReadyTimeoutMs)
	{
		int32 BytesOffset = 0;
		int32 BytesSent = 0;

		while (TargetSocket->Wait(ESocketWaitConditions::WaitForWrite, FTimespan::FromMilliseconds(ReadyTimeoutMs)))
		// && BytesLeft > 0
		{
			if (BytesLeft == 0)
				break;
			// UE_LOG(LogTemp, Warning, TEXT("Socket Send Attempt %d at point %d"), BytesLeft, BytesOffset);
			TargetSocket->Send((InBuffer) + BytesOffset, BytesLeft, BytesSent);
			if (BytesSent == -1)
			{
				UE_LOG(LogTemp, Warning, TEXT("Socket Send Failure of %d at point %d"), BytesLeft, BytesOffset);
				break;
			}
			BytesLeft -= BytesSent;
			BytesOffset += BytesSent;
		}
		if (BytesSent == -1)
		{
			return false;
		}
		return true;
	}

	static bool PipeSend(void *TargetPipe, const uint8 *InBuffer, int32 &BytesLeft, FThreadSafeBool &Stop)
	{
		int32 BytesOffset = 0;
		int32 BytesSent = 0;

		while (BytesLeft > 0 && !Stop)
		{
			if (BytesLeft == 0)
				break;
			FPlatformProcess::WritePipe(TargetPipe, (InBuffer) + BytesOffset, BytesLeft, &BytesSent);
			if (BytesSent == -1)
				break;
			BytesLeft -= BytesSent;
			BytesOffset += BytesSent;
		}

		// Clear stdout/stderr or ffmpeg will deadlock
		// FString StdoutErr = FPlatformProcess::ReadPipe(TargetPipe);

		if (BytesSent == -1)
		{
			return false;
		}
		return true;
	}

public:
	bool StartListening()
	{
		if (Thread != nullptr)
			return false;
		Thread = FRunnableThread::Create(this, TEXT("USocketMediaCaptureThread"), 128 * 1024, EThreadPriority::TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
		return (Thread != nullptr);
	}

	void StopListening()
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopping thread"));
		if (Thread != nullptr)
		{
			StopTaskFlag = true;
			if (Semaphore != nullptr)
			{
				Semaphore->Trigger();
			}
			Thread->WaitForCompletion();
			delete Thread;
			Thread = nullptr;
		}
	}

	uint32 Run()
	{
		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue started"));

		TArray<uint8> InElement;
		const uint8 *InBuffer = nullptr;
		int32 BytesLeft = 0;
		bool SendFailed = false;
		const double SocketTimeoutMs = 2000; // This is a limit on delay

		while (!Queue->IsEmpty() || !StopTaskFlag)
		{
			if (!Queue->Dequeue(InElement))
			{
				if (StopTaskFlag)
					continue;
				Semaphore->Reset();
				Semaphore->Wait();
				continue;
			}
			InBuffer = InElement.GetData();
			// UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue loop begin"));

			// Bypass the pipe to debug the queue implementation
			if (false)
			{
				PendingFrames.Decrement();
				continue;
			}

			BytesLeft = PacketSize;
			if (WritePipe != nullptr)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Send via Pipe"));
				if (!PipeSend(WritePipe, InBuffer, BytesLeft, StopTaskFlag))
				{
					SendFailed = true;
					break;
				}
			}

			else if (WriteSocket != nullptr)
			{
				if (!SocketSend(WriteSocket, InBuffer, BytesLeft, SocketTimeoutMs))
				{
					SendFailed = true;
					break;
				}
			}

			if (BytesLeft > 0)
			{
				if (BytesLeft != PacketSize)
				{
					UE_LOG(LogTemp, Warning, TEXT("Partial packet sent, pipe corrupted"));
					SendFailed = true;
					break;
				}
				UE_LOG(LogTemp, Warning, TEXT("Timeout on pipe with frames remaining: %d - packet size: %d"), PendingFrames.GetValue(), PacketSize);
				SendFailed = true;
				break;
			}

			if (SendFailed && WritePipe != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("FFmpeg pipe failed early"));
				StopTaskFlag = true;
			}

			else if (SendFailed && WriteSocket != nullptr)
			{
				ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
				ESocketErrors ErrorCode = PlatformSockets->GetLastErrorCode();
				FString ErrorString = FString(PlatformSockets->GetSocketError(ErrorCode));
				if (ErrorString == "SE_ECONNRESET")
				{
					UE_LOG(LogTemp, Warning, TEXT("FFmpeg failure on port connection %s"), *ErrorString);
				}
				else if (ErrorString == "SE_EFAULT")
				{
					UE_LOG(LogTemp, Warning, TEXT("Ffmpeg socket dropped unexpectedly: %s"), *ErrorString);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Ffmpeg unknown connection issue: %s"), *ErrorString);
				}
				StopTaskFlag = true;
			}

			else
			{
				PendingFrames.Decrement();
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("LockedSendQueue drained"));

		StopTaskFlag = true;

		return 0;
	}
};

/**
 * Media output to write to an off-thread sink, optional spawned process
 * off-thread writes to a socket/pipe to allow the next frame to render.
 */

bool USocketMediaCapture::ValidateMediaOutput() const
{
	USocketMediaOutput *SocketMediaOutput = Cast<USocketMediaOutput>(MediaOutput);
	if (!SocketMediaOutput)
	{
		UE_LOG(LogTemp, Error, TEXT("Socket Media Capture unable to cast provided Media Output"));
		return false;
	}

	bool bHasTcpEndpoint = !SocketMediaOutput->NoNetwork;
	if (bHasTcpEndpoint)
	{
		FString IPAddress = FString("127.0.0.1");
		uint32 Port = 4445;

		if (!SocketMediaOutput->IPAddress.IsEmpty())
		{
			IPAddress = SocketMediaOutput->IPAddress;
		}

		if (SocketMediaOutput->Port > 0)
		{
			Port = (uint32)SocketMediaOutput->Port;
		}

		ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
		if (PlatformSockets == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to access platform sockets"));
			return false;
		}
		bool bIsValid;
		TSharedPtr<FInternetAddr> Endpoint = PlatformSockets->CreateInternetAddr();
		Endpoint->SetIp(*IPAddress, bIsValid);
		Endpoint->SetPort(Port);
		if (!bIsValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to set ip and port"));
			return false;
		}
		// UE_LOG(LogTemp, Warning, TEXT("TCP:connect %s"), *Endpoint->ToString(true));
	}

	return true;
}

TArray<FStringFormatArg> USocketMediaCapture::AsProcessArgs(FIntPoint Size, FFrameRate Framerate, FString IPAddress, uint32 Port)
{
	USocketMediaOutput *SocketMediaOutput = CastChecked<USocketMediaOutput>(MediaOutput);

	FString inputFormat;
	inputFormat = TEXT("-f rawvideo -pixel_format rgba");
	if (SocketMediaOutput->PixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		inputFormat = TEXT("-f rawvideo -pixel_format gray");
	}
	else if (SocketMediaOutput->PixelFormat == ESocketMediaOutputPixelFormat::RGBA)
	{
		inputFormat = TEXT("-f rawvideo -pixel_format rgba");
		// Untested for A2RGB10 - currently an ffmpeg compile-time:
		// -pixel_format x2rgb10
		// still fresh: https://master.gitmailbox.com/ffmpegdev/20220218055012.916556-1-wenbin.chen@intel.com/T/
	}
	else if (SocketMediaOutput->PixelFormat == ESocketMediaOutputPixelFormat::UYVY)
	{
		// Preferred output format as this is an animation
		inputFormat = TEXT("-f rawvideo -pixel_format uyvy422");
	}
	else if (SocketMediaOutput->PixelFormat == ESocketMediaOutputPixelFormat::V210)
	{
		// Library of congress notes v210 for analog capture
		// incorrect, will lead to double image and flicker:
		// inputFormat = TEXT("-f rawvideo -pixel_format yuv422p10le -c:v v210");
		// V210 is the 10-bit standard input for production capture cards in Unreal
		inputFormat = TEXT("-f v210");
	}

	FString inputFramerate = FString::Printf(TEXT("%d/%d"), Framerate.Numerator, Framerate.Denominator);

	FString OutputMov = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::ProjectSavedDir(), *FString("output.mp4")));
	if (!SocketMediaOutput->OutputFilename.IsEmpty())
	{
		OutputMov = SocketMediaOutput->OutputFilename;
	}

	TArray<FStringFormatArg> args;
	args.Add(FStringFormatArg(inputFormat));
	args.Add(FStringFormatArg(Size.X));
	args.Add(FStringFormatArg(Size.Y));
	args.Add(FStringFormatArg(inputFramerate));
	args.Add(FStringFormatArg(IPAddress));
	args.Add(FStringFormatArg(Port));
	args.Add(FStringFormatArg(OutputMov));

	return args;
}

static FORCEINLINE bool CreatePipeWrite(void *&ReadPipe, void *&WritePipe)
{
#if PLATFORM_WINDOWS
	SECURITY_ATTRIBUTES Attr = {sizeof(SECURITY_ATTRIBUTES), NULL, true};

	if (!::CreatePipe(&ReadPipe, &WritePipe, &Attr, 0))
	{
		return false;
	}

	if (!::SetHandleInformation(WritePipe, HANDLE_FLAG_INHERIT, 0))
	{
		return false;
	}

	return true;
#else
	return FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
#endif // PLATFORM_WINDOWS
}

bool USocketMediaCapture::SpawnProcessBlocking(uint32 *ProcessId, TArray<FStringFormatArg> FormatArgs, FString ExpectStr)
{
	USocketMediaOutput *SocketMediaOutput = Cast<USocketMediaOutput>(MediaOutput);

	// FIXME: Not working out for writing to an anonymous pipe (stdin of ffmpeg)
	bool bHasTcpEndpoint = !SocketMediaOutput->NoNetwork;
	// This defaults to a null output for reporting input framerate/bandwidth:
	FString FfmpegParams = FString::Printf(TEXT("-y -report -vsync 0 -hwaccel auto {0} -video_size {1}x{2} -framerate {3} -i %s -f null {6}"), bHasTcpEndpoint ? TEXT("tcp://{4}:{5}?listen") : TEXT("-"));

	FString ExecutableParamsTemplate = SocketMediaOutput->ExecutableParams.IsEmpty() ? FfmpegParams : SocketMediaOutput->ExecutableParams;
	FString Params = FString::Format(*ExecutableParamsTemplate, FormatArgs);

	// FFmpeg specific defaults

#if PLATFORM_WINDOWS
	FString URL = SocketMediaOutput->ExecutablePath.IsEmpty() ? TEXT("C:\\ffmpeg\\bin\\ffmpeg.exe") : SocketMediaOutput->ExecutablePath;
#else
	FString URL = SocketMediaOutput->ExecutablePath.IsEmpty() ? TEXT("/usr/local/bin/ffmpeg") : SocketMediaOutput->ExecutablePath;
#endif

	if (!FPlatformProcess::CreatePipe(StderrReadPipe, StderrWritePipe))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create stdout pipe"));
		return false;
	}

	if (!bHasTcpEndpoint)
	{
		// Needs: bLaunchDetached=True on create proc
		if (!CreatePipeWrite(StdinReadPipe, StdinWritePipe))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create stdin pipe"));
			return false;
		}
	}

	bool bHasReport = !SocketMediaOutput->NoReport;
	if (bHasReport)
	{
// UE_LOG(LogTemp, Warning, TEXT("Reporting of my dreams"));
#if PLATFORM_WINDOWS
		// The colon in drive paths causes file= to fail on absolute paths
		FPlatformMisc::SetEnvironmentVar(TEXT("FFREPORT"), TEXT("file=output.log"));
#else
		// OS X PIE does not seem to respect the working folder but absolute paths work
		FString OutputLog = FPaths::ConvertRelativePathToFull(FPaths::Combine(*FPaths::ProjectSavedDir(), *FString("output.log")));
		FPlatformMisc::SetEnvironmentVar(TEXT("FFREPORT"), *FString::Printf(TEXT("file=%s"), *OutputLog));
#endif
	}

	UE_LOG(LogTemp, Warning, TEXT("Launch process: %s %s"), *URL, *Params);

	bool bHiddenExecutable = !SocketMediaOutput->NoHiddenExecutable;
	int32 PriorityModifier = 2; // High. (2 is Higher)
	ProcessHandle = FPlatformProcess::CreateProc(*URL, *Params, !bHasTcpEndpoint || !bHiddenExecutable, bHiddenExecutable, bHiddenExecutable,
												 ProcessId, PriorityModifier, *FPaths::ProjectSavedDir(), StderrWritePipe, StdinReadPipe);

	if (!ProcessHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to launch process."));
		return false;
	}

	if (ExpectStr.IsEmpty())
	{
		return true;
	}

	// Sleep, because WaitForWrite does not seem to block for ffmpeg readiness in OS X
	// and cut ffmpeg a little more slack for gpu probe / other startup activities
	for (uint8 Attempts = 0; Attempts < 10; Attempts++)
	{
		FString TmpStdout = FPlatformProcess::ReadPipe(StderrReadPipe);
		if (!TmpStdout.IsEmpty())
		{
			if (TmpStdout.Contains(*ExpectStr))
			{
				UE_LOG(LogTemp, Warning, TEXT("Process ready via stdout after waiting #%d: %s"), Attempts, *TmpStdout);
				break;
			}
		}
		else if (Attempts == 9)
		{
			UE_LOG(LogTemp, Warning, TEXT("Process stdout not ready, continuing anyway after attempt #%d: %s"), Attempts, *TmpStdout);
		}
		FPlatformProcess::Sleep(0.05);
	}
	FPlatformProcess::Sleep(0.1);
	return true;
}

// TODO: See if this is needed - we block anyway as we wait for the process to terminate
// 	virtual bool HasFinishedProcessing() const;

// From FunctionalUIScreenshotTest and MediaCapture
void USocketMediaCapture::GetBackbufferInfo(const FSceneViewport *InViewport, EPixelFormat *OutPixelFormat, bool *OutIsSRGB, FIntPoint *Size)
{

	/*
		// Or if an actual texture as provided, as in 4.26 custom capture:
		FTextureRenderTargetResource* InTextureRenderTargetResource
		SourceTexture = InTextureRenderTargetResource->GetTextureRenderTarget2DResource()->GetTextureRHI();
	*/

	/**
	 * #if WITH_EDITOR
	 *	if (!IsRunningGame())
	 *	{
	 */
	FTexture2DRHIRef SourceTexture = InViewport->GetRenderTargetTexture();
	if (SourceTexture.IsValid())
	{
		*OutPixelFormat = SourceTexture->GetFormat();
		*OutIsSRGB = (SourceTexture->GetFlags() & TexCreate_SRGB) == TexCreate_SRGB;
		*Size = SourceTexture->GetSizeXY();
		return;
	}

	if (!InViewport->GetViewportRHI())
	{
		return;
	}

	ENQUEUE_RENDER_COMMAND(GetBackbufferFormatCmd)
	(
		[InViewport, OutPixelFormat, OutIsSRGB, Size](FRHICommandListImmediate &RHICmdList)
		{
			FViewportRHIRef ViewportRHI = InViewport->GetViewportRHI();
			if (!ViewportRHI.IsValid())
			{
				return;
			}
			FTexture2DRHIRef BackbufferTexture = RHICmdList.GetViewportBackBuffer(ViewportRHI);
			if (!BackbufferTexture.IsValid())
			{
				return;
			}
			*OutPixelFormat = BackbufferTexture->GetFormat();
			*OutIsSRGB = (BackbufferTexture->GetFlags() & TexCreate_SRGB) == TexCreate_SRGB;
			*Size = BackbufferTexture->GetSizeXY();
		});
	FlushRenderingCommands();
}

// TCP / Process state is better stuffed into the Output class as the Socket / Process connections could be long-lived
// and they also require setup/teardown which have delays.
// However, all MediaOutput classes are reserved in conventional use as parameter stashes and nothing more.

EPixelFormat USocketMediaCapture::GetPixelFormat()
{
	return SourcePixelFormat;
}

bool USocketMediaCapture::CaptureSceneViewportImpl(TSharedPtr<FSceneViewport> &InSceneViewport)
{

	const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.VSync"));
	bool bLockToVsync = CVar->GetValueOnGameThread() != 0;
	if (bLockToVsync)
	{
		UE_LOG(LogTemp, Warning, TEXT("VSync is set to on which may result in reduced frame rate rendering for capture."));
	}

	USocketMediaOutput *SocketMediaOutput = CastChecked<USocketMediaOutput>(MediaOutput);
	OutputPixelFormat = SocketMediaOutput->PixelFormat;				  // Workaround alpha matte challenges
	SocketMediaOutput->SourcePixelFormat = EPixelFormat::PF_B8G8R8A8; // Workaround standalone mode issue

	// Really difficult to get to: InMediaCapture->DesiredOutputSize.X / Y
	// seems required when the command line flag -forceres is not in use
	// FIntPoint Size = InSceneViewport->GetRenderTargetTextureSizeXY();
	FIntPoint Size = InSceneViewport->GetSizeXY();

	EPixelFormat PixelFormat = PF_A2B10G10R10;
	bool bIsSRGB = false;
	// Standalone may have a different buffer than the default backing buffer; force them to meet.
	GetBackbufferInfo(&*InSceneViewport, &PixelFormat, &bIsSRGB, &Size);

	if (SocketMediaOutput->PixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		//	PixelFormat = EPixelFormat::PF_B8G8R8A8;
	}

	// Pixel format can mean many different things, the enum is overloaded.
	SourcePixelFormat = PixelFormat;

	UE_LOG(LogTemp, Warning, TEXT("Enum PixelFormat: %s"), GetPixelFormatString(SourcePixelFormat));

	// Currently on GetState == EMediaCaptureState::Preparing;

	// SocketMediaOutput->DesiredPixelFormat // Bypass????
	// if (InMediaCapture->DesiredPixelFormat != SourceTexture->GetFormat())

	bool bHasProcess = !SocketMediaOutput->NoExecutable;
	bool bHasReport = !SocketMediaOutput->NoReport;
	bool bHasTcpEndpoint = !SocketMediaOutput->NoNetwork;
	bool bHiddenExecutable = !SocketMediaOutput->NoHiddenExecutable;

	// Used throughout the methods
	bHasAsyncQueue = !SocketMediaOutput->NoAsync && FPlatformProcess::SupportsMultithreading();

	// For on-cpu bit shifting.

	// One channel of 8 bits per pixel.
	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		PixelStrideBytes = 4;
		PacketSize = Size.X * Size.Y;
		OutputPacketBuffer = reinterpret_cast<uint8 *>(FMemory::Malloc(PacketSize));
	}

	// RGBA 32 bit word, one pixel
	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::RGBA)
	{
		PixelStrideBytes = 4;
		PacketSize = Size.X * Size.Y * PixelStrideBytes;
	}

	// UYVY 32 bit word, two pixels
	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::UYVY)
	{
		PixelStrideBytes = 4;
		PacketSize = Size.X / 2 * Size.Y * PixelStrideBytes;
	}

	// V210 - 128 bit word, 6 pixels - Code from upstream (floor, aligned to 128 bit words)
	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::V210)
	{
		PixelStrideBytes = 16;
		PacketSize = ((((Size.X + 47) / 48) * 48) / 6) * Size.Y * PixelStrideBytes;
		// When on a 1280 width on window, seeing ceil or an extra padding, e.g:
		// (math.ceil((1280 + 47) / 48) * 48) / 6
	}

	FFrameRate Framerate = FApp::GetTimecodeFrameRate();

	FString IPAddress = FString("127.0.0.1");
	uint32 Port = 4445;

	if (bHasTcpEndpoint)
	{
		if (!SocketMediaOutput->IPAddress.IsEmpty())
		{
			IPAddress = SocketMediaOutput->IPAddress;
		}

		if (SocketMediaOutput->Port > 0)
		{
			Port = (uint32)SocketMediaOutput->Port;
		}
	}

	// Interprocess pipe, Stdout/Stderr, with Stdin if tcp is disabled
	uint32 ProcessId = 0;
	if (bHasProcess)
	{
		FString ExpectFfmpegOutputContains = FString("ffmpeg started on");
		if (!SpawnProcessBlocking(&ProcessId, AsProcessArgs(Size, Framerate, IPAddress, Port), ExpectFfmpegOutputContains))
		{
			return false;
		}
	}

	if (bHasTcpEndpoint)
	{
		bool bIsValid;
		ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
		TSharedPtr<FInternetAddr> Endpoint = PlatformSockets->CreateInternetAddr();
		Endpoint->SetIp(*IPAddress, bIsValid);
		Endpoint->SetPort(Port);

		FString SocketDesc = FString("Image Stream");
		Socket = FTcpSocketBuilder(SocketDesc).AsBlocking().Build();
		Socket->SetNoDelay(true);
		Socket->Connect(*Endpoint);

		// FIXME: Wait may return immediately on OS X, check state too
		// for(uint8 Attempts = 0; Attempts < 10; Attempts++)
		// if Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected break;
		UE_LOG(LogTemp, Warning, TEXT("Waiting for Socket"));
		if (Socket->Wait(ESocketWaitConditions::WaitForWrite, FTimespan::FromMilliseconds(30)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket ready"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket not yet ready, continuing anyway"));
		}
	}

	if (bHasAsyncQueue)
	{
		UE_LOG(LogTemp, Warning, TEXT("Booting output buffer"));
		SocketMediaCaptureSink *OutputBuffer = new SocketMediaCaptureSink(Socket, StdinWritePipe, PacketSize);
		this->Sink = (void *)OutputBuffer;
		OutputBuffer->StartListening();
		UE_LOG(LogTemp, Warning, TEXT("Booted output buffer"));
	}

	if (bHasProcess)
	{
		FString StdoutErr = "";
		StdoutErr = FPlatformProcess::ReadPipe(StderrReadPipe);
		if (bHasTcpEndpoint && Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
		{
			UE_LOG(LogTemp, Warning, TEXT("FFMpeg Local Tcp Endpoint unreachable"));
		}
		else
		{
			if (FPlatformProcess::IsProcRunning(ProcessHandle))
			{
				UE_LOG(LogTemp, Warning, TEXT("FFMpeg Boot: %s"), *StdoutErr);
				SetState(EMediaCaptureState::Capturing);
				return true;
			}
			UE_LOG(LogTemp, Warning, TEXT("Process stopped unexpectedly"));
		}
		if (bHasTcpEndpoint)
		{
			ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
			PlatformSockets->DestroySocket(Socket);
		}

		UE_LOG(LogTemp, Warning, TEXT("Process failed to boot: %s"), *StdoutErr);
		if (FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			FPlatformProcess::TerminateProc(ProcessHandle, true);
		}
		FPlatformProcess::CloseProc(ProcessHandle);
	}
	else if (bHasTcpEndpoint)
	{
		if (Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
		{
			SetState(EMediaCaptureState::Capturing);
			return true;
		}

		ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
		PlatformSockets->DestroySocket(Socket);

		if (Socket->GetConnectionState() == ESocketConnectionState::SCS_ConnectionError)
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket connection error: tcp://%s:%d"), *IPAddress, Port);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Socket failed unexpectedly: tcp://%s:%d"), *IPAddress, Port);
		}
	}

	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		FMemory::Free(OutputPacketBuffer);
	}

	SetState(EMediaCaptureState::Error);
	return false;
}

// NOTES: Event override has to block until complete brecause:
// - Parent StopCapture runs SetState Stopped before calling StopCaptureImpl
// - Seems we are not hooked into SetState(EMediaCaptureState::StopRequested);
// - So we can not ourselves trigger appropriately SetState(EMediaCaptureState::Stopped);

void USocketMediaCapture::StopCaptureImpl(bool bAllowPendingFrameToBeProcess)
{

	if (bAllowPendingFrameToBeProcess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pending frame before shutdown"));
		return;
	}

	if (OutputPacketBuffer != nullptr)
	{
		FMemory::Free(OutputPacketBuffer);
		OutputPacketBuffer = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Shutting down process"));
	if (Sink != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shutting down locked thread"));
		SocketMediaCaptureSink *OutputBuffer = (SocketMediaCaptureSink *)Sink;

		UE_LOG(LogTemp, Warning, TEXT("Waiting on drain thread to complete"));
		OutputBuffer->StopListening();
		delete static_cast<SocketMediaCaptureSink *>(Sink);
		UE_LOG(LogTemp, Warning, TEXT("Drain thread has completed"));
	}

	if (Socket != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shutting down tcp socket: wait time"));
		Socket->Wait(ESocketWaitConditions::WaitForWrite, FTimespan::FromMilliseconds(1500));
		UE_LOG(LogTemp, Warning, TEXT("Shutting down tcp socket: shutdown time"));
		Socket->Shutdown(ESocketShutdownMode::ReadWrite);
		UE_LOG(LogTemp, Warning, TEXT("Shutting down tcp socket: close time"));
		Socket->Close();
		ISocketSubsystem *PlatformSockets = ISocketSubsystem::Get();
		PlatformSockets->DestroySocket(Socket);
		UE_LOG(LogTemp, Warning, TEXT("Socket closed"));
	}

	if (StdinReadPipe != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shutting down process input pipes"));
		FPlatformProcess::ClosePipe(StdinReadPipe, StdinWritePipe);
		StdinReadPipe = nullptr;
		StdinWritePipe = nullptr;
	}

	if (StderrReadPipe != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shutting down process output pipes"));
		int wait_lock = 0;
		while (!FPlatformProcess::ReadPipe(StderrReadPipe).IsEmpty())
		{
			// Flush for awhile
			// UE_LOG(LogTemp, Warning, TEXT("FFmpeg: %s"), *StdoutErr);
			if (wait_lock++ > 10)
			{
				UE_LOG(LogTemp, Warning, TEXT("Stopped waiting for process output"));
				break;
			}
		}
		FPlatformProcess::ClosePipe(StderrReadPipe, StderrWritePipe);
		StderrReadPipe = nullptr;
		StderrWritePipe = nullptr;
	}

	if (ProcessHandle.IsValid())
	{
		// FIXME: This has no timeout, could freeze
		UE_LOG(LogTemp, Warning, TEXT("Closing process"));
		int32 ReturnCode = -1;
		// WaitForProc has no timeout - can easily deadlock the process
		FPlatformProcess::WaitForProc(ProcessHandle);
		FPlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCode);
		FPlatformProcess::CloseProc(ProcessHandle);
		if (ReturnCode != 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("FFmpeg unsuccessful return code %d"), ReturnCode);
		}
		UE_LOG(LogTemp, Warning, TEXT("Closed proc ffmpeg"));
		if (FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			FPlatformProcess::TerminateProc(ProcessHandle, true);
			UE_LOG(LogTemp, Warning, TEXT("Forced ffmpeg to stop."));
		}
		ProcessHandle.Reset();
		// Seems like logging still does not flush to disk; give it a pause.
		FPlatformProcess::Sleep(0.05);
		UE_LOG(LogTemp, Warning, TEXT("done sleeping, releasing."));
	}
}

/**
 * This is guaranteed to be a single thread, in-order, on the render thread path.
 */

void USocketMediaCapture::OnFrameCaptured_RenderingThread(const FCaptureBaseData &InBaseData,
														  TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData,
														  void *InBuffer, int32 Width, int32 Height
#if ENGINE_MAJOR_VERSION > 4
														  ,
														  int32 BytesPerRow
#endif
)
{
	if (Sink == nullptr)
	{
		return;
	}

	if (GetState() != EMediaCaptureState::Capturing)
	{
		return;
	}

	SocketMediaCaptureSink *OutputBuffer = (SocketMediaCaptureSink *)Sink;
	if (OutputBuffer->StopTaskFlag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping frame on stopped capture"));
		return;
	}

	// SourceFrameNumber is available as of 4.24
	// if(InBaseData.SourceFrameNumber < 4) {
	//    UE_LOG(LogTemp, Warning, TEXT("packet size: %d  height x width %d x %d"), PacketSize, Height, Width);
	//    UE_LOG(LogTemp, Warning, TEXT("Start capture frame: %d with pending %d"), InBaseData.SourceFrameNumber, PendingFrames.GetValue());
	//}

	// Copy out a GRAY8 buffer. ffmpeg has x2rgb10.
	// support for a2rgb10 is still cutting edge, with hevc support only in OS X and in nvidia drivers. some othermovement here:
	// https://github.com/intel-media-ci/ffmpeg/pull/219
	// https://patchwork.ffmpeg.org/project/ffmpeg/patch/20190910093526.13442-1-zachary.zhou@intel.com/
	// ffmpeg does like to use rgba8, vp9 has support:
	// https://stackoverflow.com/questions/66769652/ue4-capture-frame-using-id3d11texture2d-and-convert-to-r8g8b8-bitmap

	if (OutputPixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		// PixelFormat == PF_FloatRGBA
		if (SourcePixelFormat == PF_B8G8R8A8)
		{
			FColor *PixelBuffer = reinterpret_cast<FColor *>(InBuffer);
			for (int32 i = 0; i < PacketSize; i++)
			{
				*(OutputPacketBuffer + i) = PixelBuffer[i].A; // (uint8) (PixelBuffer[i] & 0x000000ff);
			}
		}
		else if (SourcePixelFormat == PF_A2B10G10R10)
		{
			// Assuming directx, this is likely: DXGI_FORMAT_R10G10B10A2_UNORM
			// a 2-bit UNORM represents 0.0f, 1/3, 2/3, and 1.0f.
			// use uint32* for portability.
			// https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
			uint32 *PixelBuffer = reinterpret_cast<uint32 *>(InBuffer);
			for (int32 i = 0; i < PacketSize; i++)
			{
				// uint8 alpha = (uint8)((PixelBuffer[i] << 30) >> 24);
				uint8 alpha = (uint8)((PixelBuffer[i] >> 30) << 6);
				*(OutputPacketBuffer + i) = alpha | (alpha >> 2) | (alpha >> 4) | (alpha >> 6);
			}
		}

		if (!OutputBuffer->EnqueuePacket((void *)OutputPacketBuffer, bHasAsyncQueue))
		{
			SetState(EMediaCaptureState::Error);
		}
		return;
	}

	int32 FramePacketSize = 4 * Width * Height;
	if (FramePacketSize != PacketSize)
	{
		// Resulting stream will be out of alignment - 4.24 downscales in PIE giving wrong size to StartCaptureImpl.
		// In practice we may want to reinitialize our output pipe to our actual texture instead of our hope at initialization
		// Width is set via private parent properties:
		// InMediaCapture->DesiredOutputSize.X hitting the ReadyFrame->ReadbackTexture
		UE_LOG(LogTemp, Warning, TEXT("packet re-size: from %d to %d"), PacketSize, FramePacketSize);
		// UE_LOG(LogTemp, Warning, TEXT("Frame size: %d x %d @ %d == %d"), Width, Height, PixelStrideBytes, PacketSize);
		PacketSize = FramePacketSize;
	}

	if (StderrReadPipe != nullptr)
	{
		// Flush subprocess stderr/stdout or it will deadlock when the buffer fills
		FPlatformProcess::ReadPipe(StderrReadPipe);
	}

	if (!OutputBuffer->EnqueuePacket(InBuffer, bHasAsyncQueue))
	{
		SetState(EMediaCaptureState::Error);
	}
}
