/*

	By Rama

*/
#include "VictoryEdEnginePCH.h"
#include "JoyISM.h"

//////////////////////////////////////////////////////////////////////////
// JoyISM

AJoyISM::AJoyISM(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RootComponent = Mesh = ObjectInitializer.CreateDefaultSubobject<UInstancedStaticMeshComponent>(this, "Ground Instanced Mesh");
} 

void AJoyISM::BeginPlay() 
{  
	Super::BeginPlay();
	//~~~~~~~~~
	 

}