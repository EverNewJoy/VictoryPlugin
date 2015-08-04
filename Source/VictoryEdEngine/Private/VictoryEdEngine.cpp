//Unreal Editor Fun With Rama

#include "VictoryEdEnginePCH.h"

#include "Slate.h"

#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "VictoryEdEngine"

const FName UVictoryEdEngine::VictoryEditorModeID = FName("VictoryEditorMode");


FMyEditorModeFactory::FMyEditorModeFactory()
{
	
}
FMyEditorModeFactory::~FMyEditorModeFactory()
{
	 
}
void FMyEditorModeFactory::OnSelectionChanged( FEditorModeTools& Tools, UObject* ItemUndergoingChange ) const
{
	//Super
	IEditorModeFactory::OnSelectionChanged(Tools, ItemUndergoingChange);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	/*
	USelection* Selection = GEditor->GetSelectedActors();

	if (ItemUndergoingChange != NULL && ItemUndergoingChange->IsSelected())
	{
		AActor* SelectedActor = Cast<AActor>(ItemUndergoingChange);
		if (SelectedActor != NULL)
		{
			UPrimitiveComponent* PC = SelectedActor->GetRootPrimitiveComponent();
			if (PC != NULL && PC->BodyInstance.bSimulatePhysics)
			{
				Tools.ActivateMode(FBuiltinEditorModes::EM_Physics);
				return;
			}
		}
	}
	else if (ItemUndergoingChange != NULL && !ItemUndergoingChange->IsA(USelection::StaticClass()))
	{
		Tools.DeactivateMode(FBuiltinEditorModes::EM_Physics);
	}
	*/
	
}
FEditorModeInfo FMyEditorModeFactory::GetModeInfo() 	const
{
	return FEditorModeInfo(FBuiltinEditorModes::EM_Physics, NSLOCTEXT("EditorModes", "Rama Vertex Snap Mode", "Rama Vertex Snap Mode"));
}
TSharedRef<FEdMode> FMyEditorModeFactory::CreateMode() const
{
	FVictoryEdAlignMode* EdPtr = new FVictoryEdAlignMode;
	
	//Joy Init!
	EdPtr->JoyInit(VictoryEd); 	//Victory Ed Set Before CreateMode() is called.
									//CreateMode() is called when the mode is registered
	
	return MakeShareable( EdPtr );
}



UVictoryEdEngine::UVictoryEdEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentVerticiesScale = 12;
	VertexDisplayChoice = 0; //stars
	DrawVerticiesMode = 2; //show all
	 
	CreatedVictoryEdMode = false;
	
	PrevModeID = FBuiltinEditorModes::EM_Default;
	
	//Font
	static ConstructorHelpers::FObjectFinder<UFont> TheFontOb(TEXT("Font'/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField'"));
	VictoryEditorFont = (UFont*)TheFontOb.Object;
}

void UVictoryEdEngine::SwitchToVictoryEdMode()
{
	//Create it if it not exist yet
	if(!CreatedVictoryEdMode)
	{
		//Create
		TSharedRef<FMyEditorModeFactory> Factory = MakeShareable( new FMyEditorModeFactory );
		
		//Essential Link
		Factory->VictoryEd = this;
		
		//Register
 		FEditorModeRegistry::Get().RegisterMode( VictoryEditorModeID, Factory );
		
		//UE_LOG(Victory,Error, TEXT("RAMA VERTEX SNAP WAS JUST CREATED!!!: %s"), *VictoryEditorModeID.ToString() );
		
		CreatedVictoryEdMode = true;
	}
	
	//~~~ Store Previous Editor Mode ~~~
	TArray<FEdMode*> OutActiveModes;
	FEdMode* CurMode = NULL;
	GLevelEditorModeTools().GetActiveModes( OutActiveModes );
	
	for(int32 Itr = 0; Itr < OutActiveModes.Num(); Itr++)
	{
		CurMode = OutActiveModes[Itr];
		if(!CurMode) continue;
		if(CurMode->GetID() == VictoryEditorModeID ) continue;
			
		PrevModeID = CurMode->GetID();
		//UE_LOG(Victory,Warning, TEXT("Previous Editor Mode: %s"), *CurMode->GetID().ToString() );
	}
		
	//Activate Victory Editor Mode
	GLevelEditorModeTools().ActivateMode(VictoryEditorModeID);
}

bool UVictoryEdEngine::Exec( UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	bool bHandled = Super::Exec(InWorld,Cmd,Ar);
	//~~~~~~~~~~~~~~~
	
	if ( FParse::Command(&Cmd,TEXT("VictoryConsole")) )
	{
		VictoryConsole();
		return true;
	} 
	
	return bHandled;
}
void UVictoryEdEngine::VictoryConsole()
{  
	UE_LOG(Victory, Warning, TEXT("Victory!!!!"));
}

void UVictoryEdEngine::RunTests()
{
	//tests
}


void UVictoryEdEngine::GetSelectedActorsRelativeOffsets()
{
	SelectionRelativeOffsets.Empty();
	//~~~~~~~~~~~~~~~~~~~~~~~~
	
	//Get the Relative Offsets
	const FVector Origin = VSelectedActor->GetActorLocation();
	
	AActor* VSelectItrActor;
	for(FSelectionIterator VSelectItr = GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		VSelectItrActor = Cast<AActor>(*VSelectItr);
		if(!VSelectItrActor) continue;
		//~~~~~~~~~~~~~~~~~~
		
		SelectionRelativeOffsets.Add(VSelectItrActor->GetActorLocation() - Origin );
		//UE_LOG(Victory, Error, TEXT("Relative offset %s"), *SelectionRelativeOffsets[SelectionRelativeOffsets.Num() - 1].ToString());
	};
}
void UVictoryEdEngine::NoteSelectionChange()
{
	Super::NoteSelectionChange();
	//~~~~~~~~~~~~~~~~~
	
	if(!GetSelectedActors()) return;
	AActor* SelectedActor = Cast<AActor>(GetSelectedActors()->GetTop(AActor::StaticClass()));
	if(!SelectedActor) return;
	//~~~~~~~~~~~~~~~
	
	 
	//Not the Same?
	if(SelectedActor != VSelectedActor)
	{
		ClearSelectedVertex = true;
	}
	
	//Update
	VSelectedActor = SelectedActor;
	SelectedJoyISM = Cast<AJoyISM>(SelectedActor);
	
	if(!VSelectedActor) return;
	  
	//Always Refresh
	SelectedActorVerticiesNeedsUpdating = true;
	
	//For use with multi-select moves
	GetSelectedActorsRelativeOffsets();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 
	//Only Activate for Static Mesh Actors Currently
	if(SelectedActor->IsA(AStaticMeshActor::StaticClass()) || SelectedActor->IsA(AJoyISM::StaticClass()))
	{
		//Switch to Victory Align Mode if not active already
		if(!GLevelEditorModeTools().IsModeActive(VictoryEditorModeID)) 
			SwitchToVictoryEdMode();
	}
	else
	{
		//Deactivate Mode for Non-Static Mesh Actors
		ExitVictoryAlignMode();
	}
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//RunTests();
	
	//UE_LOG(Victory, Warning, TEXT("New Selected Actor %s"), *SelectedActor->GetName() );
	//UE_LOG(Victory, Warning, TEXT("New Selected Actor Location %s"), *SelectedActor->GetActorLocation().ToString() );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void UVictoryEdEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds,bIdleMode);
	 
}