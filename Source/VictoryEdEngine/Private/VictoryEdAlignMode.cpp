// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.

//Victory Alignment Mode

//#include "VictoryGame.h"
#include "VictoryEdEnginePCH.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "VictoryVertexSnapEditor"

//~~~~~~~~~~~~~~~~

#define CHECK_VSELECTED if(!VictoryEngine) return; if(!VictoryEngine->VSelectedActor) return;





//~~~

#define BOOLSTR(TheBool)  ( (TheBool) ? FString("true") : FString("false") )

//~~~

//Current Class Name + Function Name where this is called!
#define JOYSTR_CUR_CLASS_FUNC (FString(__FUNCTION__))

//Current Class where this is called!
#define JOYSTR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

//Current Function Name where this is called!
#define JOYSTR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))
  
//Current Line Number in the code where this is called!
#define JOYSTR_CUR_LINE  (FString::FromInt(__LINE__))

//Current Class and Line Number where this is called!
#define JOYSTR_CUR_CLASS_LINE (JOYSTR_CUR_CLASS + "(" + JOYSTR_CUR_LINE + ")")
  
//Current Function Signature where this is called!
#define JOYSTR_CUR_FUNCSIG (FString(__FUNCSIG__))

#define V_LOG(Param1) 			UE_LOG(Victory,Warning,TEXT("%s: %s"), *JOYSTR_CUR_CLASS_LINE, *FString(Param1))
#define V_LOG2(Param1,Param2) 	UE_LOG(Victory,Warning,TEXT("%s: %s %s"), *JOYSTR_CUR_CLASS_LINE, *FString(Param1),*FString(Param2))
#define V_LOGF(Param1,Param2) 	UE_LOG(Victory,Warning,TEXT("%s: %s %f"), *JOYSTR_CUR_CLASS_LINE, *FString(Param1),float(Param2))
 








//~~~ Display Choices ~~~
#define VERTEX_DISPLAY_STARS 		0
#define VERTEX_DISPLAY_3DBOX 		1
#define VERTEX_DISPLAY_RECT 			2
#define VERTEX_DISPLAY_DIAMOND3D 	3
#define VERTEX_DISPLAY_SPHERE 		4
#define VERTEX_SELECTED_MULT		1.333
#define VERTEX_SHAPE_MULT			0.8
//~~~ Defines ~~~

//MAX
#define MAX_VERTEX_COUNT_FOR_DRAWING 10000
#define MAX_VERTEX_COUNT_FOR_DRAWING_SPHERES 5000

//Speeds
#define XYZSPEEDMULT 20

//3D
#define DEFAULT_INSTANT_MOVE_DISTANCE 2048
#define CURSOR_DELTA_DISTANCE_CALC 2048
//HUD
#define VICTORY_TITLE_HEIGHT 38
#define VICTORY_TEXT_HEIGHT 24

//Color
#define RED 		0
#define BLUE		1
#define YELLOW 3

//TIME
#define VICTORY_TITLE_VISIBLE_DURATION 2

//Button Vibes
#define BUTTON_VIBE_VICTORYHOTKEYS 		0

//~~~ Const ~~~
const FLinearColor FVictoryEdAlignMode::RV_Red = FLinearColor(1,0,0,1);
const FLinearColor FVictoryEdAlignMode::RV_Yellow = FLinearColor(1,1,0,1);
const FLinearColor FVictoryEdAlignMode::RV_Blue= FLinearColor(0,0,1,1);
const FColor FVictoryEdAlignMode::RV_VRed = FColor(255,0,0,255);
const FColor FVictoryEdAlignMode::RV_VBlue = FColor(0,0,255,255);
const FColor FVictoryEdAlignMode::RV_VYellow = FColor(255,255,0,255);
	

FVictoryEdAlignMode::FVictoryEdAlignMode()
{
	//FEdMode::FEdMode(); //call super?
}
FVictoryEdAlignMode::~FVictoryEdAlignMode()
{	
}

//INIT
void FVictoryEdAlignMode::JoyInit(UVictoryEdEngine* EnginePtr)
{
	//ID = FName("VictoryEditorMode");
	
	
	//Always Initialize Your Pointers!
	SelectedVertexBuffer = nullptr;
	HighlightedVertexBuffer = nullptr;

	  
	//VictoryEngine
	VictoryEngine = EnginePtr;
	
	//Victory Buttons
	RefreshVictoryButtons();

	//Enable Realtime
	ReEntering = true;
	
	//~~~
	
	UsingMouseInstantMove = false;
	
	
	//~~~
	
	//Get an actor, not sure if this matters
	//TObjectIterator< AActor> ActorItr;
	
	//Traces
	RV_TraceParams = FCollisionQueryParams(FName(TEXT("HUDRMBDown")), true, NULL);
	RV_TraceParams.bTraceComplex = true;
	//RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
}

void FVictoryEdAlignMode::RefreshVictoryButtons()
{
	VictoryButtons.Empty();
	//~~~~~~~~~~~~
	
	FVButton NewButton;
	NewButton.Vibe 	= BUTTON_VIBE_VICTORYHOTKEYS;
	NewButton.minX	=	10;
	NewButton.maxX	=	200;
	NewButton.minY	=	VICTORY_TITLE_HEIGHT;
	NewButton.maxY	=	VICTORY_TITLE_HEIGHT + VICTORY_TEXT_HEIGHT;
	VictoryButtons.Add(NewButton);
}

//EXIT
void FVictoryEdAlignMode::Exit()
{
	UsingMouseInstantMove = false;
}
//ENTER
void FVictoryEdAlignMode::Enter()
{
	//FEdMode::Enter();
	//~~~~~~~~
	
	//Victory Title Appears
	VictoryTitleAppears();
	ReEntering = true;
	
	//Clear Snap Key!
	SnapKeyPressed = false;
	
	//Clear keys
	MinusIsDown = false;
	PlusIsDown = false;
	ShiftDown = false;
	
	//Get an actor, not sure if this matters
	//TObjectIterator< AActor> ActorItr;
	
	//Trace
	RV_TraceParams = FCollisionQueryParams(FName(TEXT("HUDRMBDown")), true, NULL);
	
	//~~~~~~~~~~~~~~~~
	

}

