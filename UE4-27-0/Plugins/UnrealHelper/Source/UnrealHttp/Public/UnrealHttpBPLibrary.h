#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "HAL/PlatformApplicationMisc.h"
#include "HAL/FileManager.h"
#include "UnrealHttp.h"
#include "UnrealHttpBPLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE(FHttpOnCompleteSignature);
DECLARE_DYNAMIC_DELEGATE(FHttpOnErrorSignature);

UCLASS()
class UUnrealHttpBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Url as File", Keywords = "Http Url", AutoCreateRefTerm = "OnError"), Category = "Unreal HTTP")
	static void GetHttpFile(const FString Url, const FString Filename, const FHttpOnErrorSignature& OnError, const bool Resumable = false, int64 ChunkSizeBytes = 0);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Put File as Url", Keywords = "Http Url", AutoCreateRefTerm = "OnError"), Category = "Unreal HTTP")
	static void PutHttpFile(const FString Url, const FString Filename, const FHttpOnErrorSignature& OnError);

};