#pragma once

#include "MediaCapture.h"
#include "Slate/SceneViewport.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Networking/Public/Common/TcpSocketBuilder.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "SocketMediaOutput.h"
#include "SocketMediaCapture.generated.h"

UCLASS(BlueprintType)
class UNREALMEDIAOUTPUTSOCKET_API USocketMediaCapture : public UMediaCapture
{
	GENERATED_BODY()
protected:
	virtual bool ValidateMediaOutput() const;
	virtual bool CaptureSceneViewportImpl(TSharedPtr<FSceneViewport> &InSceneViewport) override;
	virtual void StopCaptureImpl(bool bAllowPendingFrameToBeProcess) override;
	virtual void OnFrameCaptured_RenderingThread(const FCaptureBaseData &InBaseData,
												 TSharedPtr<FMediaCaptureUserData, ESPMode::ThreadSafe> InUserData,
												 void *InBuffer, int32 Width, int32 Height
#if ENGINE_MAJOR_VERSION > 4
												 ,
												 int32 BytesPerRow
#endif
												 ) override;

public:
	EPixelFormat GetPixelFormat();

private:
	EPixelFormat SourcePixelFormat;
	ESocketMediaOutputPixelFormat OutputPixelFormat;
	uint8 *OutputPacketBuffer = nullptr;

	void GetBackbufferInfo(const FSceneViewport *InViewport, EPixelFormat *OutPixelFormat, bool *OutIsSRGB, FIntPoint *Size);

	uint8 PixelStrideBytes;
	// Still getting a different Width*Height from Standalone mode.
	int32 PacketSize = 0;

	// Subprocess, Stderr and Stdout
	FProcHandle ProcessHandle;
	void *StderrReadPipe = nullptr;
	void *StderrWritePipe = nullptr;

	// Stdin (vs TCP socket)
	void *StdinReadPipe = nullptr;
	void *StdinWritePipe = nullptr;

	TArray<FStringFormatArg> AsProcessArgs(FIntPoint Size, FFrameRate Framerate, FString IPAddress, uint32 Port);
	bool SpawnProcessBlocking(uint32 *ProcessId, TArray<FStringFormatArg> FormatArgs, FString ExpectStr);

	// TCP Socket
	FSocket *Socket;

	// Copy frame data from rendering thread
	bool bHasAsyncQueue = false;
	void *Sink = nullptr;
};