void FVictoryEdAlignMode::VictoryTitleAppears()
{
	CHECK_VSELECTED
	 
	if(VictoryEngine->DrawVerticiesMode != 1) return;
	//~~~~~~~~~~~~~~~~~~~~~~
	
	VictoryTitleAppearTime = FDateTime::Now();
	VictoryTitleAlpha = 1;
	VictoryTitleVisible = true;
	FadeInVictoryTitle = false;
}
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void FVictoryEdAlignMode::ReverseJoyISM()
{
	  
	CHECK_VSELECTED
	
	//Current World
	UWorld* World = GetWorld();
	if(!World) return;
	//~~~~~~~~~~~~~~
	
	const FScopedTransaction Transaction(LOCTEXT("RevertSelectedInstancedStaticMesh", "Revert Instanced Static Mesh Actor to individual Static Mesh Actors"));
	
	AJoyISM* JoyISM = Cast<AJoyISM>(VictoryEngine->VSelectedActor);
	if(!JoyISM) 
	{
		//Not a JoyISM!
		return;
	}
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail 		= true;
	SpawnInfo.bDeferConstruction 	= false;
	
	TArray<FTransform> Transforms;
	int32 Total = JoyISM->Mesh->GetInstanceCount();
	for(int32 v = 0; v < Total; v++)
	{
		FTransform Transform;
		JoyISM->Mesh->GetInstanceTransform(v,Transform,true); //world space!
		Transforms.Add(Transform);
	} 
	
	//for each Transform in world space
	for(FTransform& Each : Transforms)
	{ 
		AStaticMeshActor* EachSMA = World->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(), 
			JoyISM->GetActorLocation() , JoyISM->GetActorRotation(), SpawnInfo 
		);
		
		if(!EachSMA) continue;
		//~~~~~~~~~~~
		
		//Mesh Comp
		UStaticMeshComponent* Mesh = EachSMA->GetStaticMeshComponent();
		
		//Movable! 
		Mesh->SetMobility(EComponentMobility::Movable);
		
		//Mesh
		Mesh->SetStaticMesh(JoyISM->Mesh->StaticMesh);
		  
		//Materials
		const int32 MatTotal = JoyISM->Mesh->GetNumMaterials();
		for(int32 v = 0; v < MatTotal; v++)
		{
				Mesh->SetMaterial(v,JoyISM->Mesh->GetMaterial(v));
		}
		
		//Transform
		EachSMA->SetActorTransform(Each);
		
		//Copy Layers!
		EachSMA->Layers = JoyISM->Layers;
		
		//Set Folder
		EachSMA->SetFolderPath(JoyISM->GetFolderPath());
	
		//Done
		EachSMA->Modify();
		
		GEditor->SelectActor( EachSMA, /*InSelected=*/true, /*bNotify=*/true );
	}

	GEditor->SelectActor( JoyISM, /*InSelected=*/false, /*bNotify=*/true );
	
	//Pre Delete
	JoyISM->Modify();
	
	//Destroy
	JoyISM->Destroy(); 
}
void FVictoryEdAlignMode::PerformJoyISM()
{
	CHECK_VSELECTED
	
	const FScopedTransaction Transaction(LOCTEXT("CreateInstancedStaticMeshFromSelection", "Create Instanced Static Mesh Actor From Selected Actors"));
	
	//Get Static mesh from root selected
	
	//ignore actors that dont have same static mesh as root
	
	AStaticMeshActor* RootSMA = Cast<AStaticMeshActor>(VictoryEngine->VSelectedActor);
	if(!RootSMA) 
	{
		//Not valid
		return;
	}
	if(RootSMA->IsA(AJoyISM::StaticClass()))
	{
		//Dont perform on existing JoyISM !
		return;
	}
	//Root SM
	UStaticMeshComponent* RootSMC = RootSMA->GetStaticMeshComponent();
	UStaticMesh* RootSM = RootSMC->StaticMesh; 
	
	TArray<FTransform> Transforms;
	
	TArray<AActor*> ToDestroy;
	//For Each Selected Actor
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		//Only works with static mesh actors
		AStaticMeshActor* EachSMA = Cast<AStaticMeshActor>(*VSelectItr);
		if(!EachSMA) continue;
		//~~~~~~~~~~~~~~~~~~
		
		//Same as Root?
		if(EachSMA->GetStaticMeshComponent()->StaticMesh != RootSM)
		{
			//Skip cause not same SM
			continue;
		}
		
		//Add!
		Transforms.Add(EachSMA->GetTransform());
		ToDestroy.Add(EachSMA);
		V_LOG2("Performed JoyISM ~ ", EachSMA->GetName()); 
	}
	
	//Create JoyISM
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoCollisionFail 		= true;
	SpawnInfo.bDeferConstruction 	= false;
	 
	AJoyISM* JoyISM = GetWorld()->SpawnActor<AJoyISM>(
		AJoyISM::StaticClass(), 
		RootSMA->GetActorLocation() , RootSMA->GetActorRotation(), SpawnInfo 
	);
	 
	//Mesh
	JoyISM->Mesh->SetStaticMesh(RootSM);
	  
	//Materials
	const int32 MatTotal = RootSMC->GetNumMaterials();
	for(int32 v = 0; v < MatTotal; v++)
	{
		JoyISM->Mesh->SetMaterial(v,RootSMC->GetMaterial(v));
	}
	 
	//for each Transform in world space
	for(FTransform& Each : Transforms)
	{
		JoyISM->Mesh->AddInstanceWorldSpace(Each);
	} 
	
	//Copy Layers
	JoyISM->Layers = RootSMA->Layers;
	
	//Set Folder
	JoyISM->SetFolderPath(RootSMA->GetFolderPath());
		 
	//Done! 
	JoyISM->Modify();
	  
	//Destroy original
	for(AActor* Each : ToDestroy)
	{
		GEditor->SelectActor( Each, /*InSelected=*/false, /*bNotify=*/true );
		
		Each->Modify();
		
		Each->Destroy();
	}
	
	
	
	GEditor->SelectActor( JoyISM, /*InSelected=*/true, /*bNotify=*/true );
	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FDropToClosestSurfaceData* FVictoryEdAlignMode::GetActorWithShortestDrop(TArray<FDropToClosestSurfaceData>& TheData)
{
	//No Actors
	if(TheData.Num() < 1) return NULL;
	
	//Only 1 Actor
	if(TheData.Num() < 2) return &TheData[0];
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	float SmallestDistance = 100000000;
	int32 SmallestDataIndex = 0;
	
	//Find Smallest
	for(int32 b = 0; b < TheData.Num(); b++)
	{
		if( TheData[b].DistSquared < SmallestDistance)
		{
			SmallestDistance = TheData[b].DistSquared;
			SmallestDataIndex = b;
		}
	}
	
	//Valid?
	if(TheData.IsValidIndex(SmallestDataIndex))
	{
		return &TheData[SmallestDataIndex];
	}
	
	return NULL;
}
void FVictoryEdAlignMode::DropSelectedActorsToNearestSurface()
{
	if(!VictoryEngine) return;
	//~~~~~~~~~~~~~
	

	//ONLY WORKING WITH SMAs at the moment, due to nature of rest of plugin
	AStaticMeshActor* AsSMA = NULL;
	
	//cause I dont know how to make the widge move after dropping one or group of objects
	
	//~~~~~~~~~~~~~~
	// Closest Surface Data
	//~~~~~~~~~~~~~~
	TArray<FDropToClosestSurfaceData> ClosestSurfaceData;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//TRACE PARAMETERS
	FCollisionQueryParams TraceParams(FName(TEXT("VictoryEd Trace")), true, NULL);
	TraceParams.bTraceComplex = true;
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	
	//Ignore  ALL Selected Actors
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		VSelectItrActor = Cast<AActor>(*VSelectItr);
		if(!VSelectItrActor) continue;
		//~~~~~~~~~~~~~~~~~~
		
		//Add
		TraceParams.AddIgnoredActor(VSelectItrActor);
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//~~~~~~~~~~~
	//Hit Result
	FHitResult TheHit;
	//~~~~~~~~~~~

	
	//Iterate Over Selected Actors
	FVector ActorLoc;
	FVector BottomPoint;
	FVector OffsetFromCenter;
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		AsSMA = Cast<AStaticMeshActor>(*VSelectItr);
		if(!AsSMA) continue;
		//~~~~~~~~~~~~~~~~~~
		
		//~~~~~~~~~~~~~~~
		//~~~ Do the Trace ~~~
		//~~~~~~~~~~~~~~~

		//Re-initialize hit info
		TheHit = FHitResult(ForceInit);
		
		//Trace!
		ActorLoc = AsSMA->GetActorLocation();
		GetWorld()->LineTraceSingleByChannel(
			TheHit,		//result
			ActorLoc,								
			ActorLoc + FVector(0,0,-20000),
			ECC_Pawn, //collision channel
			TraceParams
		);
		
		//Hit any Actor?
		if(!TheHit.GetActor()) continue;
		//~~~~~~~~~~~~~~~~~
		
		
		//See .h
		GetBottomSurfacePoint(AsSMA,BottomPoint);
		
		
		//Offset From Center
		OffsetFromCenter = ActorLoc - BottomPoint;
			
		//~~~ ADD Closest Surface Data ~~~
		FDropToClosestSurfaceData NewData;
		NewData.TheActor = AsSMA;
		NewData.SurfaceLoc = TheHit.ImpactPoint + OffsetFromCenter;
		NewData.DistSquared = FVector::DistSquared(AsSMA->GetActorLocation(),NewData.SurfaceLoc);
		ClosestSurfaceData.Add(NewData);
		
	}
	
	
	//Get Smallest Drop Data
	FDropToClosestSurfaceData* SmallestDropData = GetActorWithShortestDrop(ClosestSurfaceData);
	if(!SmallestDropData) return;
	//~~~~~~~~~~~~~~~
	
	//Get Smallest Drop Actor
	AStaticMeshActor* AnchorActor = SmallestDropData->TheActor;
	if(!AnchorActor) return;
	//~~~~~~~~~~~~~~~
	
	
	
	//Get Relative Offsets of this to all other actors in selection
	const FVector Origin = AnchorActor->GetActorLocation();
	TArray<FVector> AnchorOffsets;
	AActor* AsActor;
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		AsActor = Cast<AActor>(*VSelectItr);
		if(!AsActor) continue;
		//~~~~~~~~~~~~~~~~~~
		
		AnchorOffsets.Add(AsActor->GetActorLocation() - Origin);
	}
	
	
	//~~~ Move Anchor Actor ~~~
	
	//Move to impact, Plus the offset from nearest surface of object
	AnchorActor->SetActorLocation(SmallestDropData->SurfaceLoc);
	
	//Move all other actors to Anchor Actor + Offset
	int32 AnchorItr = 0;
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		AsActor = Cast<AActor>(*VSelectItr);
		if(!AsActor) continue;
		//~~~~~~~~~~~~~~~~~~
		
		//VALID?
		if(!AnchorOffsets.IsValidIndex(AnchorItr)) break;
		
		AsActor->SetActorLocation(SmallestDropData->SurfaceLoc + AnchorOffsets[AnchorItr]);
		
		//INC
		AnchorItr++;
	}
	
	
	//~~~~~~~~~~~~~~
	
	//Update Display!
	DoSingleDisplayUpdate = true;
	
	
	//~~~ RESELECT SINGLE ACTOR TO MOVE WIDGET
	if(VictoryEngine->GetSelectedActorCount() < 2)
	{
		//~~~~~~~~~~~~~~~~~
		//Deselect without Noting
		VictoryEngine->SelectNone(false, true, false);
		VictoryEngine->SelectActor(AnchorActor,true,true);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FPositionVertexBuffer* FVictoryEdAlignMode::GetVerticies(AStaticMeshActor* TheSMA)
{
	if(!TheSMA) return NULL;
	if(!TheSMA->IsValidLowLevel()) return NULL;
	if(!TheSMA->StaticMeshComponent) return NULL;
	if(!TheSMA->StaticMeshComponent->StaticMesh) return NULL;
	if(!TheSMA->StaticMeshComponent->StaticMesh->RenderData) return NULL;
	
	//Valid LOD ?
	if(TheSMA->StaticMeshComponent->StaticMesh->RenderData->LODResources.Num() <= 0) return NULL;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	//~~~~~~~~~~~~~~~~~~~~
	//				Vertex Buffer Check
	FPositionVertexBuffer* VertexBuffer = 
		&TheSMA->StaticMeshComponent->StaticMesh->RenderData->LODResources[0].PositionVertexBuffer;
	
	//~~~~~~~~~~~~~~~~~~~~
	//			Count Too High?
	if(VertexBuffer->GetNumVertices() > MAX_VERTEX_COUNT_FOR_DRAWING)
	{
		//UE_LOG(Victory, Error, TEXT("Vertex Count too high to draw! %d"), VertexBuffer->GetNumVertices() );
		return NULL;
		//~~~~~~~
	}
	
	//Return Vertex Buffer
	return &TheSMA->StaticMeshComponent->StaticMesh->RenderData->LODResources[0].PositionVertexBuffer;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
UENUM()
enum EInputEvent
{
	IE_Pressed              =0,
	IE_Released             =1,
	IE_Repeat               =2,
	IE_DoubleClick          =3,
	IE_Axis                 =4,
	IE_MAX                  =5,
};
*/

//Pressed
void FVictoryEdAlignMode::InputKeyPressed(FKey Key)
{
	//UE_LOG(Victory, Error, TEXT("Key pressed %s"), *EKeys::GetKeyName(Key).ToString() );
	
	if(!VictoryEngine) return;
	//~~~~~~~~~~~~
	
	//RMB
	if(Key == EKeys::RightMouseButton) 
	{
		RMBDown = true;
		return;
		//~~~~~~
	}
	
	//Left Mouse
	if(Key == EKeys::LeftMouseButton)
	{
		LeftMouseClick();
		return;
		//~~~~~~
	}
	
	//X Key
	if(Key == EKeys::X) 
	{
		XDown = true;
		return;
	}
	
	//Y Key
	if(Key == EKeys::Y) 
	{
		YDown = true;
		DropSelectedActorsToNearestSurface();
		return;
	}
	
	//I Key
	if(Key == EKeys::I) 
	{
		if(ShiftDown)
		{
			ReverseJoyISM();
		}
		else
		{	
			PerformJoyISM();
		}
		return;
	}
	
	//U Key
	if(Key == EKeys::U) 
	{ 
		VictoryEngine->DrawVerticiesMode+=2; //skipping the 2 state, cause it not working at moment
		if(VictoryEngine->DrawVerticiesMode > 2) VictoryEngine->DrawVerticiesMode = 0;
		return;
	}
	
	//K Key
	if(Key == EKeys::K) 
	{
		//No modifier keys
		if(	Key == EKeys::LeftShift || Key == EKeys::RightShift ||
			Key == EKeys::LeftControl || Key == EKeys::RightControl ||
			Key == EKeys::LeftAlt || Key == EKeys::RightAlt
		) return;
		
		if(VictoryEngine->VSelectedActor)
			VictoryEngine->VSelectedActor->SetActorRotation(FRotator::ZeroRotator);
		return;
	}
	
	//SHIFT KEY
	if(Key == EKeys::LeftShift || Key == EKeys::RightShift) 
	{
		ShiftDown = true;
		return;
	}
	
	
	//- Key
	if(Key == EKeys::Hyphen) //weirdly, underscore not do anything
	{ 									
		MinusIsDown = true;
		PendingButtonRefresh = true;
		return;
	}
	//+ Key 
	if(Key == EKeys::Equals) 
	{ 
		
		PlusIsDown = true;
		PendingButtonRefresh = true;
		return;
	}
	
	//T Key
	if(Key == EKeys::T)
	{
		UsingMouseInstantMove = !UsingMouseInstantMove;
		return;
		//~~~~~~~~~
	}
	
	//V Key
	if(Key == EKeys::V)
	{
		//CTRL KEY
		if(Key == EKeys::LeftControl || Key == EKeys::RightControl) return;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 
		SnapKeyPressed = true;
		return;
		//~~~~~~~~~
	}
	//B Key
	if(Key == EKeys::B)
	{
		VictoryEngine->VertexDisplayChoice++;
		if(VictoryEngine->VertexDisplayChoice > 4) VictoryEngine->VertexDisplayChoice = 0;
		return;
		//~~~~~~~~~
	}
	
	
	//Restore Previous Editor Mode
	if(Key == EKeys::P)
	{
		//Clear all Verticies
	
		//Exit
		VictoryEngine->ExitVictoryAlignMode();
		
		return;
		//~~~~~~
	}
}




//Released
void FVictoryEdAlignMode::InputKeyReleased(FKey Key)
{
	//Vertex Key
	if(Key == EKeys::V)
	{
		SnapKeyPressed = false;
	}
	//RMB
	else if(Key == EKeys::RightMouseButton)
	{
		PendingButtonRefresh = true;
		RMBDown = false;
	}
	
	//LMB
	else if(Key == EKeys::LeftMouseButton)
	{
		PendingButtonRefresh = true;
	}
	  
	//SHIFT KEY
	if(Key == EKeys::LeftShift || Key == EKeys::RightShift) ShiftDown = false;
	 
	//- Key
	if(Key == EKeys::Hyphen) MinusIsDown = false;
	
	//+ Key
	if(Key == EKeys::Equals)  PlusIsDown = false;
	
	//X Key
	if(Key == EKeys::X) XDown = false;
	
	//Y Key
	if(Key == EKeys::Y) YDown = false;
	
}

//Double Click
void FVictoryEdAlignMode::InputKeyDoubleClick(FKey Key)
{
	//UE_LOG(Victory, Error, TEXT("Key DOUBLE CLICK %s"), *EKeys::GetKeyName(Key).ToString() );
}

bool FVictoryEdAlignMode::InputKey(FEditorViewportClient* ViewportClient,FViewport* Viewport,FKey Key,EInputEvent Event)
{
	//Middle Mouse
	if (Key == EKeys::MiddleMouseButton) 			MiddleMouseEvent(Event);
	else if(Event == EInputEvent::IE_Pressed) 		InputKeyPressed(Key);
	else if(Event == EInputEvent::IE_Released) 	InputKeyReleased(Key);
	else if(Event == EInputEvent::IE_DoubleClick) 	InputKeyDoubleClick(Key);
	
	return false;
}

void FVictoryEdAlignMode::ProcessMouseInstantMove(FEditorViewportClient* ViewportClient)
{
	CHECK_VSELECTED
	if(!ViewportClient) return;
	//~~~~~~~~~~~~~~~
	
	//Re-initialize hit info
	RV_Hit = FHitResult(ForceInit);
	
	//Clear Previous Ignore Actors
	RV_TraceParams.IgnoreComponents.Empty();
	
	//Ignore All Selected Actors!!!
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		VSelectItrActor = Cast<AActor>(*VSelectItr);
		if(!VSelectItrActor) continue;
		//~~~~~~~~~~~~~~~~~~
		
		//Add
		RV_TraceParams.AddIgnoredActor(VSelectItrActor);
	}
	
	//Trace
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		//result
		CursorWorldPos,	//start
		CursorWorldPos + 100000 * CursorWorldDir , //end
		ECC_WorldStatic, //collision channel
		RV_TraceParams
	);
	
	//Store Hit Actor
	RV_HitActor = RV_Hit.GetActor();
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				Update Highlighted Actor
	
		//does  not equal same		  does not equal the main Selected Actor
	if(RV_HitActor != HighlightedActor && RV_HitActor != VictoryEngine->VSelectedActor )
	{
		//potentially deselect any highlighted
		HighlightedActor = Cast<AStaticMeshActor>(RV_HitActor);
		
		//Refresh Vertex Buffer
		HighlightedVertexBuffer = NULL;
		if(HighlightedActor) HighlightedVertexBuffer = GetVerticies(HighlightedActor);
		
		
	}
	
	if(RV_Hit.bBlockingHit)
	{
		//INVALIDATE THE DISPLAY
		ViewportClient->Viewport->InvalidateDisplay();
	}
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			PROCESS MOUSE INSTANT MOVE?
	if(!UsingMouseInstantMove) return;
	//~~~~~~~~~~~~~~~~~~~~~~~~
	
	//If not hit something, use default distance
	if(!RV_Hit.bBlockingHit) RV_Hit.ImpactPoint = CursorWorldPos + DEFAULT_INSTANT_MOVE_DISTANCE * CursorWorldDir;
	
	
	//		Move the Primary, the Origin!
	RV_Vect = RV_Hit.ImpactPoint;
		
		//Use Existing Z of Object
	RV_Vect.Z = VictoryEngine->VSelectedActor->GetActorLocation().Z;
		
		//Update Location
	VictoryEngine->VSelectedActor->SetActorLocation(RV_Vect);	
		
		//Save New Origin Location
	const FVector NewOrigin(RV_Vect);
	
	//~~~~~~~
	
	//Update the Others According to the Moved Origin
	RV_Int32 = 0;
	for(FSelectionIterator VSelectItr = VictoryEngine->GetSelectedActorIterator(); 
		VSelectItr; ++VSelectItr )
	{
		VSelectItrActor = Cast<AActor>(*VSelectItr);
		if(!VSelectItrActor) 
		{
			RV_Int32++;
			continue;
		}
		//~~~~~~~~~~~~~~~~~~
		
		//Get New Origin
		RV_Vect = NewOrigin;
		
		//Add Offset
		if(VictoryEngine->SelectionRelativeOffsets.IsValidIndex(RV_Int32))
		{
			RV_Vect+=VictoryEngine->SelectionRelativeOffsets[RV_Int32];
		}
		
		//Update Location
		VSelectItrActor->SetActorLocation(RV_Vect);
		
		//Inc
		RV_Int32++;
	}
	
	//INVALIDATE THE DISPLAY
	ViewportClient->Viewport->InvalidateDisplay();
	
}

