#include "UnrealHttpBPLibrary.h"
#include "UnrealHttp.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HAL/IPlatformFileModule.h"

UUnrealHttpBPLibrary::UUnrealHttpBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UUnrealHttpBPLibrary::GetHttpFile(const FString Url, const FString Filename, const FHttpOnErrorSignature& OnError, const bool ResumeIfExisting, const int64 ChunkSizeBytes)
{
	FHttpModule& HttpModule = FModuleManager::LoadModuleChecked<FHttpModule>("HTTP");
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.Get().CreateRequest();

	FString StandardFilename(Filename);
	FPaths::MakeStandardFilename(StandardFilename);

	FString Directory = FPaths::GetPath(StandardFilename);
	IFileManager* FileManager = &IFileManager::Get();

	if (FileManager->DirectoryExists(*Directory) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Folder does not exist: %s"), *Directory);
		return;
	}

	int64 StartRangeAt = 0;
	if (ResumeIfExisting && FileManager->FileExists(*StandardFilename) != false)
	{
		int64 CurrentFileSize = FileManager->FileSize(*StandardFilename);
		if(CurrentFileSize > 0) StartRangeAt = CurrentFileSize;
	}

	FString BytesRangeString = FString::Printf(TEXT("%lld"), StartRangeAt);
	FString ChunkSizeString = ChunkSizeBytes > 0 ? FString::Printf(TEXT("%lld"), StartRangeAt + ChunkSizeBytes - 1) : FString("");
    FString RequestRange = FString::Format(TEXT("bytes={0}-{1}"), {BytesRangeString, ChunkSizeString});
    FString ExpectRange = FString::Format(TEXT("bytes {0}-"), {BytesRangeString, ChunkSizeString});

	Request->SetURL(Url);
	Request->SetVerb("GET");

	/*
	 * HTTP Range Requests are a work-around to limit memory use as stream to file is unsupported in FHttpModule.
	 * The Unreal curl implementation adds "Accept-Encoding: "deflate, gzip" and will transparently decompresses the buffer.
	 * Servers which support such headers will ignore the Range header.
	 */
	if(StartRangeAt > 0 || ChunkSizeBytes > 0)
	{
		Request->SetHeader(TEXT("Range"), RequestRange);
	}
	bool bAppend = StartRangeAt > 0;
	bool bAllowRead = false;

	// progress indicator does not do much for us
	if(false)
	Request->OnRequestProgress().BindLambda([](FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived) {
		FHttpResponsePtr HttpResponse = HttpRequest->GetResponse();
		if(!HttpResponse.IsValid()) {
			UE_LOG(LogTemp, Warning, TEXT("Invalid HTTP Request"));
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Progressing: %d"), BytesReceived);
		/*
		 * The main HTTP client implementation is hard coded to expand its write buffer to the full size of the result:
 		 * FCurlHttpRequest::ReceiveResponseBodyCallback uses FMemory::Memcpy with TotalBytesRead to offset the write location.
		 *
		 * We can start writing the file to disk but we can not safely clear its memory buffer.
		 */
		  // TArray<uint8> Content = HttpResponse->GetContent();
		  // 
		  // bool bSuccess = OutputFile->Write(&Content[0], Content.Num());
		  // UE_LOG(LogTemp, Warning, TEXT("Current position Request: %d"), Content.Num());
		  // We can't clear the buffer as it is marked private
		  // HttpResponse.TotalBytesRead.SetValue(0); Content.Reset();
	});

	Request->OnProcessRequestComplete().BindLambda(
		[StandardFilename, bAppend, StartRangeAt, ChunkSizeBytes, ExpectRange, Url, Filename, OnError]
		(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		int32 HttpCode = HttpResponse->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("Final HTTP Status: %d"), HttpCode);
		if(!EHttpResponseCodes::IsOk(HttpCode)) {
			OnError.ExecuteIfBound();
			return;
		}

		FString ContentRange = HttpResponse->GetHeader("Content-Range");
		int64 FullContentLength = 0;
		// Assume HttpCode 206
		if(!ContentRange.IsEmpty())
		{
			int32 MaxRangeSeparator = 0;
			if(ContentRange.FindChar(TEXT('/'), MaxRangeSeparator) && ContentRange.StartsWith(ExpectRange, ESearchCase::CaseSensitive)) {
				FString MaxRangePart = ContentRange.RightChop(MaxRangeSeparator+1);
				if(MaxRangePart.IsNumeric())
				{
					FullContentLength = FCString::Atoi64(*MaxRangePart);
				}
				else {
					OnError.ExecuteIfBound();
					return;
				}
			}
			else {
				OnError.ExecuteIfBound();
				return;
			}
		}

		bool bAllowRead = false;
		IFileHandle *OutputFile = IPlatformFile::GetPlatformPhysical().OpenWrite(*StandardFilename, bAppend, bAllowRead);
		TArray<uint8> Content = HttpResponse->GetContent();
		int64 CurrentPosition = StartRangeAt + Content.Num();
		bool bWriteSuccess = OutputFile->Write(&Content[0], Content.Num());
		OutputFile->Flush(true);
		delete OutputFile;
	
		if(!bWriteSuccess)
		{
			OnError.ExecuteIfBound();
			return;
		}

		if(Content.Num() > ChunkSizeBytes)
		{
			UE_LOG(LogTemp, Warning, TEXT("Range request ignored for chunk size: %d"), ChunkSizeBytes);
		}
		else if(HttpCode == 206 && FullContentLength > CurrentPosition)
		{
			bool ContinueWritingOutput = true;
			UE_LOG(LogTemp, Warning, TEXT("Ready for Sub-Request: Range %d-%d/%d"), CurrentPosition, FullContentLength);
			GetHttpFile(Url, Filename, OnError, ContinueWritingOutput, ChunkSizeBytes);
		}
		UE_LOG(LogTemp, Warning, TEXT("Finished Request: %d"), Content.Num());
	});
	Request->ProcessRequest();
    // 	IFileHandler *OutputFile = IPlatformFile::GetPlatformPhysical().DeleteFile(*StandardFilename);
}

void UUnrealHttpBPLibrary::PutHttpFile(const FString Url, const FString Filename, const FHttpOnErrorSignature& OnError)
{
	FHttpModule& HttpModule = FModuleManager::LoadModuleChecked<FHttpModule>("HTTP");
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.Get().CreateRequest();

	FString StandardFilename(Filename);
	FPaths::MakeStandardFilename(StandardFilename);

	FString Directory = FPaths::GetPath(StandardFilename);
	IFileManager* FileManager = &IFileManager::Get();

	if (FileManager->FileExists(*StandardFilename) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("File does not exist: %s"), *Filename);
		return;
	}

	Request->SetURL(Url);
	Request->SetVerb("PUT");
	Request->SetContentAsStreamedFile(StandardFilename);
	Request->OnProcessRequestComplete().BindLambda([OnError](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
		if (!HttpResponse.IsValid())
		{
			OnError.ExecuteIfBound();
			return;
		}
		if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) {
			UE_LOG(LogTemp, Warning, TEXT("Http returned error code: %d"), HttpResponse->GetResponseCode());
			OnError.ExecuteIfBound();
			return;
		}
		UE_LOG(LogTemp, Warning, TEXT("Finished Request"));
	});
	Request->ProcessRequest();
}
