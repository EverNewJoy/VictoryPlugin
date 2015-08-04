/*

	By Rama

*/

#include "VictoryBPLibraryPrivatePCH.h"
#include "VictoryTMapComp.h"
 
DEFINE_LOG_CATEGORY(VictoryTMapLog)

//////////////////////////////////////////////////////////////////////////
// UVictoryTMapComp

UVictoryTMapComp::UVictoryTMapComp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 
}

//~~~ Add ~~~
void UVictoryTMapComp::String_Actor__AddPair(FString Key, AActor* Value)
{
	StringActor.Add(Key,Value);
}

void UVictoryTMapComp::String_String__AddPair(FString Key, FString Value)
{
	StringString.Add(Key,Value);
}

void UVictoryTMapComp::String_Int__AddPair(FString Key, int32 Value)
{
	StringInt.Add(Key,Value);
}

void UVictoryTMapComp::String_Vector__AddPair(FString Key, FVector Value)
{
	StringVector.Add(Key,Value);
}
void UVictoryTMapComp::String_Rotator__AddPair(FString Key, FRotator Value)
{
	StringRotator.Add(Key,Value);
}

void UVictoryTMapComp::Int_Vector__AddPair(int32 Key, FVector Value)
{
	IntVector.Add(Key,Value);
}

void UVictoryTMapComp::Int_Float__AddPair(int32 Key, float Value)
{
	IntFloat.Add(Key,Value);
}

//~~~ Get ~~~
AActor* UVictoryTMapComp::String_Actor__Get(FString Key, bool& IsValid)
{
	IsValid = false;
	if(!StringActor.Contains(Key)) 
	{
		return nullptr;
	}
	IsValid = true;
	return StringActor[Key];
}

FString UVictoryTMapComp::String_String__Get(FString Key, bool& IsValid)
{
	IsValid = false;
	if(!StringString.Contains(Key)) 
	{
		return "";
	}
	IsValid = true;
	return StringString[Key];
}

int32 UVictoryTMapComp::String_Int__Get(FString Key, bool& IsValid)
{
	IsValid = false;
	if(!StringInt.Contains(Key)) 
	{
		return 0;
	}
	IsValid = true;
	return StringInt[Key];
}

FVector UVictoryTMapComp::String_Vector__Get(FString Key, bool& IsValid)
{
	IsValid = false;
	if(!StringVector.Contains(Key)) 
	{
		return FVector::ZeroVector;
	}
	IsValid = true;
	return StringVector[Key];
}
FRotator UVictoryTMapComp::String_Rotator__Get(FString Key, bool& IsValid)
{
	IsValid = false;
	if(!StringRotator.Contains(Key)) 
	{
		return FRotator::ZeroRotator;
	}
	IsValid = true;
	return StringRotator[Key];
}

FVector UVictoryTMapComp::Int_Vector__Get(int32 Key, bool& IsValid)
{
	IsValid = false;
	if(!IntVector.Contains(Key)) 
	{
		return FVector::ZeroVector;
	}
	IsValid = true;
	return IntVector[Key];
}

float UVictoryTMapComp::Int_Float__Get(int32 Key, bool& IsValid)
{
	IsValid = false;
	if(!IntFloat.Contains(Key)) 
	{
		return -1;
	}
	IsValid = true;
	return IntFloat[Key];
}

//~~~ Remove ~~~
void UVictoryTMapComp::String_Actor__Remove(FString Key)
{
	StringActor.Remove(Key);
}

void UVictoryTMapComp::String_String__Remove(FString Key)
{
	StringString.Remove(Key);
}
  
void UVictoryTMapComp::String_Int__Remove(FString Key)
{
	StringInt.Remove(Key);
}

void UVictoryTMapComp::String_Vector__Remove(FString Key)
{
	StringVector.Remove(Key);
}
void UVictoryTMapComp::String_Rotator__Remove(FString Key)
{
	StringRotator.Remove(Key);
}

void UVictoryTMapComp::Int_Vector__Remove(int32 Key)
{
	IntVector.Remove(Key);
}

void UVictoryTMapComp::Int_Float__Remove(int32 Key, float Value)
{
	IntFloat.Remove(Key);
}
	
//~~~ Clear ~~~
void UVictoryTMapComp::String_Actor__Clear()
{
	StringActor.Empty();
}
 
void UVictoryTMapComp::String_String__Clear()
{
	StringString.Empty();
}

void UVictoryTMapComp::String_Int__Clear()
{
	StringInt.Empty();
}

void UVictoryTMapComp::String_Vector__Clear()
{
	StringVector.Empty();
}
void UVictoryTMapComp::String_Rotator__Clear()
{
	StringRotator.Empty();
}
 
void UVictoryTMapComp::Int_Vector__Clear()
{
	IntVector.Empty();
}

void UVictoryTMapComp::Int_Float__Clear()
{
	IntFloat.Empty();
}