bool FVictoryEdAlignMode::InputDelta( FEditorViewportClient* InViewportClient,FViewport* InViewport,FVector& InDrag,FRotator& InRot,FVector& InScale )
{
	//only works if LMB is down
	
	return false;
}

bool FVictoryEdAlignMode::InputAxis(FEditorViewportClient* InViewportClient,FViewport* Viewport,int32 ControllerId,FKey Key,float Delta,float DeltaTime)
{
	//only works if any mouse button is down
	
	return false;
}

bool FVictoryEdAlignMode::MouseMove(FEditorViewportClient* ViewportClient,FViewport* Viewport,int32 x, int32 y)
{
	if(!ViewportClient) return false;
	
	//~~~~~~~~~~~~~~~
	//	ESSENTIAL UPDATES
	MouseLocation.X=x;
	MouseLocation.Y=y;
	
	
	
	return false;
}

//Middle Mouse Event
void FVictoryEdAlignMode::MiddleMouseEvent(EInputEvent Event)
{
	//Pressed
	if(Event == EInputEvent::IE_Pressed)
	{
		
	
		return;
	}
	
	//Released
	if(Event == EInputEvent::IE_Released)
	{
		
		return;
	}
}

//Left Mouse Click
void FVictoryEdAlignMode::LeftMouseClick()
{
	CHECK_VSELECTED
	
	//SelectedVertexForSelectedActor = -1;
}
bool FVictoryEdAlignMode::HandleClick(FEditorViewportClient* InViewportClient, HHitProxy *HitProxy, const FViewportClick &Click)
{

	return false;
}

