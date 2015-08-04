/*

	By Rama

*/

#pragma once

#include "VictoryISM.generated.h"
 
UCLASS()
class AVictoryISM	: public AActor
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, Category="ISM")
	UInstancedStaticMeshComponent* Mesh; 
	
//~~~~~~~~~~~~~
//	  ISM
//~~~~~~~~~~~~~
public:		

public:
	virtual void BeginPlay() override;
	
};

