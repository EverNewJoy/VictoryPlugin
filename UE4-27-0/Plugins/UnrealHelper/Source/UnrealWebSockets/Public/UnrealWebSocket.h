#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Modules/ModuleManager.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "UnrealWebSocket.generated.h"

/**
 * 
 */
// Compromise between Unreal and WebSocket API

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWebSocketOnOpenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebSocketOnClosedSignature, const FString&, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebSocketOnErrorSignature, const FString&, error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebSocketOnMessageSignature, const FString&, data);

UENUM(BlueprintType)
enum class EWebSocketReadyState : uint8 {
	CONNECTING       UMETA(DisplayName = "Connecting"),
	OPEN             UMETA(DisplayName = "Open"),
	CLOSING          UMETA(DisplayName = "Closing"),
	CLOSED           UMETA(DisplayName = "Closed")
};

UCLASS(BlueprintType)
class UNREALWEBSOCKETS_API UUnrealWebSocket : public UObject
{
	GENERATED_BODY()
private:
	
	TArray<FString> Protocols;

	UUnrealWebSocket(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitProperties() override;

	virtual void PostLoad() override;

	virtual void BeginDestroy() override;

public:

	TSharedPtr<IWebSocket> WebSocket;

	void Cleanup();

	void Connect(const FString& Url);

	UPROPERTY(BlueprintReadOnly)
	EWebSocketReadyState ReadyState;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close", Keywords = "Close", AutoCreateRefTerm="Reason"), Category = "WebSocket")
	void Close(int32 Code = 1000, const FString& Reason = "");

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send", Keywords = "Send"), Category = "WebSocket")
	void Send(const FString& Data);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Send Buffer", Keywords = "Send"), Category = "WebSocket")
	void SendBuffer(const TArray<uint8>& Data);

	UPROPERTY(BlueprintAssignable, Category = "Unreal WebSockets")
	FWebSocketOnClosedSignature OnClosed;

	UPROPERTY(BlueprintAssignable, Category = "Unreal WebSockets")
	FWebSocketOnErrorSignature OnError;

	UPROPERTY(BlueprintAssignable, Category = "Unreal WebSockets")
	FWebSocketOnOpenSignature OnOpen;

	UPROPERTY(BlueprintAssignable, Category = "Unreal WebSockets")
	FWebSocketOnMessageSignature OnMessage;


};