//~~~

//``
void FVictoryEdAlignMode::GetSelectedVertexLocation(FVector& LocOut)
{
	CHECK_VSELECTED
	
	if(!SelectedVertexBuffer)  return;	
	//~~~~~~~~~~~~~~~~~~
	
	const FTransform SMATransform = VictoryEngine->VSelectedActor->GetTransform();
	const FVector SMALocation 		= SMATransform.GetLocation();
	const int32 VertexCount 			= SelectedVertexBuffer->GetNumVertices();
	for(int32 Itr = 0; Itr < VertexCount; Itr++)
	{
		if(Itr == SelectedVertexForSelectedActor) 
		{
			LocOut = SMALocation + SMATransform.TransformVector(SelectedVertexBuffer->VertexPosition(Itr));
			return;
			//~~~~
		}
	}
}

void FVictoryEdAlignMode::DoVertexSnap(const FVector& Dest)
{
	CHECK_VSELECTED
	  
	//Somehow undo redo is capturing the actor move event on its own and recording it in a wonky way
	//const FScopedTransaction Transaction( NSLOCTEXT("Snap","VictoryGame", "Victory Vertex Snap" )  );
		
	FVector SelectedVertexLocation;
	
	//Initial Location
	GetSelectedVertexLocation(SelectedVertexLocation);
	
	//Match rotation of owner of target vertex?
	if(ShiftDown)
	{
		VictoryEngine->VSelectedActor->SetActorRotation(RV_Hit.ImpactNormal.Rotation());
		//Recalc Selected Vertex After Rotation
		GetSelectedVertexLocation(SelectedVertexLocation);
		
		//UE_LOG(Victory, Error, TEXT("WHAT IS HIT NORMAL %s"), *RV_Hit.ImpactNormal.Rotation().ToString() );
	}
	
	//No modifier, just regular click
	
	//1. calc distance between selected vertex and JSMA location
	//2. move the JSMA to the target vertex
	//3. subtract the distance
	
	//Distance Offset
	const FVector LocOffset = SelectedVertexLocation - VictoryEngine->VSelectedActor->GetActorLocation();
	
	//Set Location
	VictoryEngine->VSelectedActor->SetActorLocation(Dest - LocOffset);
	
	//Refresh After Moving
	PendingButtonRefresh = true;
	
	//Done! 
	//VictoryEngine->VSelectedActor->Modify();
}

