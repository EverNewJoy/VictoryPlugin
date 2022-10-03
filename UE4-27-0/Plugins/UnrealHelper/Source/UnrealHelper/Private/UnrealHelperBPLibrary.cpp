#include "UnrealHelperBPLibrary.h"
#include "UnrealHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "RenderUtils.h"
#include "Engine/World.h"
#include "HAL/IPlatformFileModule.h"
#include "IPlatformFilePak.h"
#include "AssetRegistryModule.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Serialization/ArrayReader.h"

UUnrealHelperBPLibrary::UUnrealHelperBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UUnrealHelperBPLibrary::GetStringFromGameConfig(const FString Section, const FString Key, bool& Succeeded, FString& String)
{
	if (!GConfig)
	{
		return;
	}
	
	Succeeded = GConfig->GetString(
		*Section,
		*Key,
		String,
		GGameIni
	);
}

FString UUnrealHelperBPLibrary::RunCommand(FString Cmd, FString Args, FString& StdErr)
{
	FString Result;
	int32 ExitCode;
	// example (on mac)
	// FPlatformProcess::ExecProcess(TEXT("/usr/bin/curl"), TEXT("-XPOST localhost:4443"), &ExitCode, &Result, &StdErr);
	// this may be poorly implemented in windows, running sleep instead of blocking on read.
	FPlatformProcess::ExecProcess(*Cmd, *Args, &ExitCode, &Result, &StdErr);
	return Result;
}

void UUnrealHelperBPLibrary::ListFiles(FString Directory, FString FileExtension, TArray<FString>& Files)
{
	IFileManager& FileManager = IFileManager::Get();
	FPaths::NormalizeDirectoryName(Directory);
	FileManager.FindFiles(Files, *Directory, *FileExtension);
}

void UUnrealHelperBPLibrary::ExportRenderTargetAsBuffer(UTextureRenderTarget2D* RenderTarget, bool& bSuccess, TArray<uint8>& Buffer)
{
	// From FImageUtils
	bSuccess = false;

	if(RenderTarget->GetFormat() != PF_B8G8R8A8)
	{
		return;
	}

	check(RenderTarget != nullptr);

	FRenderTarget* SafeRenderTarget = RenderTarget->GameThread_GetRenderTargetResource();

	FIntPoint Size = SafeRenderTarget->GetSizeXY();

	TArray<uint8> RawData;
	EPixelFormat Format = RenderTarget->GetFormat();
	int32 ImageBytes = 32 * Size.X * Size.Y; // CalculateImageBytes(Size.X, Size.Y, 0, Format);
	RawData.AddUninitialized(ImageBytes);
	bSuccess = SafeRenderTarget->ReadPixelsPtr((FColor*) RawData.GetData());

	IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

	TSharedPtr<IImageWrapper> PNGImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	PNGImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), Size.X, Size.Y, ERGBFormat::BGRA, 8);

	Buffer = PNGImageWrapper->GetCompressed(100); 

	bSuccess = true;

}

void UUnrealHelperBPLibrary::GetClipboardAsString(FString& String)
{
	FPlatformApplicationMisc::ClipboardPaste(String);
}

void UUnrealHelperBPLibrary::MountPak(const FString PakFilename, const FString ProjectFolder, const FString ContentFolder)
{
	// Asset Registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Virtual Filesystem
	const TCHAR* PakTypeName = FPakPlatformFile::GetTypeName();

	FPlatformFileManager* PlatformFileManager = &FPlatformFileManager::Get();
	IPlatformFile* PlatformFile = PlatformFileManager->FindPlatformFile(PakTypeName);

	FPakPlatformFile* PakPlatformFile = static_cast<FPakPlatformFile*>(PlatformFile);
	if (PakPlatformFile == nullptr)
	{
		PakPlatformFile = static_cast<FPakPlatformFile*>(PlatformFileManager->GetPlatformFile(PakTypeName)); // new FPakPlatformFile();
		PakPlatformFile->Initialize(&PlatformFileManager->GetPlatformFile(), PakTypeName);
		PakPlatformFile->InitializeNewAsyncIO();
		PlatformFileManager->SetPlatformFile(*PakPlatformFile);
	}

    // Mount Pak at /GRACE/<PakBaseFilename>/ then link appropriately to /Game/<ContentFolder>/
	// if(PakFile->GetIsMounted()) UnRegisterMountPoint

	FString StandardFilename(PakFilename);
	FPaths::MakeStandardFilename(StandardFilename);
	FString BaseFilename = FPaths::GetBaseFilename(StandardFilename);
	FString MountPoint = FString::Format(TEXT("/GRACE/{0}/"), {BaseFilename});
	FString IgnoreEngine = FString::Format(TEXT("{0}Engine/"),  {MountPoint});
    FString OptionalProjectFolder = ProjectFolder.IsEmpty() ? ProjectFolder : FString::Format(TEXT("{0}/"), {ProjectFolder});
	FString PakContentFolder = FString::Format(TEXT("/{0}Content/{1}/"), {OptionalProjectFolder, ContentFolder});
	FString GraceContentMount = FString::Format(TEXT("{0}{1}Content/{2}/"), {MountPoint, OptionalProjectFolder, ContentFolder});
	FString GameContentFolder = FString::Format(TEXT("/Game/{0}/"), {ContentFolder});

	if (!FPaths::FileExists(*StandardFilename))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find pak file: %s"), *StandardFilename);
		return;
	}

	FPakFile* PakFile = new FPakFile(PakPlatformFile, *StandardFilename, false);
	if (!PakFile->IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid pak file: %s"), *StandardFilename);
		return;
	}

	PakFile->SetMountPoint(*MountPoint);

    if(!PakFile->DirectoryExistsInPruned(*GraceContentMount))
	{
		UE_LOG(LogTemp, Warning, TEXT("Content folder not found in Pak File: %s"), *GraceContentMount);
		return;
	}

    if (!PakPlatformFile->Mount(*PakFilename, 0, *MountPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to mount: %s"), *MountPoint);
		return;
	}

	FPackageName::RegisterMountPoint(GameContentFolder, GraceContentMount);
	TArray< FString > ContentPaths;
	ContentPaths.Add(GameContentFolder);
	AssetRegistry.ScanPathsSynchronous(ContentPaths);
}