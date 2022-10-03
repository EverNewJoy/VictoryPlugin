// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UnrealWebSocket.h"
#include "UnrealWebSocketsBPLibrary.generated.h"

UCLASS()
class UUnrealWebSocketsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Create WebSocket", Keywords = "Create", DefaultToSelf="Owner", HidePin="Owner"), Category = "Unreal WebSockets")
	static void CreateWebSocket(UObject* Owner, const FString Url, UUnrealWebSocket*& WebSocket);

};