void FVictoryEdAlignMode::RefreshVertexButtons(const FSceneView* View)
{
	CHECK_VSELECTED
	
	if(!View) return;
	//~~~~~~~~~~~~~
	
	//No Longer Pending
	PendingButtonRefresh = false;
	
	//Refresh
	SelectedActorButtons.Empty();
	HighlightedActorButtons.Empty();
	
	//~~~ Vars ~~~
	const float ButtonHalfSize = VictoryEngine->CurrentVerticiesScale/2;
	
	//~~~~~~~~~~~~~~~~~~~~~
	//			Selected Actor
	//~~~~~~~~~~~~~~~~~~~~~
	if(!VictoryEngine->VSelectedActor->IsValidLowLevel() ) return;
	
	if(!SelectedVertexBuffer)  return;	
	//~~~~~~~~~~~~~~~~~~
	
	FTransform SMATransform = VictoryEngine->VSelectedActor->GetTransform();
	FVector SMALocation 		= SMATransform.GetLocation();
	const int32 VertexCount 			= SelectedVertexBuffer->GetNumVertices();
	for(int32 Itr = 0; Itr < VertexCount; Itr++)
	{
		//Get Rotated Scaled Translated Vertex Pos
		VertexWorldSpace = SMALocation + SMATransform.TransformVector(SelectedVertexBuffer->VertexPosition(Itr));
		
		//Get 2D Center
		VictoryProject(View, 
			VertexWorldSpace, 
			Vertex2DCenter 
		);
		
		FVButton NewButton;
		NewButton.Vibe 			= 	Itr;
		NewButton.PointInWorld 	= 	VertexWorldSpace;
		NewButton.minX			=	Vertex2DCenter.X - ButtonHalfSize;
		NewButton.maxX			=	Vertex2DCenter.X + ButtonHalfSize;
		NewButton.minY			=	Vertex2DCenter.Y - ButtonHalfSize;
		NewButton.maxY			=	Vertex2DCenter.Y + ButtonHalfSize;

		//Add Button
		SelectedActorButtons.Add(NewButton);
	}
	
	//~~~~~~~~~~~~~~~~~~~~~
	//			Highlighted Actor
	//~~~~~~~~~~~~~~~~~~~~~
	if(!HighlightedActor) return;
	if(!HighlightedActor->IsValidLowLevel()) return;
	
	if(!HighlightedVertexBuffer)  return;	
	//~~~~~~~~~~~~~~~~~~
	
	SMATransform = HighlightedActor->GetTransform();
	SMALocation 		= SMATransform.GetLocation();
	const int32 VertexCount2 			= HighlightedVertexBuffer->GetNumVertices();
	for(int32 Itr = 0; Itr < VertexCount2; Itr++)
	{
		//Get Rotated Scaled Translated Vertex Pos
		VertexWorldSpace = SMALocation + SMATransform.TransformVector(HighlightedVertexBuffer->VertexPosition(Itr));
		
		//Get 2D Center
		VictoryProject(View, 
			VertexWorldSpace, 
			Vertex2DCenter 
		);
		
		FVButton NewButton;
		NewButton.Vibe 			= 	Itr;
		NewButton.PointInWorld 	= 	VertexWorldSpace;
		NewButton.minX			=	Vertex2DCenter.X - ButtonHalfSize;
		NewButton.maxX			=	Vertex2DCenter.X + ButtonHalfSize;
		NewButton.minY			=	Vertex2DCenter.Y - ButtonHalfSize;
		NewButton.maxY			=	Vertex2DCenter.Y + ButtonHalfSize;

		//Add Button
		HighlightedActorButtons.Add(NewButton);
	}
	
}

