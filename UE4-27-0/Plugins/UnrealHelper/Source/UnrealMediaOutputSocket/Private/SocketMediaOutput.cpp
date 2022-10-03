#include "SocketMediaOutput.h"
#include "SocketMediaCapture.h"

#include "Misc/Paths.h"
#include "UnrealEngine.h"

// Really not fun methods of finding the viewport

#include "Slate/SceneViewport.h"
#include "Engine/GameEngine.h"
#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#endif

// MediaCaptureDetails was stashed in MediaOutput.cpp
USocketMediaOutput::USocketMediaOutput()
	: Super()
{
}

bool USocketMediaOutput::FindSceneViewportAndLevel(TSharedPtr<FSceneViewport> &OutSceneViewport) const
{
#if WITH_EDITOR
	if (GIsEditor)
	{
		for (const FWorldContext &Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				// UEditorEngine::GetPrivateStaticClass missing from link; plsfix
				// UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
				UEditorEngine *EditorEngine = (UEditorEngine *)(GEngine);
				FSlatePlayInEditorInfo &Info = EditorEngine->SlatePlayInEditorMap.FindChecked(Context.ContextHandle);
				if (Info.SlatePlayInEditorWindowViewport.IsValid())
				{
					OutSceneViewport = Info.SlatePlayInEditorWindowViewport;
					return true;
				}
			}
		}
		return false;
	}
	else
#endif
	{
		UGameEngine *GameEngine = CastChecked<UGameEngine>(GEngine);
		OutSceneViewport = GameEngine->SceneViewport;
		return true;
	}
}

// TODO: Add initialize option to check for resize, look into resize options
bool USocketMediaOutput::Validate(FString &OutFailureReason) const
{
	return true;
}

/*
 * Hard coded the logic:
 * Uncertain as to why UMediaOutput::RequestCaptureSourceSize (FIntPoint::ZeroValue)
 * did not fall back to FoundSceneViewport->GetSizeXY
 * e.g. (GetRequestedSize() == UMediaOutput::RequestCaptureSourceSize)
 */
FIntPoint USocketMediaOutput::GetRequestedSize() const
{
	TSharedPtr<FSceneViewport> FoundSceneViewport;
	if (FindSceneViewportAndLevel(FoundSceneViewport))
	{
		FIntPoint Size = FoundSceneViewport->GetRenderTargetTextureSizeXY();
		// MediaCapture should have handled this, but does not seem to in 4.22
		// FIntPoint SizeIsh = FoundSceneViewport->GetSizeXY();
		return Size;
	}
	// assert(FIntPoint::ZeroValue == UMediaOutput::RequestCaptureSourceSize);
	return FIntPoint::ZeroValue;
}

EPixelFormat USocketMediaOutput::GetRequestedPixelFormat() const
{
	// Trying again for matte processing in a2rgb10
	// if(PixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	//{
	//	return EPixelFormat::PF_B8G8R8A8;
	// }

	// ValidateSceneViewport seems to require this:
	if (SourcePixelFormat == EPixelFormat::PF_Unknown)
	{
		static const auto CVarDefaultBackBufferPixelFormat = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DefaultBackBufferPixelFormat"));
		return EDefaultBackBufferPixelFormat::Convert2PixelFormat(EDefaultBackBufferPixelFormat::FromInt(CVarDefaultBackBufferPixelFormat->GetValueOnAnyThread()));
	}

	// Then CaptureSceneViewportImpl is run; which can update our Pixel Format to the correct one?

	// return EPixelFormat::PF_B8G8R8A8;
	// return EPixelFormat::PF_A2B10G10R10;
	/*
		if(SourcePixelFormat == EPixelFormat::PF_Unknown)
		{
			// CaptureImpl is not set - this is all one big workaround
			SourcePixelFormat = ((USocketMediaCapture*) CaptureImpl)->GetPixelFormat();
		}
	*/
	return SourcePixelFormat;
	// This worked nicely except that Standalone mode can switch from rgb10 to rgb8
}

EMediaCaptureConversionOperation USocketMediaOutput::GetConversionOperation(EMediaCaptureSourceType InSourceType) const
{
	static const auto CVarDefaultBackBufferPixelFormat = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.DefaultBackBufferPixelFormat"));
	EPixelFormat SceneTargetFormat = EDefaultBackBufferPixelFormat::Convert2PixelFormat(EDefaultBackBufferPixelFormat::FromInt(CVarDefaultBackBufferPixelFormat->GetValueOnAnyThread()));

	// Same as the PNG / EXR encoder
	if (PixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		return EMediaCaptureConversionOperation::INVERT_ALPHA;
	}
	if (PixelFormat == ESocketMediaOutputPixelFormat::RGBA)
	{
		return EMediaCaptureConversionOperation::NONE;
	}
	// This triggers a pixel shader to compress/alter the format, e.g. rgba to uyuv.
	if (SceneTargetFormat == EPixelFormat::PF_A2B10G10R10)
	{
		if (PixelFormat == ESocketMediaOutputPixelFormat::V210)
		{
			UE_LOG(LogTemp, Warning, TEXT("V210 Capture Format"));
			return EMediaCaptureConversionOperation::RGB10_TO_YUVv210_10BIT;
		}
		UE_LOG(LogTemp, Warning, TEXT("UYVY Capture Format"));
		return EMediaCaptureConversionOperation::RGBA8_TO_YUV_8BIT;
		// UE 4.23+ EMediaCaptureConversionOperation::CUSTOM
	}
	if (SceneTargetFormat == EPixelFormat::PF_B8G8R8A8)
	{
		if (PixelFormat == ESocketMediaOutputPixelFormat::V210)
		{
			UE_LOG(LogTemp, Warning, TEXT("V210 Capture Format Selected but Backing is RGBA8 - Falling to UYUV"));
		}
		return EMediaCaptureConversionOperation::RGBA8_TO_YUV_8BIT;
	}
	return EMediaCaptureConversionOperation::NONE;
}

template <typename T>
static FString EnumToString(const FString &enumName, const T value)
{
	UEnum *pEnum = FindObject<UEnum>(ANY_PACKAGE, *enumName);
	return *(pEnum ? pEnum->GetNameStringByIndex(static_cast<uint8>(value)) : "null");
}

UMediaCapture *USocketMediaOutput::CreateMediaCaptureImpl()
{
	if (PixelFormat == ESocketMediaOutputPixelFormat::MATTE)
	{
		DesiredPixelFormat = EPixelFormat::PF_B8G8R8A8;
	}
	UMediaCapture *Result = NewObject<USocketMediaCapture>();
	if (Result)
	{
		Result->SetMediaOutput(this);
	}
	return Result;
}
