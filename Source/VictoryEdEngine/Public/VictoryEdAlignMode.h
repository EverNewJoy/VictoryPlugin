#pragma once

#include "UnrealEd.h" 
#include "Editor.h"

//FVertex Buffer
#include "StaticMeshResources.h"

#include "JoyISM.h"

//Input
#include "InputCoreTypes.h"

class UVictoryEdEngine;

struct FVButton;
struct FDropToClosestSurfaceData;
  
class FVictoryEdAlignMode : public FEdMode
{
	//Another Redo/Undo Method 
	/*
	FORCEINLINE void SaveUndoRedoSimple(UObject* Obj)
	{
		if(!Obj) return;
		Obj->SetFlags( RF_Transactional );
		Obj->Modify();
	}
	*/
	
	
	
//Statics
public:
	static const FColor RV_VRed;
	static const FColor RV_VBlue;
	static const FColor RV_VYellow;
	static const FLinearColor RV_Red;
	static const FLinearColor RV_Yellow;
	static const FLinearColor RV_Blue;

//Display Updates
public:
	bool DoSingleDisplayUpdate;
	
//Joy ISM
public:
	void PerformJoyISM();
	void ReverseJoyISM();
	
//Drop to Surface
public:
	
	//if multiple selected, the test runs on each and finds the nearest of nearest surfaces
	void DropSelectedActorsToNearestSurface();
	
	FDropToClosestSurfaceData* GetActorWithShortestDrop(TArray<FDropToClosestSurfaceData>& TheData);
	
	FORCEINLINE void GetBottomSurfacePoint(AStaticMeshActor* TheSMA, FVector& BottomMostPoint)
	{
		if(!TheSMA) return;
		if(!TheSMA->GetStaticMeshComponent()) BottomMostPoint = FVector::ZeroVector;
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		FVector Origin;
		FVector Extent;
		TheSMA->GetActorBounds(true,Origin,Extent);
		TheSMA->GetStaticMeshComponent()->GetDistanceToCollision(Origin - FVector(0,0,Extent.Z + 100),BottomMostPoint);
	}
//Vertex:
public:
	
	void GetSelectedVertexLocation(FVector& LocOut);
	
	//Snap Key Pressed!
	bool SnapKeyPressed;
		
	//Snap!
	void DoVertexSnap(const FVector& Dest);
	
	FPositionVertexBuffer* 			SelectedVertexBuffer;
	FPositionVertexBuffer* 			HighlightedVertexBuffer;
	
	//Highlighted Actor
	AStaticMeshActor* 				HighlightedActor;
	
	//Buttons
	TArray<FVButton> SelectedActorButtons;
	TArray<FVButton> HighlightedActorButtons;
	
	//CheckArrays - find the vertex button closest to camera
	TArray<FVButton*> ClosestSelectedActorButtons;
	TArray<FVButton*> ClosestHighlightedActorButtons;
	int32 FindClosestOfButtons(TArray<FVButton*>& Inbuttons);
	void RefreshVertexButtons(const FSceneView* View);
	//~~~~~~~~~
	
	//Vertex Scale
	bool PlusIsDown;
	bool MinusIsDown;
	
	//Index of Vertex, so actor can move and maintain selection
	int32 SelectedVertexForSelectedActor;
	int32 HighlightedVertexForSelectedActor;
	int32 HighlightedVertexForHighlightedActor;
	
	//~~~ Re-used ~~~
	FVector VertexWorldSpace;
	FVector2D Vertex2DCenter;

	//Vertex Functions
	void PDI_DrawVerticies(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FPositionVertexBuffer* VertexBuffer, const FTransform& SMATransform, bool DrawingSelectedActor);
	
	//Get Vertex Bufer
	FPositionVertexBuffer* GetVerticies(AStaticMeshActor* TheSMA);
	
//Mouse Instant Move SMA
public:
	bool UsingMouseInstantMove;
	void ProcessMouseInstantMove(FEditorViewportClient* ViewportClient);
	//
//Selection
public:
	AActor* VSelectItrActor;
	
//Input Related
public:
	FVector2D 		MouseLocation;
	FVector2D 		PrevMouseLocation;
	FVector 	CursorWorldPrevPos;
	FVector 	CursorWorldPos;
	FVector 	CursorWorldDir;
	FVector 	CursorWorldDelta;
	
	void InputKeyPressed(FKey Key);
	void InputKeyReleased(FKey Key);
	void InputKeyDoubleClick(FKey Key);
	
