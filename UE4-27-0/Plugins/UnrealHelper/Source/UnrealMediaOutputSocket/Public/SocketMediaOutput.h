#pragma once

#include "FileMediaOutput.h"
#include "Engine/RendererSettings.h"
#include "Slate/SceneViewport.h"
#include "SocketMediaOutput.generated.h"

/*
 * UYVY422 is two pixels packed into 4 bytes
 * - Apple: 2yuv
 * - FFmpeg: -pixel_format uyvy422
 * - Unreal: RGBA8_TO_YUV_8BIT
 *
 * V210 is four pixels packed into 16 bites
 * - Apple: V210
 * - FFmpeg: -pixel_format yuv422p10le -c:v v210
 * -pix_fmt yuv422p10le -vcodec v210
 * - Unreal: RGB10_TO_YUVv210_10BIT
 *
 * RGBA is one pixel in 4 bytes with format by back buffer
 * Unreal: PF_A2B10G10R10
 * - Nvidia: NV_ENC_BUFFER_FORMAT_ABGR10
 * - FFmpeg: -pix_fmt x2rgb10
 * - Apple: R210
 * - Kona: R10K
 * Unreal: PF_B8G8R8A8
 * - Nvidia: NV_ENC_BUFFER_FORMAT_ABGR
 * - FFmpeg: -pixel_format bgra
 * For ffmpeg see ffmpeg -pix_fmts
 */

UENUM(BlueprintType)
enum class ESocketMediaOutputPixelFormat : uint8
{
	UYVY UMETA(DisplayName = "UYVY"),
	V210 UMETA(DisplayName = "V210"),
	RGBA UMETA(DisplayName = "RGBA"),
	MATTE UMETA(DisplayName = "MATTE")
};

/**
 * Output information for a socket media capture.
 */
UCLASS(BlueprintType)
class UNREALMEDIAOUTPUTSOCKET_API USocketMediaOutput : public UMediaOutput
{
	GENERATED_BODY()
public:
	USocketMediaOutput();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network", meta = (DisplayName = "IP Address", MakeStructureDefaultValue = "127.0.0.1"))
	FString IPAddress;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Network", meta = (ClampMin = "0", ClampMax = "65535", MakeStructureDefaultValue = "4445"))
	int32 Port;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (DisplayName = "No Background Process"))
	bool NoExecutable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (DisplayName = "Show Background Process Window"))
	bool NoHiddenExecutable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (DisplayName = "No FFMpeg Report"))
	bool NoReport;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (DisplayName = "No TCP Network Pipe"))
	bool NoNetwork;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (DisplayName = "No Async Queue"))
	bool NoAsync;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process", meta = (MakeStructureDefaultValue = "output.mkv"))
	FString OutputFilename;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process")
	FString ExecutablePath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Process")
	FString ExecutableParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Media", meta = (MakeStructureDefaultValue = "UYUV"))
	ESocketMediaOutputPixelFormat PixelFormat;

	// Kind of broken:
	EPixelFormat DesiredPixelFormat;

public:
	// primary viewport to match GetRequestedSize to back buffer resolution
	bool FindSceneViewportAndLevel(TSharedPtr<FSceneViewport> &OutSceneViewport) const;
	virtual bool Validate(FString &FailureReason) const override;
	virtual FIntPoint GetRequestedSize() const override;
	virtual EPixelFormat GetRequestedPixelFormat() const override;
	virtual EMediaCaptureConversionOperation GetConversionOperation(EMediaCaptureSourceType InSourceType) const override;

protected:
	virtual UMediaCapture *CreateMediaCaptureImpl() override;
	UMediaCapture *CreateMediaCapture();

public:
	// Work-around the pixel format check in MediaCapture
	EPixelFormat SourcePixelFormat;
};
