/*

	By Rama

*/

#pragma once

#include "Runtime/Engine/Classes/Components/InstancedStaticMeshComponent.h"
#include "VictoryISM.generated.h"
 
UCLASS()
class AVictoryISM	: public AActor
{
	GENERATED_BODY()
public:
	 
	AVictoryISM(const FObjectInitializer& ObjectInitializer);
	  
	UPROPERTY(Category = "Joy ISM", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* Mesh; 
	
//~~~~~~~~~~~~~
//	  ISM
//~~~~~~~~~~~~~
public:
	virtual void BeginPlay() override;
	
};