//```
void FVictoryEdAlignMode::PDI_DrawVerticies(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FPositionVertexBuffer* VertexBuffer, const FTransform& SMATransform, bool DrawingSelectedActor)
{
	CHECK_VSELECTED
	 
	if(!View) 			return;
	if(!PDI) 			return;
	if(!VertexBuffer) 	return;
	 
	if(!VictoryEngine->VSelectedActor->IsA(AStaticMeshActor::StaticClass())) return;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Location
	const FVector SMALocation 			= SMATransform.GetLocation();
	 
	//Get Count
	//		how is it crashing here?
	const int32 VertexCount = VertexBuffer->GetNumVertices();
	
	//~~~~~~~~~~~~~~~~~~~~~
	//			Selected Actor
	//~~~~~~~~~~~~~~~~~~~~~
	if(DrawingSelectedActor)
	{
	for(int32 Itr = 0; Itr < VertexCount; Itr++)
	{
		//Selected Vertex?
		if(SelectedVertexForSelectedActor == Itr )
		{
			//Draw to the PDI
			PDI->DrawPoint(
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				RV_Yellow,
				VictoryEngine->CurrentVerticiesScale*VERTEX_SELECTED_MULT,
				0 //depth
			);
			continue;
		}
		else if(HighlightedVertexForSelectedActor == Itr)
		{
			//Draw to the PDI
			PDI->DrawPoint(
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				FLinearColor(0,1,1,1),
				VictoryEngine->CurrentVerticiesScale*VERTEX_SELECTED_MULT,
				0 //depth
			);
			continue;
		}
		else
		{
			if(VictoryEngine->DrawVerticiesMode < 2) continue;
			//~~~~~~~~~~~~~~~~~~~
			
			//Spheres
			if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_SPHERE)
			{
				if(VertexCount > MAX_VERTEX_COUNT_FOR_DRAWING_SPHERES)
				{
				DrawWireBox(
					PDI,
					BoxFromPointWithSize(SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),VictoryEngine->CurrentVerticiesScale*0.5),
					RV_VRed,
					0
				);
				continue;
				}
				else
				{
				DrawWireSphere(
					PDI, 
					SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
					RV_VRed, 
					VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT, 
					12, 
					0
				);
				continue;
				}
			}
			
			//Diamond
			else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_DIAMOND3D)
			{
			DrawWireSphere(
				PDI, 
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				RV_VRed, 
				VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT, 
				4, 
				0
			);
			continue;
			}
			
			//Box
			else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_3DBOX)
			{
			DrawWireBox(
				PDI,
				BoxFromPointWithSize(SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT),
				RV_VRed,
				0
			);
			continue;
			}
			
			//Stars
			else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_STARS)
			{
			DrawWireStar(
				PDI,
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				VictoryEngine->CurrentVerticiesScale, 
				RV_VRed,
				0
			);
			continue;
			}
			
			//Rect
			else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_RECT)
			{
			//Draw to the PDI
			PDI->DrawPoint(
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				RV_Red,
				VictoryEngine->CurrentVerticiesScale,
				0 //depth
			);
			continue;
			}
			
		}
			
	}
	}
	
	//~~~~~~~~~~~~~~~~~~~~~
	//			Highlighted Actor
	//~~~~~~~~~~~~~~~~~~~~~
	else
	{
	for(int32 Itr = 0; Itr < VertexCount; Itr++)
	{
		if(HighlightedVertexForHighlightedActor == Itr)
		{
			//Draw to the PDI
			PDI->DrawPoint(
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				FLinearColor(0,1,1,1),
				VictoryEngine->CurrentVerticiesScale*VERTEX_SELECTED_MULT,
				0 //depth
			);
			continue;
		}
		
		//~~~~~~~~~~~~~~~~~~~
		if(VictoryEngine->DrawVerticiesMode < 2) continue;
		//~~~~~~~~~~~~~~~~~~~
		
		
		//Spheres
		if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_SPHERE)
		{
			if(VertexCount > MAX_VERTEX_COUNT_FOR_DRAWING_SPHERES)
			{
			DrawWireBox(
				PDI,
				BoxFromPointWithSize(SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT),
				RV_VBlue,
				0
			);
			continue;
			}
			else
			{
			DrawWireSphere(
				PDI, 
				SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
				RV_VBlue, 
				VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT, 
				12, 
				0
			);
			continue;
			}
		}
		
		//Diamond
		else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_DIAMOND3D)
		{
		DrawWireSphere(
			PDI, 
			SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
			RV_VBlue, 
			VictoryEngine->CurrentVerticiesScale*VERTEX_SHAPE_MULT, 
			4, 
			0
		);
		continue;
		}
			
		//Box
		else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_3DBOX)
		{
		DrawWireBox(
			PDI,
			BoxFromPointWithSize(SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),VictoryEngine->CurrentVerticiesScale),
			RV_VBlue,
			0
		);
		continue;
		}
		
		//Stars
		else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_STARS)
		{
		
		DrawWireStar(
			PDI,
			SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
			VictoryEngine->CurrentVerticiesScale, 
			RV_VBlue,
			0
		);
		continue;
		}
		
		//Rect
		else if(VictoryEngine->VertexDisplayChoice == VERTEX_DISPLAY_RECT)
		{
		//Draw to the PDI
		PDI->DrawPoint(
			SMALocation + SMATransform.TransformVector(VertexBuffer->VertexPosition(Itr)),
			RV_Blue,
			VictoryEngine->CurrentVerticiesScale,
			0 //depth
		);
		continue;
		}
	}	
	}
}


