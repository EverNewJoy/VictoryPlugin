/*

	By Rama

*/
#pragma once

//HTTP
#include "Http.h"

#include "Runtime/Engine/Classes/Gameframework/PlayerController.h"
#include "VictoryPC.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(VictoryPCLog, Log, All);


//Exposing the UE4 Subtitle system for Solus
// <3 Rama
USTRUCT(BlueprintType)
struct FVictorySubtitleCue
{
	GENERATED_USTRUCT_BODY()

	/** The text to appear in the subtitle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SubtitleCue)
	FText SubtitleText;

	/** The time at which the subtitle is to be displayed, in seconds relative to the beginning of the line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SubtitleCue)
	float Time;

	FVictorySubtitleCue()
		: Time(0)
	{ } 
	FVictorySubtitleCue(const FText& InText, float InTime)
	: SubtitleText(InText)
	, Time(InTime)
	{}
};

UCLASS()
class VICTORYBPLIBRARY_API AVictoryPC : public APlayerController
{
	GENERATED_BODY()

public:
	AVictoryPC(const FObjectInitializer& ObjectInitializer);
	
	/** 
	* When the sound is played OnVictorySubtitlesQueued will be called with the subtitles!
	* You can bind an event off of the audio component for OnAudioFinished to know hwen the sound is done! 
	*/ 
	UFUNCTION(Category="Victory Subtitles", BlueprintCallable, BlueprintCosmetic, meta=( UnsafeDuringActorConstruction = "true", Keywords = "play" ))
	UAudioComponent* VictoryPlaySpeechSound(class USoundBase* Sound, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f);
	
	UFUNCTION(Category="Victory Subtitles", BlueprintImplementableEvent)
	void OnVictorySubtitlesQueued(const TArray<struct FVictorySubtitleCue>& VictorySubtitles, float CueDuration);
	
	UFUNCTION()
	void Subtitles_CPPDelegate(const TArray<struct FSubtitleCue>& VictorySubtitles, float CueDuration);
	
public:
	/** This node relies on http://api.ipify.org, so if this node ever stops working, check out http://api.ipify.org.  Returns false if the operation could not occur because HTTP module was not loaded or unable to process request. */
	UFUNCTION(BlueprintCallable, Category="Victory PC")
	bool VictoryPC_GetMyIP_SendRequest();
	
	/** Implement this event to receive your IP once the request is processed! This requires that your computer has a live internet connection */
	UFUNCTION(BlueprintImplementableEvent, Category = "Victory PC", meta = (DisplayName = "Victory PC ~ GetMyIP ~ Data Received!"))
	void VictoryPC_GetMyIP_DataReceived(const FString& YourIP);
	
	void HTTPOnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};