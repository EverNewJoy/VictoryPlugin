/*

	By Rama

*/

#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryPC.h"
 
DEFINE_LOG_CATEGORY(VictoryPCLog)

//////////////////////////////////////////////////////////////////////////
// AVictoryPC

AVictoryPC::AVictoryPC(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

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
	TSharedRef < IHttpRequest > Request = Http->CreateRequest(); 
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
	this->VictoryPC_GetMyIP_DataReceived(Response->GetContentAsString());
}
 