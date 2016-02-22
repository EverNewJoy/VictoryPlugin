/*

	By Rama

*/

#pragma once

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

