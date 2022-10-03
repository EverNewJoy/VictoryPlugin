#include "UnrealWebSocketsBPLibrary.h"
#include "UnrealWebSockets.h"

UUnrealWebSocketsBPLibrary::UUnrealWebSocketsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
void UUnrealWebSocketsBPLibrary::CreateWebSocket(UObject* Owner, const FString Url, UUnrealWebSocket*& WebSocket)
{
	WebSocket = NewObject<UUnrealWebSocket>(Owner);
	WebSocket->Connect(Url);
}

