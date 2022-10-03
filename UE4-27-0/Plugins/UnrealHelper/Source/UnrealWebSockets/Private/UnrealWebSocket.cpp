#include "UnrealWebSocket.h"

UUnrealWebSocket::UUnrealWebSocket(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	WebSocket = nullptr;
	ReadyState = EWebSocketReadyState::CLOSED;
}


void UUnrealWebSocket::PostInitProperties()
{
	Super::PostInitProperties();
	FWebSocketsModule& Module = FModuleManager::LoadModuleChecked<FWebSocketsModule>(TEXT("WebSockets"));
	Protocols.Add(TEXT("ws"));
	Protocols.Add(TEXT("wss"));
}

void UUnrealWebSocket::PostLoad()
{
	Super::PostLoad();

}

void UUnrealWebSocket::Connect(const FString& Url)
{
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url, Protocols);

	// https://html.spec.whatwg.org/multipage/web-sockets.html#the-websocket-interface
	if (WebSocket.IsValid())
	{
		ReadyState = EWebSocketReadyState::CONNECTING;

		WebSocket->OnConnected().AddWeakLambda(this, [this]() -> void {

			ReadyState = EWebSocketReadyState::OPEN;
			OnOpen.Broadcast();

			});

		WebSocket->OnConnectionError().AddWeakLambda(this, [this](const FString& Error) -> void {

			ReadyState = EWebSocketReadyState::CLOSED;
			OnError.Broadcast(Error);

			});

		WebSocket->OnClosed().AddWeakLambda(this, [this](int32 StatusCode, const FString Reason, bool bWasClean) -> void {

			ReadyState = EWebSocketReadyState::CLOSED;
			OnClosed.Broadcast(Reason);
			Cleanup();

			});

		WebSocket->OnMessage().AddWeakLambda(this, [this](const FString& Message) -> void {

			OnMessage.Broadcast(Message);

			});

		WebSocket->Connect();
	}
	else
	{	
		ReadyState = EWebSocketReadyState::CLOSED;
		OnError.Broadcast(FString("Unable to Create WebSocket"));
	}

}

void UUnrealWebSocket::Close(int32 Code, const FString& Reason)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		ReadyState = EWebSocketReadyState::CLOSING;
		WebSocket->Close();
	}
}

void UUnrealWebSocket::Send(const FString& Data)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Data);
	}
}

void UUnrealWebSocket::SendBuffer(const TArray<uint8>& Data)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		WebSocket->Send(Data.GetData(), Data.GetAllocatedSize(), true);
	}
}

void UUnrealWebSocket::Cleanup()
{
	if (WebSocket.IsValid())
	{
		WebSocket->OnConnected().RemoveAll(this);
		WebSocket->OnConnectionError().RemoveAll(this);
		WebSocket->OnMessage().RemoveAll(this);
		WebSocket->OnClosed().RemoveAll(this);
		WebSocket.Reset();
	}
}

void UUnrealWebSocket::BeginDestroy()
{
	Super::BeginDestroy();
}
