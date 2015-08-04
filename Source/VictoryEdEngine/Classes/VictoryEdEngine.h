#pragma once

#include "UnrealEd.h" 

#include "JoyISM.h"
#include "VictoryEdAlignMode.h"
#include "VictoryEdEngine.generated.h"

USTRUCT()
struct FDropToClosestSurfaceData
{  
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	AStaticMeshActor* TheActor;
	
	UPROPERTY()
	FVector SurfaceLoc;
	
	UPROPERTY()
	float DistSquared;
	
	FDropToClosestSurfaceData()
	{
		DistSquared = 1000000000;
		SurfaceLoc = FVector::ZeroVector;
		TheActor = NULL;
	}
};
USTRUCT()
struct FVButton
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	int32 Vibe;
	
	UPROPERTY()
	FVector PointInWorld;
	
	UPROPERTY()
	float minX;
	
	UPROPERTY()
	float maxX;
	
	UPROPERTY()
	float minY;
	
	UPROPERTY()
	float maxY;
	
	FVButton()
	{
		Vibe = -1;
	}
};

class FMyEditorModeFactory : public IEditorModeFactory
{	
	public:
		class UVictoryEdEngine* VictoryEd;
		
		FMyEditorModeFactory();
		~FMyEditorModeFactory(); 
		
	public:
		virtual void OnSelectionChanged( FEditorModeTools& Tools, UObject* ItemUndergoingChange ) const override;
		virtual FEditorModeInfo GetModeInfo() 															const override;
		virtual TSharedRef<FEdMode> CreateMode() 														const override;
};
 
UCLASS(config=Engine)
class UVictoryEdEngine : public UUnrealEdEngine
{
	GENERATED_UCLASS_BODY()
		
	bool CreatedVictoryEdMode;

	//the user-selected mode prior to initiating VictoryEdMode
	FEditorModeID PrevModeID;

//Exec 
public:
	virtual bool Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar=*GLog ) override;
	void VictoryConsole();
	
//utility

//Assets
public:
	
	/*
	UPROPERTY()
	UStaticMesh* AssetSM_EngineCube;
	//StaticMesh'/Engine/EngineMeshes/Cube.Cube'
	
	
	UPROPERTY()
	UMaterial* ColorMat;
	
	UPROPERTY()
	UMaterialInstanceDynamic* ColorMatInst_GlowyBlue;
	
	UPROPERTY()
	UMaterialInstanceDynamic* ColorMatInst_GlowyRed;
	
	UPROPERTY()
	UMaterialInstanceDynamic* ColorMatInst_GlowyYellow;
	*/
	
//Font
public:
	UPROPERTY()
	UFont* VictoryEditorFont;
	//Font'/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField'
	
	FORCEINLINE UFont* GetVictoryFont()
	{
		if(VictoryEditorFont) return VictoryEditorFont;
		return GetStatsFont();
	}
	
	
//User Choices
public:
	uint8 VertexDisplayChoice;
	float CurrentVerticiesScale;
	uint8 DrawVerticiesMode; //0 = dont show, 1= show 1, 2 = show all
	
//Selection
public:
	AActor* VSelectedActor;
	AJoyISM* SelectedJoyISM;
	TArray<FVector> SelectionRelativeOffsets;
	void GetSelectedActorsRelativeOffsets();
	bool SelectedActorVerticiesNeedsUpdating;
	bool ClearSelectedVertex;
	
//core
public:
	static const FName VictoryEditorModeID;
	void SwitchToVictoryEdMode();
	
	FORCEINLINE void ExitVictoryAlignMode()
	{
		//Deactivate Mode
		GLevelEditorModeTools().DeactivateMode( VictoryEditorModeID );
		
		//Restore Previous Realtime State
		//GCurrentLevelEditingViewportClient->RestoreRealtime(true); //allow disable
		
		//Activate Previous Mode
		GLevelEditorModeTools().ActivateMode(PrevModeID);
	}
	
//tests
public:
	void RunTests();
	
protected:
	
	virtual void NoteSelectionChange() override;
	
	virtual void Tick(float DeltaSeconds, bool bIdleMode) override;
};