	void MiddleMouseEvent(EInputEvent Event);
	void LeftMouseClick();
	
	
	//States
	bool RMBDown;
	bool XDown;
	bool YDown;
	bool ShiftDown;
	
//HUD
public:
	FIntPoint ViewportSize;
	
	void DrawHotkeyToolTip(FCanvas* Canvas);
	void DrawMouseCoordinates(FCanvas* Canvas);
	
	void CheckCursorInButtons(FCanvas* Canvas);
	TArray<FVButton> 	VictoryButtons;
	int32 					ActiveButton_Vibe;
	FVButton* 			CurCheckButton;
	void RefreshVictoryButtons();
	bool PendingButtonRefresh;
	
//Utility
public:
	void DrawVictoryText(FCanvas* Canvas, const FString& TheStr, float X, float Y, float TheScale=1 );
	void DrawVictoryTextWithColor(FCanvas* Canvas, const FString& TheStr, float X, float Y, FLinearColor TheColor=FLinearColor(1,0,1,1), float TheScale=1 );
	void DrawVictoryLine(FCanvas* Canvas, const FVector2D& Start, const FVector2D& End, FLinearColor TheColor=FLinearColor(1,0,1,1), float Thick=0 );
	void DrawVictoryRect(FCanvas* Canvas, float X, float Y, float Width, float Height, FLinearColor Color=FLinearColor(0,0,1,1));

	//Scene-view Level Project and Deproject
	void VictoryProject(		const FSceneView* View,const FVector& WorldPoint,		FVector2D& OutPixelLocation);
	void VictoryDeProject(	const FSceneView* View,const FVector2D& ScreenPoint, FVector& out_WorldOrigin, FVector& out_WorldDirection);
	
	//Box from Point with Size
	FORCEINLINE FBox BoxFromPointWithSize(const FVector& InCenter, const float& Size)
	{
		return FBox(
			InCenter+(FVector(1,1,1) * -Size/2),
			InCenter+(FVector(1,1,1) * Size/2)
		);
	}
	
//RV
public:
	int32 RV_Int32;
	float RV_yStart;
	float RV_Float;
	FVector RV_Vect;
	FRotator RV_Rot;
	FCollisionQueryParams RV_TraceParams;
	FHitResult RV_Hit;
	AActor* RV_HitActor;
//Victory Title
//Time
public:
	FDateTime VictoryTitleAppearTime;
	FTimespan CurTimeSpan;
	bool VictoryTitleVisible;
	bool FadeInVictoryTitle;
	float VictoryTitleAlpha;
	
	void VictoryTitleAppears();
	
//Tick
public:
	void Tick_VictoryTitle(FEditorViewportClient* ViewportClient);
	void Tick_VictoryTitleFadeIn();
	void Tick_VictoryInterface(FEditorViewportClient* ViewportClient);
		
//Core
public:
	UPROPERTY()
	UVictoryEdEngine* VictoryEngine;
	
	bool ReEntering;
	
//Constructor/Destructor
public:
	FVictoryEdAlignMode(); 
	~FVictoryEdAlignMode(); 
	void JoyInit(UVictoryEdEngine* EnginePtr);
	
//FEdMode
public:
	virtual void Enter() override;
	virtual void Exit() override;
	
	//More fundamental than DrawHUD
	virtual void Render(const FSceneView* View,FViewport* Viewport,FPrimitiveDrawInterface* PDI) override;
	
	virtual void DrawHUD(FEditorViewportClient* ViewportClient,FViewport* Viewport,const FSceneView* View,FCanvas* Canvas) override;
	virtual bool InputKey(FEditorViewportClient* ViewportClient,FViewport* Viewport,FKey Key,EInputEvent Event) override;
	virtual bool MouseMove(FEditorViewportClient* ViewportClient,FViewport* Viewport,int32 x, int32 y) override;
	virtual bool InputAxis(FEditorViewportClient* InViewportClient,FViewport* Viewport,int32 ControllerId,FKey Key,float Delta,float DeltaTime) override;
	/** Notifies all active modes of any change in mouse movement */
	virtual bool InputDelta( FEditorViewportClient* InViewportClient,FViewport* InViewport,FVector& InDrag,FRotator& InRot,FVector& InScale ) override;
	
	virtual bool HandleClick(FEditorViewportClient* InViewportClient, HHitProxy *HitProxy, const FViewportClick &Click) override;
	virtual void Tick(FEditorViewportClient* ViewportClient,float DeltaTime) override;
	
	//Cursor
	virtual bool GetCursor(EMouseCursor::Type& OutCursor) const override; 
	
	/**
	 * Lets the mode determine if it wants to draw the widget or not.
	 */
	virtual bool ShouldDrawWidget() const;
};