void FVictoryEdAlignMode::DrawHotkeyToolTip(FCanvas* Canvas)
{
	if(!Canvas) return;
	//~~~~~~~~~
	
	RV_yStart = MouseLocation.Y + 16;
	
	//Background
	DrawVictoryRect(Canvas, 
		MouseLocation.X - 7, 
		RV_yStart - 7,
		420,
		VICTORY_TEXT_HEIGHT * 11 + 120,
		FLinearColor(0,0,1,0.777)
	);
	
	DrawVictoryTextWithColor(Canvas, 
		"~~~ Victory Editor Mode Hotkeys ~~~", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	  
	RV_yStart += VICTORY_TEXT_HEIGHT * 2;
	DrawVictoryTextWithColor(Canvas, 
		"i/Shift + i ~ Convert/Revert JoyISM!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	); 
	RV_yStart += VICTORY_TEXT_HEIGHT + 3;
	DrawVictoryTextWithColor(Canvas, 
		"Create or revert Instanced Static Mesh!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 20;
	DrawVictoryTextWithColor(Canvas, 
		"Y ~ Drop Objects to Nearest Surface!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 20;
	DrawVictoryTextWithColor(Canvas, 
		"T ~ Toggle Instant Mouse Move Mode", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT;
	DrawVictoryTextWithColor(Canvas, 
		"K ~ Reset Rotation", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 20;
	DrawVictoryTextWithColor(Canvas, 
		"V ~ Select Verticies for Snapping!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 5;
	DrawVictoryTextWithColor(Canvas, 
		"SHIFT + V ~ Snap by Surface Normal", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 5;
	DrawVictoryTextWithColor(Canvas, 
		"B ~ Toggle Different Vertex Types!!!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	 
	RV_yStart += VICTORY_TEXT_HEIGHT + 4;
	DrawVictoryTextWithColor(Canvas, 
		"Hold + or - ~ Change Vertex Size!", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
	
	RV_yStart += VICTORY_TEXT_HEIGHT + 4;
	DrawVictoryTextWithColor(Canvas, 
		"U ~ Toggle Hiding of All Verticies", 
		MouseLocation.X,RV_yStart,
		RV_Yellow
	);
}

void FVictoryEdAlignMode::DrawVictoryText(FCanvas* Canvas, const FString& TheStr, float X, float Y, float TheScale )
{
	if(!Canvas) return;
	//~~~~~~~~~
	
	FCanvasTextItem NewText(
		FVector2D(X,Y),
		FText::FromString(TheStr),
		VictoryEngine->GetVictoryFont(),
		FLinearColor(1,0,1,1)
	);
	NewText.Scale.Set(TheScale,TheScale);
	NewText.bOutlined = true;
	NewText.Draw(Canvas);
}
void FVictoryEdAlignMode::DrawVictoryTextWithColor(FCanvas* Canvas, const FString& TheStr, float X, float Y, FLinearColor TheColor, float TheScale )
{
	if(!Canvas) return;
	//~~~~~~~~~
	
	FCanvasTextItem NewText(
		FVector2D(X,Y),
		FText::FromString(TheStr),
		VictoryEngine->GetVictoryFont(),
		TheColor
	);
	NewText.Scale.Set(TheScale,TheScale);
	NewText.bOutlined = true;
	
	//Outline gets its alpha from the main color
	NewText.OutlineColor = FLinearColor(0,0,0,TheColor.A * 2);
	
	NewText.Draw(Canvas);
}

void FVictoryEdAlignMode::DrawVictoryLine(
	FCanvas* Canvas, 
	const FVector2D& Start, 
	const FVector2D& End, 
	FLinearColor TheColor, 
	float Thick
)
{
	if(!Canvas) return;
	//~~~~~~~~~
	
	FCanvasLineItem NewLine(Start,End);
	NewLine.SetColor(TheColor);
	NewLine.LineThickness = Thick;
	NewLine.Draw(Canvas);
}
void FVictoryEdAlignMode::DrawVictoryRect(FCanvas* Canvas, float X, float Y, float Width, float Height, FLinearColor Color)
{
	if(!Canvas) return;
	//~~~~~~~~~
	
	FCanvasTileItem TileItem( 
		FVector2D(X, Y), 
		FVector2D( Width, Height ), 
		Color 
	);
   
    TileItem.BlendMode = SE_BLEND_Translucent;
	TileItem.Draw(Canvas);
	
	/*
	Canvas->DrawTile( 
		X, Y, 
		Width, Height, 
		0, 0, 1, 1, 
		Color
	);
	*/

}

int32 FVictoryEdAlignMode::FindClosestOfButtons(TArray<FVButton*>& Inbuttons)
{
	float CurDistance;
	float MinDistance = 10000000000;
	int32 MinIndex = -1;
	for(int32 Itr=0; Itr < Inbuttons.Num(); Itr++)
	{
		CurCheckButton = Inbuttons[Itr];
		if(!CurCheckButton) continue;
		
		//world space location stored in the Button itself
		CurDistance = FVector::DistSquared(CurCheckButton->PointInWorld,CursorWorldPos);
		
		//Min Check
		if(CurDistance < MinDistance)
		{
			MinDistance = CurDistance;
			MinIndex = Itr;
		}
	}
	
	//VALIDITY CHECK
	if(!Inbuttons.IsValidIndex(MinIndex))
		return -1;
		
	else return MinIndex;
}

void FVictoryEdAlignMode::CheckCursorInButtons(FCanvas* Canvas)
{	
	if(!Canvas) return;
	//~~~~~~~~~

	//Reset Highlighted
	HighlightedVertexForSelectedActor = -1;
	HighlightedVertexForHighlightedActor = -1;
	
	//~~~~~~~~~~~~~~~~~~~
	//Main Buttons
	//~~~~~~~~~~~~~~~~~~~
	RV_Int32=0;
	while(VictoryButtons.IsValidIndex(RV_Int32))
	{
		CurCheckButton = &VictoryButtons[RV_Int32];
		
		//check cursor in bounds
		if (CurCheckButton->minX <= MouseLocation.X && MouseLocation.X <= CurCheckButton->maxX &&
			CurCheckButton->minY <= MouseLocation.Y && MouseLocation.Y <= CurCheckButton->maxY )
		{
			//Active Button Type
			ActiveButton_Vibe = CurCheckButton->Vibe; 
			
			if(ActiveButton_Vibe == BUTTON_VIBE_VICTORYHOTKEYS)
			{
				DrawHotkeyToolTip(Canvas);
				FadeInVictoryTitle = true;
				VictoryTitleVisible = false; //prevent a duel between the two
				return;
				//~~~~~
			}
		}
		
		//~~~~~~~
		RV_Int32++;
	}
	
	//~~~~~~~~~~~~~~~~~~~
	//Selected Actor Vertex Buttons
	//~~~~~~~~~~~~~~~~~~~
	RV_Int32=0;
	ClosestSelectedActorButtons.Empty();
	while(SelectedActorButtons.IsValidIndex(RV_Int32))
	{
		CurCheckButton = &SelectedActorButtons[RV_Int32];
		//check cursor in bounds
		if (CurCheckButton->minX <= MouseLocation.X && MouseLocation.X <= CurCheckButton->maxX &&
			CurCheckButton->minY <= MouseLocation.Y && MouseLocation.Y <= CurCheckButton->maxY )
		{
			ClosestSelectedActorButtons.Add(&SelectedActorButtons[RV_Int32]);
		}
		
		//~~~~~~~
		RV_Int32++;
	}
	
	//Find closest of potentially highlighted/selected
	const int32 FoundIndex = FindClosestOfButtons(ClosestSelectedActorButtons);
	if(FoundIndex != -1)
	{
		CurCheckButton = ClosestSelectedActorButtons[FoundIndex];
		if(CurCheckButton)
		{
			//Active Button Type
			ActiveButton_Vibe = CurCheckButton->Vibe; 
			
			//Highlight Vertex if hovered over!
			HighlightedVertexForSelectedActor = ActiveButton_Vibe;
			
			//~~~~~~~~~~~~~~~~~~~~~~
			//			VERTEX SELECTION
			//~~~~~~~~~~~~~~~~~~~~~~
			//Vertex Selected and not same?
			if(SnapKeyPressed && SelectedVertexForSelectedActor != ActiveButton_Vibe )
			{
				SelectedVertexForSelectedActor = ActiveButton_Vibe;
				return;
				//~~~~~
			}
		}
	}
		
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
	//~~~~~~~~~~~~~~~~~~~~~~
	//Highlighted Actor Vertex Buttons
	//~~~~~~~~~~~~~~~~~~~~~~
	
	RV_Int32=0;
	ClosestHighlightedActorButtons.Empty();
	while(HighlightedActorButtons.IsValidIndex(RV_Int32))
	{
		CurCheckButton = &HighlightedActorButtons[RV_Int32];
		//check cursor in bounds
		if (CurCheckButton->minX <= MouseLocation.X && MouseLocation.X <= CurCheckButton->maxX &&
			CurCheckButton->minY <= MouseLocation.Y && MouseLocation.Y <= CurCheckButton->maxY )
		{
			ClosestHighlightedActorButtons.Add(&HighlightedActorButtons[RV_Int32]);
		}
		
		//~~~~~~~
		RV_Int32++;
	}
	
	//Find closest of potentially highlighted
	
	const int32 FoundIndex2 = FindClosestOfButtons(ClosestHighlightedActorButtons);
	if(FoundIndex2 != -1)
	{
		CurCheckButton = ClosestHighlightedActorButtons[FoundIndex2];
		if(CurCheckButton)
		{
			//Active Button Type
			ActiveButton_Vibe = CurCheckButton->Vibe; 
			
			//Highlight Vertex if hovered over!
			HighlightedVertexForHighlightedActor = ActiveButton_Vibe;
			
			//Vertex Selected?
			if(SnapKeyPressed && SelectedVertexForSelectedActor != -1)
			{
				DoVertexSnap(CurCheckButton->PointInWorld);
			}
		}
	}
}

//Victory Project
void FVictoryEdAlignMode::VictoryProject(const FSceneView* View,const FVector& WorldPoint,FVector2D& OutPixelLocation)
{
	if(!View) return;
	//~~~~~~~~~
	
	/** Transforms a point from the view's world-space into pixel coordinates relative to the view's X,Y (left, top). */
	View->WorldToPixel(WorldPoint,OutPixelLocation);
}

//Victory DeProject
void FVictoryEdAlignMode::VictoryDeProject(const FSceneView* View,const FVector2D& ScreenPoint, FVector& out_WorldOrigin, FVector& out_WorldDirection)
{
	if(!View) return;
	//~~~~~~~~~
	
	//NEEDS TO BE MADE CONST TO BE ABLE TO USE THIS
	/** transforms 2D screen coordinates into a 3D world-space origin and direction 
	 * @param ScreenPos - screen coordinates in pixels
	 * @param out_WorldOrigin (out) - world-space origin vector
	 * @param out_WorldDirection (out) - world-space direction vector
	 */
	//View->DeprojectFVector2D(ScreenPoint, out_WorldOrigin, out_WorldDirection);

	
	/** transforms 2D screen coordinates into a 3D world-space origin and direction 
	 * @param ScreenPos - screen coordinates in pixels
	 * @param ViewRect - view rectangle
	 * @param InvViewMatrix - inverse view matrix
	 * @param InvProjMatrix - inverse projection matrix
	 * @param out_WorldOrigin (out) - world-space origin vector
	 * @param out_WorldDirection (out) - world-space direction vector
	 */
	FSceneView::DeprojectScreenToWorld(
		ScreenPoint, 
		View->ViewRect, 
		View->InvViewMatrix, 
		View->ViewMatrices.GetInvProjMatrix(), 
		out_WorldOrigin, 
		out_WorldDirection
	);

}
	

//Draw Using the More Fundamental Method, PDI
void FVictoryEdAlignMode::Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI)
{
	CHECK_VSELECTED
	
	//~~~ Verticies ~~~
	if(VictoryEngine->DrawVerticiesMode > 0 )
	{
	
	if(SelectedVertexBuffer && VictoryEngine->VSelectedActor)
	{
		PDI_DrawVerticies(View,	PDI,
			SelectedVertexBuffer,VictoryEngine->VSelectedActor->GetTransform(),
			true
		);
	}
	
	//Dont draw if Highlighted == Selected, drawn already
	if(HighlightedVertexBuffer && HighlightedActor && HighlightedActor != VictoryEngine->VSelectedActor) 
	{
		PDI_DrawVerticies( View,	PDI,
			HighlightedVertexBuffer, HighlightedActor->GetTransform(),
			false //Highlighted Actor
		);
	}
	
	} //end of do draw verticies
	//~~~~~~~~~~~~~~~~~~~
}

//Draw HUD
void FVictoryEdAlignMode::DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas)
{
	if(!Canvas) return;
	if(!VictoryEngine) return;
	//~~~~~~~~~~~
	
	//Dont show title if drawing of verticies is disabled
	if(VictoryEngine->DrawVerticiesMode == 2)
	{ 
		//Title Button
		DrawVictoryTextWithColor(Canvas, "Victory Editor Hotkeys", 10,VICTORY_TITLE_HEIGHT, FLinearColor(1,0,1,VictoryTitleAlpha));
			
		//~~~ Cursor ~~~
		if(!UsingMouseInstantMove) CheckCursorInButtons(Canvas);
		
		//~~~ Make Buttons? ~~~
		if(PendingButtonRefresh) RefreshVertexButtons(View);
		
		//SUPER HYPER BUTTON REFRESHING
		//		due to the slide physx camera thing when moving
		if(!UsingMouseInstantMove) PendingButtonRefresh = true;
	} 
	
	//~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~
	
	//~~~ Cursor World Space ~~~
	VictoryDeProject(View, MouseLocation,CursorWorldPos,CursorWorldDir);
	
	//Calc Delta
	RV_Vect = CursorWorldPos + CursorWorldDir * CURSOR_DELTA_DISTANCE_CALC;
	CursorWorldDelta = RV_Vect - CursorWorldPrevPos;
	CursorWorldDelta /= 4;
	if(CursorWorldDelta.SizeSquared() < 3) CursorWorldDelta = FVector::ZeroVector;
	if(CursorWorldDelta.SizeSquared() > 10000) CursorWorldDelta = FVector::ZeroVector;
	
	//Save Prev
	CursorWorldPrevPos = RV_Vect;
}

void FVictoryEdAlignMode::Tick_VictoryTitle(FEditorViewportClient* ViewportClient)
{
	CurTimeSpan = FDateTime::Now() - VictoryTitleAppearTime;
	
	//If > then Allowed Visible Duration, start to fade out
	if(CurTimeSpan.GetTotalSeconds() > VICTORY_TITLE_VISIBLE_DURATION)
	{
		if(VictoryTitleAlpha <= 0)
		{
			VictoryTitleVisible = false;
			VictoryTitleAlpha = 0;
			return;
		}
		
		VictoryTitleAlpha -= 0.01;
	}
	
	//INVALIDATE THE DISPLAY
	ViewportClient->Viewport->InvalidateDisplay();
}
void FVictoryEdAlignMode::Tick_VictoryTitleFadeIn()
{
	if(VictoryTitleAlpha >=1)
	{
		VictoryTitleAppears();
		return;
	}
	
	VictoryTitleAlpha += 0.01;
}

//TICK
void FVictoryEdAlignMode::Tick_VictoryInterface(FEditorViewportClient* ViewportClient)
{
	if(!ViewportClient) return;
	//~~~~~~~~~~~
	
	//Re-activate Real-time each time enter Mode
	/*
	if(ReEntering)
	{
		ReEntering = false;
		ViewportClient->SetRealtime(true,true);
	}
	*/
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//							HUD
	ViewportSize = ViewportClient->Viewport->GetSizeXY();
	
	//Victory Title Tick
	if(VictoryTitleVisible) Tick_VictoryTitle(ViewportClient);
	
	//Fading in?
	if(FadeInVictoryTitle) 
	{
		Tick_VictoryTitleFadeIn();
		ViewportClient->Viewport->InvalidateDisplay();
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~						
	//			Move cursor if RMB is down
	//if(RMBDown) ViewportClient->Viewport->SetMouse(ViewportSize.X/2, ViewportSize.Y/2);
}

bool FVictoryEdAlignMode::ShouldDrawWidget() const
{
	if(UsingMouseInstantMove) return false;
	return true;
}


bool FVictoryEdAlignMode::GetCursor(EMouseCursor::Type& OutCursor) const
{
	if(UsingMouseInstantMove)
	{
		OutCursor = EMouseCursor::GrabHand;
		return true;
	}
	
	return false; //no modified cursor
}

void FVictoryEdAlignMode::Tick(FEditorViewportClient* ViewportClient,float DeltaTime)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				Single Invalidate
	if(DoSingleDisplayUpdate) 
	{
		ViewportClient->Viewport->InvalidateDisplay();
		DoSingleDisplayUpdate = false;
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				HUD Elements and Input
	Tick_VictoryInterface(ViewportClient);
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//				TRACE CURSOR POSITION
	ProcessMouseInstantMove(ViewportClient);
	
	
	
	//~~~ Begin Vertex Related ~~~
	CHECK_VSELECTED
	
	//Minus or Plus?
	if(MinusIsDown) VictoryEngine->CurrentVerticiesScale -= 0.333;
	else if(PlusIsDown) VictoryEngine->CurrentVerticiesScale += 0.333;
	if(VictoryEngine->CurrentVerticiesScale < 2) VictoryEngine->CurrentVerticiesScale = 2;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//					New Selection Actor?
	if(VictoryEngine->SelectedActorVerticiesNeedsUpdating)
	{
		VictoryEngine->SelectedActorVerticiesNeedsUpdating = false;
		SelectedVertexBuffer = GetVerticies(Cast<AStaticMeshActor>(VictoryEngine->VSelectedActor));
		PendingButtonRefresh = true;
	}
	
	if(VictoryEngine->ClearSelectedVertex)
	{
		VictoryEngine->ClearSelectedVertex = false;
		SelectedVertexForSelectedActor = -1;
	}
}

#undef LOCTEXT_NAMESPACE