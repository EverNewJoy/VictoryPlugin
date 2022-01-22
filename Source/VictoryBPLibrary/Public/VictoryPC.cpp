/*

	By Rama

*/

#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryPC.h"
#include "HttpModule.h"
#include "Components/AudioComponent.h"
#include "Interfaces/IHttpResponse.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(VictoryPCLog)

//////////////////////////////////////////////////////////////////////////
// AVictoryPC

AVictoryPC::AVictoryPC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


UAudioComponent* AVictoryPC::VictoryPlaySpeechSound(
	USoundBase* Sound
	,float VolumeMultiplier
	,float PitchMultiplier
	, float StartTime)
{

	UAudioComponent* Audio = UGameplayStatics::SpawnSound2D(this,Sound,VolumeMultiplier,PitchMultiplier,StartTime);
	if(Audio)
	{
		//Subtitle Delegate for You!
		//		<3 Rama
		Audio->OnQueueSubtitles.BindDynamic(this, &AVictoryPC::Subtitles_CPPDelegate);
	}      
	
	/*
		Note that the OnAudioFinished is BP assignable off of return of this node!
		
		//called when we finish playing audio, either because it played to completion or because a Stop() call turned it off early
		UPROPERTY(BlueprintAssignable)
		FOnAudioFinished OnAudioFinished;
	*/
	
	return Audio;
}
 
void AVictoryPC::Subtitles_CPPDelegate(const TArray<struct FSubtitleCue>& Subtitles, float CueDuration)
{
	TArray<FVictorySubtitleCue> VictorySubtitles; 
	for(const FSubtitleCue& Each : Subtitles)
	{
		VictorySubtitles.Add(FVictorySubtitleCue(Each.Text,Each.Time));
	} 
	  
	OnVictorySubtitlesQueued(VictorySubtitles,CueDuration);
}
 
//~~~

bool AVictoryPC::VictoryPC_GetMyIP_SendRequest()
{
	FHttpModule* Http = &FHttpModule::Get();
	
	if(!Http)
	{
		return false;
	}
	 
	if(!Http->IsHttpEnabled()) 
	{
		return false;
	} 
	//~~~~~~~~~~~~~~~~~~~
	
	FString TargetHost = "http://api.ipify.org";
	auto Request = Http->CreateRequest(); 
	Request->SetVerb("GET");
	Request->SetURL(TargetHost);
	Request->SetHeader("User-Agent", "VictoryBPLibrary/1.0");
	Request->SetHeader("Content-Type" ,"text/html");
 
	Request->OnProcessRequestComplete().BindUObject(this, &AVictoryPC::HTTPOnResponseReceived);
	if (!Request->ProcessRequest())
	{
		return false;
	}
	  
	return true;
}
	
void AVictoryPC::HTTPOnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString ResponseStr = "AVictoryPC::HTTPOnResponseReceived>>> Connection Error";
	if(bWasSuccessful) 
	{
		ResponseStr = Response->GetContentAsString();
	} 
	 
	this->VictoryPC_GetMyIP_DataReceived(ResponseStr);
}
 
