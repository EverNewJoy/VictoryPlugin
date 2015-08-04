/*

	By Rama

*/
#pragma once

#include "VictoryTMapComp.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(VictoryTMapLog, Log, All);

UCLASS(ClassGroup=VictoryBPLibrary, meta=(BlueprintSpawnableComponent))
class UVictoryTMapComp : public UActorComponent
{
	GENERATED_BODY()
public:
	UVictoryTMapComp(const FObjectInitializer& ObjectInitializer);
	
//Add
public:
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void String_Actor__AddPair(FString Key, AActor* Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void String_String__AddPair(FString Key, FString Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void String_Int__AddPair(FString Key, int32 Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void String_Vector__AddPair(FString Key, FVector Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void String_Rotator__AddPair(FString Key, FRotator Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void Int_Vector__AddPair(int32 Key, FVector Value);
	
	/** If a key already exists in the TMap, its current value is replaced with your new value! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Add")
	void Int_Float__AddPair(int32 Key, float Value);
	
//Get
public:
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	AActor* String_Actor__Get(FString Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	FString String_String__Get(FString Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	int32 String_Int__Get(FString Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	FVector String_Vector__Get(FString Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	FRotator String_Rotator__Get(FString Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	FVector Int_Vector__Get(int32 Key, bool& IsValid);
	
	/** Get the value associated with they key at fastest possible speed! <3 Rama */
	UFUNCTION(BlueprintPure,Category="VictoryBPLibrary|TMap Component|Get")
	float Int_Float__Get(int32 Key, bool& IsValid);
	
//Remove
public:
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void String_Actor__Remove(FString Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void String_String__Remove(FString Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void String_Int__Remove(FString Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void String_Vector__Remove(FString Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void String_Rotator__Remove(FString Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void Int_Vector__Remove(int32 Key);
	
	/** Removes the key and any associated value from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Remove")
	void Int_Float__Remove(int32 Key, float Value);
	
//Clear
public:
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void String_Actor__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void String_String__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void String_Int__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void String_Vector__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void String_Rotator__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void Int_Vector__Clear();
	
	/** Removes every entry from the TMap! <3 Rama */
	UFUNCTION(BlueprintCallable,Category="VictoryBPLibrary|TMap Component|Clear", meta = (Keywords = "Clear Empty Reset"))
	void Int_Float__Clear();
	
public:
	TMap<FString,AActor*> 	StringActor;
	TMap<FString,FString> 		StringString;
	TMap<FString,int32> 		StringInt;
	TMap<FString,FVector> 	StringVector;
	TMap<FString,FRotator> 	StringRotator;
	TMap<int32,FVector> 		IntVector;
	TMap<int32,float> 			IntFloat;
	
};