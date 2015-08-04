/*

	By Rama

*/

#pragma once

#include "JoyISM.generated.h"
 
UCLASS()
class AJoyISM	: public AActor
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, Category="JoyISM")
	UInstancedStaticMeshComponent* Mesh; 
	
//~~~~~~~~~~~~~
//	 Joy ISM
//~~~~~~~~~~~~~
public:		

public:
	virtual void BeginPlay() override;
	
};

