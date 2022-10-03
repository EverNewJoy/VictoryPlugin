// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Engine/TextureRenderTarget2D.h"
#include "IImageWrapper.h"
#include "HAL/PlatformApplicationMisc.h"
#include "HAL/FileManager.h"
#include "UnrealHelperBPLibrary.generated.h"

// class UTextureRenderTarget2D;

UCLASS()
class UUnrealHelperBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get String from Game Config", Keywords = "Config"), Category = "Unreal Helper BP Library")
	static void GetStringFromGameConfig(const FString Section, const FString Key, bool& Succeeded, FString& String);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Project Version from Game Config", Keywords = "Config"), Category = "Unreal Helper BP Library")
	static void GetProjectVersion(FString& ProjectVersion);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Float as String Rounded to Precision", Keywords = "Math"), Category = "Unreal Helper BP Library")
	static void GetFloatAsStringWithPrecision(float InFloat, FString& ReturnValue, int32 Precision = 2, bool IncludeLeadingZero = true);

	// Intended to block execution of rendering while it waits.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Blocking External Command", Keywords = "Shell"), Category = "Unreal Helper BP Library")
	static FString RunCommand(FString Cmd, FString Params, FString& StdErr);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "List Files", Keywords = "File System"), Category = "Unreal Helper BP Library")
	static void ListFiles(FString Directory, FString FileExtension, TArray<FString>& Files);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Export Render Target as Buffer", Keywords = "Media"), Category = "Unreal Helper BP Library")
	static void ExportRenderTargetAsBuffer(UTextureRenderTarget2D* RenderTarget, bool& bSuccess, TArray<uint8>& Buffer);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Clipboard as String", Keywords = "Clipboard"), Category = "Unreal Helper BP Library")
	static void GetClipboardAsString(FString& String);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Mount Pak", Keywords = "File System"), Category = "Unreal Helper BP Library")
	static void MountPak(const FString PakFilename, const FString ProjectFolder, const FString ContentFolder);

};
