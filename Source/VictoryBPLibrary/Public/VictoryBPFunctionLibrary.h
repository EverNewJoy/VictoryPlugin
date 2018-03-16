/*

	By Rama

*/
#pragma once

//to prevent nodes from getting called in constructors:
//meta=(UnsafeDuringActorConstruction = "true")

#include "VictoryISM.h"

//~~~~~~~~~~~~ UMG ~~~~~~~~~~~~~~~
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//AI
#include "AIController.h"		//MoveToWithFilter

//Audio
#include "Components/AudioComponent.h"
#include "AudioDecompress.h"
#include "AudioDevice.h"
#include "ActiveSound.h"
#include "Audio.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"
#include "VorbisAudioInfo.h"

//Texture2D
//#include "Engine/Texture2D.h"
#include "DDSLoader.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "VictoryBPFunctionLibrary.generated.h"

// BP Library for You
//
// Written by Rama

//note about UBlueprintFunctionLibrary
// This class is a base class for any function libraries exposed to blueprints.
// Methods in subclasses are expected to be static, and no methods should be added to the base class.


//~~~~~~~~~~~~~~~~~~~~~~
//			Key Modifiers
//~~~~~~~~~~~~~~~~~~~~~~
UENUM(BlueprintType)
enum class EJoyImageFormats : uint8
{
	JPG		UMETA(DisplayName="JPG        "),
	PNG		UMETA(DisplayName="PNG        "),
	BMP		UMETA(DisplayName="BMP        "),
	ICO		UMETA(DisplayName="ICO        "),
	EXR		UMETA(DisplayName="EXR        "),
	ICNS		UMETA(DisplayName="ICNS        ")
};

//! REMOVE THIS AFTER A FEW VERSIONS
//!
//!
UENUM(BlueprintType)
enum class EVictoryHMDDevice : uint8
{
	None					UMETA(DisplayName="None"),
	OculusRift				UMETA(DisplayName="Oculus Rift"),
	Morpheus				UMETA(DisplayName="Morpheus"),
	ES2GenericStereoMesh	UMETA(DisplayName="ES2 Generic Stereo Mesh"),
	SteamVR					UMETA(DisplayName="Vive (Steam VR)"),
	GearVR					UMETA(DisplayName="Gear VR")
};

USTRUCT(BlueprintType)
struct FVictoryInput
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FString ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FString KeyAsString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	uint32 bShift:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	uint32 bCtrl:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	uint32 bAlt:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	uint32 bCmd:1;


	FVictoryInput(){}
	FVictoryInput(const FString InActionName, const FKey InKey, const bool bInShift, const bool bInCtrl, const bool bInAlt, const bool bInCmd)
		: Key(InKey)
		, KeyAsString(InKey.GetDisplayName().ToString())
		, bShift(bInShift)
		, bCtrl(bInCtrl)
		, bAlt(bInAlt)
		, bCmd(bInCmd)
	{
		ActionName = InActionName;
	}

	FVictoryInput(const FInputActionKeyMapping& Action)
		: Key(Action.Key)
		, KeyAsString(Action.Key.GetDisplayName().ToString())
		, bShift(Action.bShift)
		, bCtrl(Action.bCtrl)
		, bAlt(Action.bAlt)
		, bCmd(Action.bCmd)
	{
		ActionName = Action.ActionName.ToString();
	}
};

USTRUCT(BlueprintType)
struct FVictoryInputAxis
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FString AxisName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FString KeyAsString = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Song")
	float Scale = 1;

	FVictoryInputAxis(){}
	FVictoryInputAxis(const FString InAxisName, FKey InKey, float InScale)
		: AxisName(InAxisName)
		, KeyAsString(InKey.GetDisplayName().ToString())
		, Key(InKey)
		, Scale(InScale)
	{ }

	FVictoryInputAxis(const FInputAxisKeyMapping& Axis)
		: KeyAsString(Axis.Key.GetDisplayName().ToString())
		, Key(Axis.Key)
		, Scale(Axis.Scale)
	{
		AxisName = Axis.AxisName.ToString();
	}
};

UENUM(BlueprintType)
namespace EJoyGraphicsFullScreen
{
	//256 entries max
	enum Type
	{
		FullScreen							UMETA(DisplayName="Regular Full Screen"),
		WindowedFullScreen					UMETA(DisplayName="Windowed Full Screen High Quality"),
		WindowedFullScreenPerformance		UMETA(DisplayName="Windowed Full Screen (Default)"),
		//~~~

		//256th entry
		EJoyGraphicsFullScreen_Max		UMETA(Hidden),
	};
}

USTRUCT(BlueprintType)
struct FLevelStreamInstanceInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	FName PackageName;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	FName PackageNameToLoad;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	FVector Location;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	uint8 bShouldBeLoaded:1;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	uint8 bShouldBeVisible:1;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	uint8 bShouldBlockOnLoad:1;

	UPROPERTY(Category = "LevelStreaming", BlueprintReadWrite)
	int32 LODIndex;

	FLevelStreamInstanceInfo() {}

	FLevelStreamInstanceInfo(ULevelStreamingKismet* LevelInstance);

	FString ToString() const
	{
		return FString::Printf(TEXT("PackageName: %s\nPackageNameToLoad:%s\nLocation:%s\nRotation:%s\nbShouldBeLoaded:%s\nbShouldBeVisible:%s\nbShouldBlockOnLoad:%s\nLODIndex:%i")
			, *PackageName.ToString()
 			, *PackageNameToLoad.ToString()
			, *Location.ToString()
 			, *Rotation.ToString()
 			, (bShouldBeLoaded) ? TEXT("True") : TEXT("False")
 			, (bShouldBeVisible) ? TEXT("True") : TEXT("False")
 			, (bShouldBlockOnLoad) ? TEXT("True") : TEXT("False")
 			, LODIndex);
	}
};


UCLASS()
class VICTORYBPLIBRARY_API UVictoryBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	//~~~~~~~~~~~~~~~~~~
	// 	Level Generation
	//~~~~~~~~~~~~~~~~~~
	/**  Load a level to a specific location and rotation, can create multiple of the same level!
	*
	* Ensure that each InstanceNumber is unique to spawn multiple instances of the same level!
	*
	*  <3 Rama
	*
	* @param MapFolderOffOfContent - Maps or Maps/TileSets/TileSet1  etc
	* @param LevelName - Just the level name itself, such as Tile1
	* @param InstanceNumber - Ensure this is unique by keeping count to spawn as many instances of same level as you want!
	* @param Location - Worldspace location where the level should be spawned
	* @param Rotation - Worldspace rotation for rotating the entire level
	* @return false if the level name was not found
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Dynamic Level Generation",meta=(DeprecatedFunction, DeprecationMessage="My LoadLevelInstance BP node is in the main UE4 Engine as of 4.13! This version is deprecated and will be removed in the near future. <3 -Rama",WorldContext="WorldContextObject"))
	static ULevelStreaming* VictoryLoadLevelInstance(UObject* WorldContextObject, FString MapFolderOffOfContent, FString LevelName, int32 InstanceNumber, FVector Location, FRotator Rotation,bool& Success);

	//~~~~~~~~~~
	// 	AI
	//~~~~~~~~~~
	/** Move to Location with optional Query Filter!
	*
	* 1. Create Custon Nav Area Classes.
	*
	* 2. Use Nav Modifier Volumes to apply custom area class data within the level, then
	*
	* 3. Create Query Filters which alter/exclude those custom nav areas.
	*
	* 4. Can then choose to use the filters per character or even per Move To using this node.
	*
	*  <3 Rama
	*
	* @param FilterClass - Allows different types of units to path in different ways all the time, or path differently per Move To using this node!
	* @param bProjectDestinationToNavigation - Whether to attempt to find a nearby point on the nav mesh below/above/close to the supplied point. Uses the Agent's Nav Extent for the projection
	* @param bStopOnOverlap - Add pawn's radius to AcceptanceRadius
	* @param bCanStrafe - Set focus related flag: bAllowStrafe
	* @return Whether the Pawn's AI Controller is valid and goal can be pathed to
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|AI")
	static EPathFollowingRequestResult::Type Victory_AI_MoveToWithFilter(
		APawn* Pawn,
		const FVector& Dest,
		TSubclassOf<UNavigationQueryFilter> FilterClass = NULL,
		float AcceptanceRadius = 0,
		bool bProjectDestinationToNavigation = false,
		bool bStopOnOverlap = false,
		bool bCanStrafe = false
	);

	//~~~~~~~~~~~~~~~~
	// 	GPU  <3 Rama
	//~~~~~~~~~~~~~~~~

	UFUNCTION(BlueprintPure,Category="Victory BP Library|GPU")
	static FString Victory_GetGPUBrand()
	{
		return FPlatformMisc::GetPrimaryGPUBrand();
	}
	UFUNCTION(BlueprintPure,Category="Victory BP Library|GPU", meta=(Keywords="GPU"))
	static FString Victory_GetGRHIAdapterName()
	{
		return GRHIAdapterName;
	}

	UFUNCTION(BlueprintPure,Category="Victory BP Library|GPU")
	static void Victory_GetGPUInfo(FString& DeviceDescription, FString& Provider, FString& DriverVersion, FString& DriverDate);

	//~~~~~~~~~~
	// 	Core
	//~~~~~~~~~~

	/**
		Launch a new process, if it is not set to be detached, UE4 will not fully close until the other process completes.

		The new process id is returned!

		Priority options: -2 idle, -1 low, 0 normal, 1 high, 2 higher

		♥ Rama
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|System")
	static void VictoryCreateProc(int32& ProcessId, FString FullPathOfProgramToRun,TArray<FString> CommandlineArgs,bool Detach,bool Hidden, int32 Priority=0, FString OptionalWorkingDirectory="");

	/** You can obtain ProcessID from processes you initiate via VictoryCreateProc */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
	static FString VictoryGetApplicationName(int32 ProcessId)
	{
		//Please note it should really be uint32 but that is not supported by BP yet
		return FPlatformProcess::GetApplicationName(ProcessId);
	}

	/** You can obtain ProcessID from processes you initiate via VictoryCreateProc */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
	static bool VictoryIsApplicationRunning( int32 ProcessId )
	{
		//Please note it should really be uint32 but that is not supported by BP yet
		return FPlatformProcess::IsApplicationRunning(ProcessId);
	}
	/* Blueprints does not support int64 so at some pt in future int32 will not be enough, probably by then dolphins will rule the world, or UE4 BP will support int64, or both!  <3 Rama*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
	static int32 GetUnixTimeStamp(const FDateTime& UTCTime)
	{
		//Please note it should really be int64 but that is not supported by BP yet
		return UTCTime.ToUnixTimestamp();
	}
	/* Blueprints does not support int64 so at some pt in future int32 will not be enough, probably by then dolphins will rule the world, or UE4 BP will support int64, or both!  <3 Rama*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
	static void GetUTCFromUnixTimeStamp(int32 UnixTimeStamp, FDateTime& UTCTime)
	{
		//Please note it should really be int64 but that is not supported by BP yet
		UTCTime = FDateTime::FromUnixTimestamp( UnixTimeStamp );
	}

	UFUNCTION(BlueprintPure, Category = "Rama Save System|File IO")
	static void UTCToLocal(const FDateTime& UTCTime, FDateTime& LocalTime)
	{
		//Turn UTC into local ♥ Rama
		FTimespan UTCOffset = FDateTime::Now() - FDateTime::UtcNow();
		LocalTime = UTCTime;
		LocalTime += UTCOffset;
		//♥ Rama
	}

	/** Game thread may pause while hashing is ocurring. Please note that hashing multi-gb size files is very very slow, smaller files will process much faster :) <3 Rama*/
	UFUNCTION(BlueprintCallable,Category="Victory BP Library|MD5")
	static bool CreateMD5Hash(FString FileToHash, FString FileToStoreHashTo );

	/** Game thread may pause while hashing is ocurring. Please note that hashing multi-gb size files is very very slow, smaller files will process much faster :) <3 Rama */
	UFUNCTION(BlueprintCallable,Category="Victory BP Library|MD5")
	static bool CompareMD5Hash(FString MD5HashFile1, FString MD5HashFile2 );

	/** Dynamically change how frequently in seconds a component will tick! Can be altered at any point during game-time! ♥ Rama */
	UFUNCTION(BlueprintCallable,Category="Victory BP Library|System")
	static void SetComponentTickRate(UActorComponent* Component, float Seconds)
	{
		if(!Component) return;
		Component->PrimaryComponentTick.TickInterval = Seconds;
	}

	/** Retrieves command line arguments that were passed into unreal */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
		static const FString GetCommandLine()
	{
		return FCommandLine::Get();
	}

	/**
	* Create a new Texture Render Target 2D, ideal for use with Scene Capture Components created during runtime that need their own unique Render Targets
	* @param 	Width Texture Width
	* @param 	Height Texture Height
	* @param	ClearColor The color the texture is cleared to
	* @param 	Gamma Will override FTextureRenderTarget2DResource::GetDisplayGamma if > 0.
	* @return	A new Texture Render Target 2D!
	*
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
	static UTextureRenderTarget2D* CreateTextureRenderTarget2D(int32 Width=256, int32 Height=256, FLinearColor ClearColor = FLinearColor::White, float Gamma = 1)
	{
		UTextureRenderTarget2D* NewRenderTarget2D = NewObject<UTextureRenderTarget2D>();
		if(!NewRenderTarget2D)
		{
			return nullptr;
		}
		NewRenderTarget2D->ClearColor = FLinearColor::White;
		NewRenderTarget2D->TargetGamma = Gamma;
		NewRenderTarget2D->InitAutoFormat(Width, Height);
		return NewRenderTarget2D;
	}

	//~~~~~~~~~~
	// 	Physics
	//~~~~~~~~~~

	/** Update the Angular Damping during runtime! Make sure the component is simulating physics before calling this! Returns false if the new value could not be set. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
	static bool VictoryPhysics_UpdateAngularDamping(UPrimitiveComponent* CompToUpdate, float NewAngularDamping);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Physics", meta=(Keywords="Closest Surface"))
	static float GetDistanceToCollision(UPrimitiveComponent* CollisionComponent, const FVector& Point, FVector& ClosestPointOnCollision);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Physics", meta=(Keywords="Closest Surface"))
	static float GetDistanceBetweenComponentSurfaces(UPrimitiveComponent* CollisionComponent1, UPrimitiveComponent* CollisionComponent2, FVector& PointOnSurface1, FVector& PointOnSurface2);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics", meta=(Keywords="APEX Piece fracture damage PhysX Physics"))
	static bool VictoryDestructible_DestroyChunk(UDestructibleComponent* DestructibleComp, int32 HitItem);


	//~~~~~~~~~~
	// 	Joy ISM
	//~~~~~~~~~~

	/** Retrieve an array of all of the Victory Instanced Static Mesh Actors that have been created during runtime! */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Instanced Static Mesh",meta=(WorldContext="WorldContextObject"))
	static void VictoryISM_GetAllVictoryISMActors(UObject* WorldContextObject, TArray<AVictoryISM*>& Out);

	/** Finds all instances of a specified Blueprint or class, and all subclasses of this class, and converts them into a single Instanced Static Mesh Actor! Returns the created Victory ISM actors. Please note all actors of subclasses are found as well, so use a very specific blueprint / class if you only want to generate Victory ISM actors for specific classes! Ignores actor classes that dont have a static mesh component. Please note that instanced static mesh actors can only be created for actors sharing the same static mesh asset. Different Instanced Static Mesh Actors are created for each unique static mesh asset found in the whole group of actors! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Instanced Static Mesh",meta=(WorldContext="WorldContextObject"))
	static void VictoryISM_ConvertToVictoryISMActors(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, TArray<AVictoryISM*>& CreatedISMActors, bool DestroyOriginalActors=true, int32 MinCountToCreateISM=2);

	//~~~~~~~~~~
	// 	File I/O
	//~~~~~~~~~~

	/** Obtain all files in a provided directory, with optional extension filter. All files are returned if Ext is left blank. Returns false if operation could not occur. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|File IO")
	static bool JoyFileIO_GetFiles(TArray<FString>& Files, FString RootFolderFullPath, FString Ext);

	/** Obtain all files in a provided root directory, including all subdirectories, with optional extension filter. All files are returned if Ext is left blank. The full file path is returned because the file could be in any subdirectory. Returns false if operation could not occur. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|File IO")
	static bool JoyFileIO_GetFilesInRootAndAllSubFolders(TArray<FString>& Files, FString RootFolderFullPath, FString Ext);

	/** Obtain a listing of all SaveGame file names that were saved using the Blueprint Save Game system. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|File IO")
	static void SaveGameObject_GetAllSaveSlotFileNames(TArray<FString>& FileNames);

	/** Returns false if the new file could not be created. The folder path must be absolute, such as C:\Users\Self\Documents\YourProject\MyPics. You can use my other Paths nodes to easily get absolute paths related to your project! <3 Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Screenshots", meta=(Keywords="High resolution"))
	static bool ScreenShots_Rename_Move_Most_Recent(FString& OriginalFileName, FString NewName, FString NewAbsoluteFolderPath, bool HighResolution=true);

	//~~~~ Key Re Binding ! ~~~~

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Key Rebinding")
	static void VictoryGetAllAxisAndActionMappingsForKey(FKey Key, TArray<FVictoryInput>& ActionBindings, TArray<FVictoryInputAxis>& AxisBindings);

	//	Axis Mapping
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Key Rebinding")
	static FVictoryInputAxis VictoryGetVictoryInputAxis(const FKeyEvent& KeyEvent);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Key Rebinding")
	static void VictoryGetAllAxisKeyBindings(TArray<FVictoryInputAxis>& Bindings);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Key Rebinding")
	static void VictoryRemoveAxisKeyBind(FVictoryInputAxis ToRemove);

	/** You can leave the AsString field blank :) Returns false if the key could not be found as an existing mapping!  Enjoy! <3  Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Key Rebinding")
	static bool VictoryReBindAxisKey(FVictoryInputAxis Original, FVictoryInputAxis NewBinding);

	static FORCEINLINE void UpdateAxisMapping(FInputAxisKeyMapping& Destination, const FVictoryInputAxis& VictoryInputBind)
	{
		Destination.Key = VictoryInputBind.Key;
		Destination.Scale = VictoryInputBind.Scale;
	}


	//	Action Mapping
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Key Rebinding")
	static FVictoryInput VictoryGetVictoryInput(const FKeyEvent& KeyEvent);

	static FORCEINLINE void UpdateActionMapping(FInputActionKeyMapping& Destination, const FVictoryInput& VictoryInputBind)
	{
		Destination.Key = VictoryInputBind.Key;
		Destination.bShift = VictoryInputBind.bShift;
		Destination.bCtrl = VictoryInputBind.bCtrl;
		Destination.bAlt = VictoryInputBind.bAlt;
		Destination.bCmd = VictoryInputBind.bCmd;
	}

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Key Rebinding")
		static void VictoryGetAllActionKeyBindings(TArray<FVictoryInput>& Bindings);

	/** You can leave the AsString field blank :) Returns false if the key could not be found as an existing mapping!  Enjoy! <3  Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Key Rebinding")
	static bool VictoryReBindActionKey(FVictoryInput Original, FVictoryInput NewBinding);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Key Rebinding")
	static void VictoryRemoveActionKeyBind(FVictoryInput ToRemove);

	//~~~~~~~~~~~~~~~~~~~~

	/** Change volume of Sound class of your choosing, sets the volume instantly! Returns false if the sound class was not found and volume was not set. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Sound")
	static bool VictorySoundVolumeChange(USoundClass* SoundClassObject, float NewVolume);

	/** Get Current Sound Volume! Returns -1 if sound class was not found*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Sound")
	static float VictoryGetSoundVolume(USoundClass* SoundClassObject);

	//~~~~~~~~~~~~~~~~~~~~

	/** The number of seconds that this actor has been in play, relative to Get Game Time In Seconds. */
	UFUNCTION(BlueprintPure,  Category = "Victory BP Library|Actor")
	static float GetTimeInPlay(AActor* Actor)
	{
		if(!Actor) return -1;

		UWorld* World = Actor->GetWorld();

		//Use FApp Current Time as a back up
		float CurrentTime = (World) ? World->GetTimeSeconds() : FApp::GetCurrentTime();
		return CurrentTime - Actor->CreationTime;
	}

	/**
	* Creates a plane centered on a world space point with a facing direction of Normal.
	*
	* @param Center  The world space point the plane should be centered on (easy to observe with DrawDebugPlane)
	* @param Normal  The facing direction of the plane (can receive a Rotator)
	* @return Plane coordinates
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Math|Plane", meta=(Keywords="make base plane"))
	static FPlane CreatePlane(FVector Center, FVector Normal)
	{
		return FPlane(Center,Normal);
	}

	/** >0: point is in front of the plane, <0: behind, =0: on the plane **/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Math|Plane")
	static void PointDistanceToPlane(const FPlane& Plane, FVector Point,float& Distance)
	{
		Distance = Plane.PlaneDot(Point);
	}

	/** Use a larger tolerance to allow inaccuracy of measurement in certain situations */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Math|Plane")
	static bool IsPointOnPlane(const FPlane& Plane, FVector Point, float Tolerance= 0.01)
	{
		return FMath::Abs(Plane.PlaneDot(Point)) < Tolerance;
	}

	/** Easily add to an integer! <3 Rama*/
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "+=",Keywords = "increment integer"), Category = "Victory BP Library|Math|Integer")
	static void VictoryIntPlusEquals(UPARAM(ref) int32& Int, int32 Add, int32& IntOut);

	/** Easily subtract from an integer! <3 Rama*/
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "-=",Keywords = "decrement integer"), Category = "Victory BP Library|Math|Integer")
	static void VictoryIntMinusEquals(UPARAM(ref) int32& Int, int32 Sub, int32& IntOut);

	/** Easily add to a float! <3 Rama*/
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "+=",Keywords = "increment float"), Category = "Victory BP Library|Math|Float")
	static void VictoryFloatPlusEquals(UPARAM(ref) float& Float, float Add, float& FloatOut);

	/** Easily subtract from a float! <3 Rama*/
	UFUNCTION(BlueprintCallable, meta = (CompactNodeTitle = "-=",Keywords = "decrement float"), Category = "Victory BP Library|Math|Float")
	static void VictoryFloatMinusEquals(UPARAM(ref) float& Float, float Sub, float& FloatOut);

	/** Sort an integer array, smallest value will be at index 0 after sorting. Modifies the input array, no new data created. <3 Rama */
	UFUNCTION(BlueprintCallable, meta = (Keywords = "sort integer array"), Category = "Victory BP Library|Array")
	static void VictorySortIntArray(UPARAM(ref) TArray<int32>& IntArray, TArray<int32>& IntArrayRef);

	/** Sort a float array, smallest value will be at index 0 after sorting. Modifies the input array, no new data created. */
	UFUNCTION(BlueprintCallable, meta = (Keywords = "sort float array"), Category = "Victory BP Library|Array")
	static void VictorySortFloatArray(UPARAM(ref) TArray<float>& FloatArray, TArray<float>& FloatArrayRef);


	/* Returns true if vector2D A is equal to vector2D B (A == B) within a specified error tolerance */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Equal (vector2D)", CompactNodeTitle = "==", Keywords = "== equal"), Category="Victory BP Library|Math|Vector2D")
	static bool EqualEqual_Vector2DVector2D(FVector2D A, FVector2D B, float ErrorTolerance = 1.e-4f)
	{
		return A.Equals(B,ErrorTolerance);
	}

	/* Returns true if vector2D A is not equal to vector2D B (A != B) within a specified error tolerance */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Not Equal (vector2D)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category="Victory BP Library|Math|Vector2D")
	static bool NotEqual_Vector2DVector2D(FVector2D A, FVector2D B, float ErrorTolerance = 1.e-4f)
	{
		return !A.Equals(B,ErrorTolerance);
	}

	//~~~

	/**
	 * Tries to reach Target based on distance from Current position, giving a nice smooth feeling when tracking a position.
	 *
	 * @param		Current			Actual position
	 * @param		Target			Target position
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	//UFUNCTION(BlueprintPure, Category="Math|Interpolation", meta=(Keywords="position"))
	UFUNCTION(BlueprintPure, Category="Victory BP Library|Math", meta=(Keywords="position"))
	static FVector2D Vector2DInterpTo(FVector2D Current, FVector2D Target, float DeltaTime, float InterpSpeed);

	/**
	 * Tries to reach Target at a constant rate.
	 *
	 * @param		Current			Actual position
	 * @param		Target			Target position
	 * @param		DeltaTime		Time since last tick
	 * @param		InterpSpeed		Interpolation speed
	 * @return		New interpolated position
	 */
	//UFUNCTION(BlueprintPure, Category="Math|Interpolation", meta=(Keywords="position"))
	UFUNCTION(BlueprintPure, Category="Victory BP Library|Math", meta=(Keywords="position"))
	static FVector2D Vector2DInterpTo_Constant(FVector2D Current, FVector2D Target, float DeltaTime, float InterpSpeed);

	//~~~ Text To Number ~~~

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion")
	static bool Text_IsNumeric(const FText& Text)
	{
		return Text.IsNumeric();
	}

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion", meta=(AdvancedDisplay = "1"))
	static float Text_ToFloat(const FText& Text, bool UseDotForThousands=false)
	{
		//because commas lead to string number being truncated, FText 10,000 becomes 10 for FString
		FString StrFloat = Text.ToString();
		TextNumFormat(StrFloat,UseDotForThousands);
		return FCString::Atof(*StrFloat);
	}

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion", meta=(AdvancedDisplay = "1"))
	static int32 Text_ToInt(const FText& Text, bool UseDotForThousands=false)
	{
		//because commas lead to string number being truncated, FText 10,000 becomes 10 for FString
		FString StrInt = Text.ToString();
		TextNumFormat(StrInt,UseDotForThousands);
		return FCString::Atoi(*StrInt);
	}

	static void TextNumFormat(FString& StrNum, bool UseDotForThousands)
	{
		//10.000.000,997
		if(UseDotForThousands)
		{
			StrNum.ReplaceInline(TEXT("."),TEXT(""));	//no dots as they truncate
			StrNum.ReplaceInline(TEXT(","),TEXT("."));	//commas become decimal
		}

		//10,000,000.997
		else
		{
			StrNum.ReplaceInline(TEXT(","),TEXT(""));  //decimal can stay, commas would truncate so remove
		}
	}

	//~~~ End of Text To Number ~~~

	/** Returns Value mapped from one range into another where the value is clamped to the output range.  (e.g. 0.5 normalized from the range 0->1 to 0->50 would result in 25) */
	UFUNCTION(BlueprintPure, Category="Victory BP Library|Math", meta=(Keywords = "get mapped value clamped"))
	static float MapRangeClamped(float Value, float InRangeA, float InRangeB, float OutRangeA, float OutRangeB);

	/** Server Travel! This is an async load level process which allows you to put up a UMG widget while the level loading occurs! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|System",meta=(WorldContext="WorldContextObject"))
	static void ServerTravel(UObject* WorldContextObject,FString MapName, bool bNotifyPlayers=true);

	/** Get a Player Start by Name! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|System",meta=(WorldContext="WorldContextObject"))
	static APlayerStart* GetPlayerStart(UObject* WorldContextObject,FString PlayerStartName);

	/** Convert String Back To Vector. IsValid indicates whether or not the string could be successfully converted. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion",meta=(DisplayName = "String to Vector", CompactNodeTitle = "->"))
	static void Conversions__StringToVector(const FString& String, FVector& ConvertedVector, bool& IsValid);

	/** Convert String Back To Rotator. IsValid indicates whether or not the string could be successfully converted. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion",meta=(DisplayName = "String to Rotator", CompactNodeTitle = "->"))
	static void Conversions__StringToRotator(const FString& String, FRotator& ConvertedRotator, bool& IsValid);

	/** Convert String Back To Color. IsValid indicates whether or not the string could be successfully converted. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion",meta=(DisplayName = "String to Color", CompactNodeTitle = "->"))
	static void Conversions__StringToColor(const FString& String, FLinearColor& ConvertedColor, bool& IsValid);

	/** Convert Color to String! */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion",meta=(DisplayName = "Color to String ", CompactNodeTitle = "~>"))
	static void Conversions__ColorToString(const FLinearColor& Color, FString& ColorAsString);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	//UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars!")
	//static uint8 Victory_ConvertStringToByte(UEnum* Enum,FString String);
	//! not working yet, always getting 255

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static bool VictoryGetCustomConfigVar_Bool(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static int32 VictoryGetCustomConfigVar_Int(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static float VictoryGetCustomConfigVar_Float(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static FVector VictoryGetCustomConfigVar_Vector(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static FRotator VictoryGetCustomConfigVar_Rotator(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static FLinearColor VictoryGetCustomConfigVar_Color(FString SectionName, FString VariableName, bool& IsValid);

	/** Get Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static FString VictoryGetCustomConfigVar_String(FString SectionName, FString VariableName, bool& IsValid);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Custom Config Vars")
	static FVector2D VictoryGetCustomConfigVar_Vector2D(FString SectionName, FString VariableName, bool& IsValid);

	//~~~~~~~~~~~~~~~~~~~~

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Vector2D(FString SectionName, FString VariableName, FVector2D Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Bool(FString SectionName, FString VariableName, bool Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Int(FString SectionName, FString VariableName, int32 Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
		static void VictorySetCustomConfigVar_Float(FString SectionName, FString VariableName, float Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Vector(FString SectionName, FString VariableName, FVector Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Rotator(FString SectionName, FString VariableName, FRotator Value);

	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_Color(FString SectionName, FString VariableName, FLinearColor Value);


	/** Set Custom Config Var! These are stored in Saved/Config/Windows/Game.ini */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Custom Config Vars")
	static void VictorySetCustomConfigVar_String(FString SectionName, FString VariableName, FString Value);


	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Transform")
	FRotator TransformVectorToActorSpaceAngle(AActor* Actor, const FVector& InVector);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Transform")
	FVector TransformVectorToActorSpace(AActor* Actor, const FVector& InVector);


	UFUNCTION(BlueprintPure, Category = "Victory BP Library|UMG", meta=(keywords="HMD vive oculus rift gearvr morpheus"))
	static FName GetHeadMountedDisplayDeviceType();


	/** The FName that is expected is the exact same format as when you right click on asset -> Copy Reference! You can directly paste copied references into this node! IsValid lets you know if the path was correct or not and I was able to load the object. MAKE SURE TO SAVE THE RETURNED OBJECT AS A VARIABLE. Otherwise your shiny new asset will get garbage collected. I recommend you cast the return value to the appropriate object and then promote it to a variable :)  -Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
	static UObject* LoadObjectFromAssetPath(TSubclassOf<UObject> ObjectClass, FName Path, bool& IsValid);

	/** Uses the same format as I use for LoadObjectFromAssetPath! Use this node to get the asset path of objects in the world! -Rama */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Misc")
	static FName GetObjectPath(UObject* Obj);


	/** Find all widgets of a certain class! Top level only means only widgets that are directly added to the viewport will be found */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|UMG", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "FoundWidgets"))
	static void GetAllWidgetsOfClass(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, TArray<UUserWidget*>& FoundWidgets, bool TopLevelOnly = true);

	/** Remove all widgets of a certain class from viewport! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|UMG", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void RemoveAllWidgetsOfClass(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|UMG", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsWidgetOfClassInViewport(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass);

	/** Handy helper to check if a Key Event was for specified Key ♥ Rama*/
	UFUNCTION(BlueprintPure,Category="Victory BP Library|UMG", meta = (Keywords = "== match same equal"))
	static void JoyIsKey(const FKeyEvent& KeyEvent, FKey Key, bool& Ctrl, bool& Shift, bool& Alt, bool& Cmd, bool& Match)
	{
		Ctrl = KeyEvent.IsControlDown();
		Alt =  KeyEvent.IsAltDown();
		Shift = KeyEvent.IsShiftDown();
		Cmd = KeyEvent.IsCommandDown();
		Match = KeyEvent.GetKey() == Key;
	}

	/** Retrieves the unique net ID for the local player as a number! The number itself will vary based on what Online Subsystem is being used, but you are guaranteed that this number is unique per player! */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Networking")
	static int32 GetPlayerUniqueNetID();

	/** Call this periodically in a huge loop you are intentionally using to reset the BP runaway loop system. Caution, if you have an infinite loop this will permanently hang your system until you turn your computer off. Use very very carefully! When constructing a new loop and you are not sure if it is totally stable, do NOT use this node! Always test new loops normally to ensure you dont truly have a runaway loop that would hang your computer forever. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
	static void Loops_ResetBPRunawayCounter();

	/** Set the Max Frame Rate. Min value is 10. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Graphics Settings")
	static void GraphicsSettings__SetFrameRateCap(float NewValue);

	/** Only hardware dependent, no smoothing */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Graphics Settings")
	static void GraphicsSettings__SetFrameRateToBeUnbound();

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
	static FVector2D ProjectWorldToScreenPosition(const FVector& WorldLocation);

	/** Make sure to save off the return value as a global variable in one of your BPs or else it will get garbage collected! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc", meta = (DeprecatedFunction, DeprecationMessage="Epic has introduced Construct Object as of 4.9.0, I recommend you use that instead! -Rama", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UObject* CreateObject(UObject* WorldContextObject, UClass* TheObjectClass);

	/** Make sure to save off the return value as a global variable in one of your BPs or else it will get garbage collected! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UPrimitiveComponent* CreatePrimitiveComponent(UObject* WorldContextObject, TSubclassOf<UPrimitiveComponent> CompClass, FName Name, FVector Location, FRotator Rotation);



	/** Spawn an Actor and choose which level you want them to spawn into! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static AActor* SpawnActorIntoLevel(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Level = NAME_None, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator, bool SpawnEvenIfColliding = true);

	/** Get the names of all currently loaded and visible levels! */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void GetNamesOfLoadedLevels(UObject* WorldContextObject, TArray<FString>& NamesOfLoadedLevels);




	/** Obtain the scaled,rotated, and translated vertex positions for any static mesh! Returns false if operation could not occur because the comp or static mesh asset was invalid. <3 Rama */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Actor")
	static bool GetStaticMeshVertexLocations(UStaticMeshComponent* Comp, TArray<FVector>& VertexPositions);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor")
	static void AddToActorRotation(AActor* TheActor, FRotator AddRot);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void DrawCircle(
		UObject* WorldContextObject,
		FVector Center,
		float Radius,
		int32 NumPoints = 32,
		float Thickness = 7,
		FLinearColor LineColor = FLinearColor::Red,
		FVector YAxis = FVector(0, 1, 0),
		FVector ZAxis = FVector(0, 0, 1),
		float Duration = 0,
		bool PersistentLines = false
		);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|AI",meta=(WorldContext="WorldContextObject"))
		static AActor* GetClosestActorOfClassInRadiusOfLocation(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FVector Center, float Radius, bool& IsValid);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|AI",meta=(WorldContext="WorldContextObject"))
		static AActor* GetClosestActorOfClassInRadiusOfActor(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, AActor* ActorCenter, float Radius, bool& IsValid);

	/**
	* Generates a box that is guaranteed to contain all of the supplied points.
	*
	* @param Points  The world space points that the box will encompass.
	*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Misc")
	static void GetBoxContainingWorldPoints(const TArray<FVector>& Points, FVector& Center, FVector& Extent)
	{
		FBox Box(ForceInit);

		for(const FVector& Each : Points)
		{
			Box += Each;
		}
		Center = Box.GetCenter();
		Extent = Box.GetExtent();
	}

	/** Implementation of a Selection Marquee / Selection Box as a BP Node. AnchorPoint is the first clicked point, which user then drags from to make the box. Class filter is optional way to narrow the scope of actors that can be selected by the selection box! -Rama*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static void Selection_SelectionBox(UObject* WorldContextObject, TArray<AActor*>& SelectedActors, FVector2D AnchorPoint, FVector2D DraggedPoint, TSubclassOf<AActor> ClassFilter);


	/** Get the Controller ID for a supplied Player Controller <3 Rama. Returns false if operation could not occur.  */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Controller")
		static bool PlayerController_GetControllerID(APlayerController* ThePC, int32& ControllerID);

	/** Get the Unique PlayerID from the PlayerState for a supplied Player Controller. Returns false if operation could not occur. Epic accepted my pull request for this a while back so now you can just GetPlayerState and directly access <3 Rama*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Networking")
	static bool PlayerState_GetPlayerID(APlayerController* ThePC, int32& PlayerID);

	/** Returns whether this game instance is single player.  <3 Rama*/
	UFUNCTION(BlueprintPure, Category="Victory BP Library|Networking", meta=(Keywords="SinglePlayer multiplayer", WorldContext="WorldContextObject"))
	static bool IsStandAlone(UObject* WorldContextObject)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked( WorldContextObject );
		return World ? (World->GetNetMode() == NM_Standalone) : false;
	}


	/** Launches the specified URL in the OS default web browser :) <3 Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|System")
		static void Open_URL_In_Web_Browser(FString TheURL);



	/** Returns which platform the game code is running in.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System")
		static void OperatingSystem__GetCurrentPlatform(
		bool& Windows_,
		bool& Mac,
		bool& Linux,
		bool& iOS,
		bool& Android,
		bool& Android_ARM,
		bool& Android_Vulkan,
		bool& PS4,
		bool& XBoxOne,
		bool& HTML5,
		bool& Apple
		);

	//~~~

	/** Retrieves the OS system Date and Time as a string at the instant this BP node runs. Use my other RealWorldTime node to get the time passed since the return value of this node! You can use this to record milliseconds/seconds/minutes/hours between events in game logic! */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System")
		static FString RealWorldTime__GetCurrentOSTime(
		int32& MilliSeconds,
		int32& Seconds,
		int32& Minutes,
		int32& Hours12,
		int32& Hours24,
		int32& Day,
		int32& Month,
		int32& Year
		);

	/** Get the amount of seconds/minutes/hours since the the supplied DateTime string! You can use this to record milliseconds/seconds/minutes/hours between events in game logic! */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System")
		static void RealWorldTime__GetTimePassedSincePreviousTime(
		const FString& PreviousTime,
		float& Milliseconds,
		float& Seconds,
		float& Minutes,
		float& Hours
		);

	/** Get the difference between two recorded times! You can use this to record milliseconds/seconds/minutes/hours between events in game logic! */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System")
		static void RealWorldTime__GetDifferenceBetweenTimes(
		const FString& PreviousTime1,
		const FString& PreviousTime2,
		float& Milliseconds,
		float& Seconds,
		float& Minutes,
		float& Hours
		);

	//~~~

	/** Loads a text file from hard disk and parses it into a String array, where each entry in the string array is 1 line from the text file. Option to exclude lines that are only whitespace characters or '\n'. Returns the size of the final String Array that was created. Returns false if the file could be loaded from hard disk. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|File IO")
	static bool LoadStringArrayFromFile(TArray<FString>& StringArray, int32& ArraySize, FString FullFilePath = "Enter Full File Path", bool ExcludeEmptyLines = false);

	/** Load a text file to a single string that you can use ParseIntoArray on newline characters if you want same format as LoadStringArrayFromFile. This version supports unicode characters! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|File IO")
	static bool LoadStringFromFile(FString& Result, FString FullFilePath = "Enter Full File Path")
	{
		return FFileHelper::LoadFileToString( Result, *FullFilePath);
	}

	//~~~

	/** Max of all array entries. Returns -1 if the supplied array is empty. Returns the index of the max value as well as the value itself. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Array")
		static void MaxOfFloatArray(const TArray<float>& FloatArray, int32& IndexOfMaxValue, float& MaxValue);

	/** Max of all array entries. Returns -1 if the supplied array is empty. Returns the index of the max value as well as the value itself. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Array")
		static void MaxOfIntArray(const TArray<int32>& IntArray, int32& IndexOfMaxValue, int32& MaxValue);

	/** Min of all array entries. Returns -1 if the supplied array is empty. Returns the index of the min value as well as the value itself. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Array")
		static void MinOfFloatArray(const TArray<float>& FloatArray, int32& IndexOfMinValue, float& MinValue);

	/** Min of all array entries. Returns -1 if the supplied array is empty. Returns the index of the min value as well as the value itself. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Array")
		static void MinOfIntArray(const TArray<int32>& IntArray, int32& IndexOfMinValue, int32& MinValue);

	//~~~

	/** Set Max Move Speed. Supply the Character whose Character Movement to change! Returns false if operation could not occur due to invalid Character or MovementComponent could not be obtained.*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Character", meta = (DefaultToSelf = "TheCharacter"))
		static bool CharacterMovement__SetMaxMoveSpeed(ACharacter* TheCharacter, float NewMaxMoveSpeed);

	//~~~

	/** Converts a float to a rounded Integer, examples: 1.4 becomes 1,   1.6 becomes 2 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|Math")
	static int32 Conversion__FloatToRoundedInteger(float IN_Float);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
	static int32 CountOccurrancesOfSubString(FString Source, FString SubString, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase)
	{
		return Source.ReplaceInline(*SubString,TEXT(""),SearchCase);
	}



	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|String", meta=( Keywords = "concatenate append"))
	static void VictoryAppendInline(UPARAM(ref) FString& String, const FString& ToAppend, FString& Result, bool AppendNewline=false)
	{
		String += ToAppend;
		if(AppendNewline) String += LINE_TERMINATOR;
		Result = String;
	}

	/** Handy option to trim any extra 00: 's while keeping a base set of 00:ss as per user expectation. 00:05:30 will become 05:30. ♥ Rama */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System")
	static FString Victory_SecondsToHoursMinutesSeconds(float Seconds, bool TrimZeroes=true);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
	static bool IsAlphaNumeric(const FString& String);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
	static void Victory_GetStringFromOSClipboard(FString& FromClipboard);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|String")
	static void Victory_SaveStringToOSClipboard(const FString& ToClipboard);

	/**
	 * Returns whether or not the SearchIn string contains the supplied Substring.
	 * 	Ex: "cat" is a contained within "concatenation" as a substring.
	 * @param SearchIn The string to search within
	 * @param Substring The string to look for in the SearchIn string
	 * @param bUseCase Whether or not to be case-sensitive
	 * @param bSearchFromEnd Whether or not to start the search from the end of the string instead of the beginning
	 */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
	static bool HasSubstring(const FString& SearchIn, const FString& Substring, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase, ESearchDir::Type SearchDir = ESearchDir::FromStart);

	/** Combines two strings together! The Separator and the Labels are optional*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
	static FString String__CombineStrings(FString StringFirst, FString StringSecond, FString Separator = "", FString StringFirstLabel = "", FString StringSecondLabel = "");

	/** Separator is always a space */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String", meta=( Keywords = "concatenate append", CommutativeAssociativeBinaryOperator = "true"))
	static FString String__CombineStrings_Multi(FString A, FString B);

	/** Returns three arrays containing all of the resolutions and refresh rates for the current computer's current display adapter. You can loop over just 1 of the arrays and use the current index for the other two arrays, as all 3 arrays will always have the same length. Returns false if operation could not occur. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|System", meta = (Keywords = "screen resolutions display adapter"))
	static bool OptionsMenu__GetDisplayAdapterScreenResolutions(TArray<int32>& Widths, TArray<int32>& Heights, TArray<int32>& RefreshRates, bool IncludeRefreshRates = false);

	UFUNCTION(BlueprintPure, Category = "Victory BP Library|GPU", meta=(Keyword="amd nvidia graphics card brand make model"))
	static void GetUserDisplayAdapterBrand(bool& IsAMD, bool& IsNvidia, bool& IsIntel, bool& IsUnknown, int32& UnknownId);

	/** Clones an actor by obtaining its class and creating a copy. Returns the created Actor. The cloned actor is set to have the rotation and location of the initial actor. You can optionally specify location / rotation offsets for the new clone from original actor. Use IsValid to know if the actor was able to be cloned. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static AStaticMeshActor* Clone__StaticMeshActor(UObject* WorldContextObject, bool&IsValid, AStaticMeshActor* ToClone, FVector LocationOffset = FVector(0, 0, 0), FRotator RotationOffset = FRotator(0, 0, 0));


	/** Teleport Actor To Actor. Moves an actor instantly to the position and rotation of another actor. Useful for player starts, notes, triggers, and any other destination actors who dont have collision. Returns false if the operation could not occur. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|Actor")
		static bool Actor__TeleportToActor(AActor* ActorToTeleport, AActor* DestinationActor);

	/** Is this game logic running in the Editor world? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WorldType__InEditorWorld(UObject* WorldContextObject);

	/** Is this game logic running in the PIE world? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WorldType__InPIEWorld(UObject* WorldContextObject);

	/** Is this game logic running in an actual independent game instance? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Victory BP Library|System", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
		static bool WorldType__InGameInstanceWorld(UObject* WorldContextObject);

	/** Cause a supplied Character (casted from Actor internally) to enter Ragdoll physics. A check will be done to see if the character has a physics asset. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
		static bool Physics__EnterRagDoll(AActor* TheCharacter);

	/** Cause a supplied Character (casted from Actor internally) to exit Ragdoll physics. HeightAboveRBMesh is how far above the RB Mesh the Actor Capsule should be moved to upon exiting. Pass in the InitLocation and InitRotation from InitializeVictoryRagdoll */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
	static bool Physics__LeaveRagDoll(AActor* TheCharacter, bool SetToFallingMovementMode=true, float HeightAboveRBMesh = 64, const FVector& InitLocation = FVector(0, 0, 0), const FRotator& InitRotation = FRotator(0, 0, 0));

	/** Returns whether or not a supplied Character is in Ragdoll Physics. Cast from Actor done internally for your convenience. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
		static bool Physics__IsRagDoll(AActor* TheCharacter);

	/** Get the Ragdoll Position of the supplied actor, casted to Character internally. Returns false if operation could not occur. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
		static bool Physics__GetLocationofRagDoll(AActor* TheCharacter, FVector& RagdollLocation);

	/** Initialize Victory Ragdoll Mode, by Obtaining the Default Relative Rotation and Location for this Character's Mesh. The Output Location and Rotation must be saved for use with LeaveRagdoll. Returns false if operation could not occur */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
		static bool Physics__InitializeVictoryRagDoll(AActor* TheCharacter, FVector&InitLocation, FRotator&InitRotation);

	/** Update the TheCharacter's Capsule Location and Camera to the Current Location of the Ragdoll. InterpSpeed is how fast the camera keeps up with the moving ragdoll! HeightOffset is the height above the ragdoll that the camera should stay. Returns false if operation could not occur or if Mesh was not in ragdoll */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Physics")
		static bool Physics__UpdateCharacterCameraToRagdollLocation(AActor* TheCharacter, float HeightOffset = 128, float InterpSpeed = 3);


	/** This node checks all Scalar, Vector, and Texture parameters of a material to see if the supplied parameter name is an actual parameter in the material! ♥ Rama*/
	UFUNCTION(BlueprintPure,Category="Victory BP Library|Material")
	static bool DoesMaterialHaveParameter(UMaterialInterface* Mat, FName Parameter);


	/** Get Name as String*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|String")
		static FString Accessor__GetNameAsString(const UObject* TheObject);

	/** Get Socket Local Transform. Returns false if operation could not occur.*/
	//UFUNCTION(BlueprintCallable, Category = "Victory BP Library")
	//static bool Accessor__GetSocketLocalTransform(const USkeletalMeshComponent* Mesh, FTransform& LocalTransform, FName SocketName=FName("SocketName"));

	/** Get Player Character's Player Controller. Requires: The Passed in Actor must be a character and it must be a player controlled character. IsValid will tell you if the return value is valid, make sure to do a Branch to confirm this! */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Character")
		static APlayerController* Accessor__GetPlayerController(AActor* TheCharacter, bool&IsValid);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Input")
	static void VictorySimulateMouseWheel(const float& Delta);

	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Input")
	static void VictorySimulateKeyPress(APlayerController* ThePC, FKey Key, EInputEvent EventType);

	/** This handy node lets you turn the rendering of the entire world on or off! Does not affect UMG or HUD, which allows you to use loading screens effectively! <3 Rama. Returns false if player controller could not be used to get the viewport. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|System", meta=(keywords="disable show hide loading screen"))
	static bool Viewport__EnableWorldRendering(const APlayerController* ThePC, bool RenderTheWorld);

	/** SET the Mouse Position! Returns false if the operation could not occur */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Input")
		static bool Viewport__SetMousePosition(const APlayerController* ThePC, const float& PosX, const float& PosY);

	/** Get the Cursor Position within the Player's Viewport. This will return a result consistent with SET Mouse Position Returns false if the operation could not occur */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Game Window")
		static bool Viewport__GetMousePosition(const APlayerController* ThePC, float& PosX, float& PosY);


	/** Get the coordinates of the center of the player's screen / viewport. Returns false if the operation could not occur */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Game Window")
		static bool Viewport__GetCenterOfViewport(const APlayerController* ThePC, float& PosX, float& PosY);


	/** Convert Vector to Rotator*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion")
		static FRotator Conversions__VectorToRotator(const FVector& TheVector);

	/** Convert Rotator to Vector */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion")
		static FVector Conversions__RotatorToVector(const FRotator& TheRotator);

	/** Input Actor is expected to be a ACharacter, conversion done internally for your convenience */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion")
		static FRotator Character__GetControllerRotation(AActor * TheCharacter);



	/** Draw 3D Line of Chosen Thickness From Socket on Character's Mesh to Destination, conversion of AActor to ACharacter done internally for your convenience. Duration is in Seconds */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|3d Lines")
		static void Draw__Thick3DLineFromCharacterSocket(AActor* TheCharacter, const FVector& EndPoint, FName Socket = FName("SocketName"), FLinearColor LineColor = FLinearColor(1, 0, 0, 1), float Thickness = 7, float Duration = -1.f);
	/** Draw 3D Line of Chosen Thickness From Mesh Socket to Destination. Duration is in Seconds */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|3d Lines")
		static void Draw__Thick3DLineFromSocket(USkeletalMeshComponent* Mesh, const FVector& EndPoint, FName Socket = FName("SocketName"), FLinearColor LineColor = FLinearColor(0, 1, 0, 1), float Thickness = 7, float Duration = -1.f);
	/** Draw 3D Line of Chosen Thickness Between Two Actors. Duration is in Seconds */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|3d Lines")
		static void Draw__Thick3DLineBetweenActors(AActor* StartActor, AActor* EndActor, FLinearColor LineColor = FLinearColor(0, 0, 1, 1), float Thickness = 7, float Duration = -1.f);

	/** AnimBPOwner - Must be a Character, Conversion Internally For Convenience.\n\nRetrieves the Aim Offsets Pitch & Yaw Based On the Rotation of the Controller of The Character Owning The Anim Instance.\n\nThe Pitch and Yaw are meant to be used with a Blend Space going from -90,-90 to 90,90.\n   Returns true if function filled the pitch and yaw vars successfully */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Aim Offset")
		static bool Animation__GetAimOffsets(AActor* AnimBPOwner, float& Pitch, float& Yaw);

	/** AnimBPOwner - Must be a Character, Conversion Internally For Convenience.\n\nRetrieves the Aim Offsets Pitch & Yaw for the AnimBPOwner Based On the supplied Rotation.\n\nThe Pitch and Yaw are meant to be used with a Blend Space going from -90,-90 to 90,90.\n    Returns true if function filled the pitch and yaw vars successfully */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Aim Offset")
		static bool Animation__GetAimOffsetsFromRotation(AActor * AnimBPOwner, const FRotator & TheRotation, float & Pitch, float & Yaw);

	/** Saves text to filename of your choosing, make sure include whichever file extension you want in the filename, ex: SelfNotes.txt . Make sure to include the entire file path in the save directory, ex: C:\MyGameDir\BPSavedTextFiles */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|File IO")
	static bool FileIO__SaveStringTextToFile(FString SaveDirectory, FString JoyfulFileName, FString SaveText, bool AllowOverWriting = false);

	/** Saves multiple Strings to filename of your choosing, with each string on its own line! Make sure include whichever file extension you want in the filename, ex: SelfNotes.txt . Make sure to include the entire file path in the save directory, ex: C:\MyGameDir\BPSavedTextFiles */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|File IO")
	static bool FileIO__SaveStringArrayToFile(FString SaveDirectory, FString JoyfulFileName, TArray<FString> SaveText, bool AllowOverWriting = false);


	/** Obtain an Array of Actors Rendered Recently. You can specifiy what qualifies as "Recent" in seconds. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor", meta=(WorldContext="WorldContextObject"))
		static void Visibility__GetRenderedActors(UObject* WorldContextObject, TArray<AActor*>& CurrentlyRenderedActors, float MinRecentTime = 0.01);

	/** Obtain an Array of Actors NOT Rendered Recently! You can specifiy what qualifies as "Recent" in seconds. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor", meta=(WorldContext="WorldContextObject"))
		static void Visibility__GetNotRenderedActors(UObject* WorldContextObject, TArray<AActor*>& CurrentlyNotRenderedActors, float MinRecentTime = 0.01);

	/** Is the Current Game Window the Foreground window in the OS, or in the Editor? This will be accurate in standalone running of the game as well as in the editor PIE */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Game Window")
	static bool ClientWindow__GameWindowIsForeGroundInOS();

	/** Flashes the game on the windows OS task bar! Please note this won't look the best in PIE, flashing is smoother in Standalone or packaged game. You can use GameWindowIsForeGroundInOS to see if there is a need to get the user's attention! <3 Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Game Window")
	static void FlashGameOnTaskBar(APlayerController* PC, bool FlashContinuous=false, int32 MaxFlashCount = 3, int32 FlashFrequencyMilliseconds=500);

	/** Freeze Game Render, Does Not Stop Game Logic, Just Rendering. This is not like Pausing. Mainly useful for freezing render when window is not in the foreground */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Game Window")
		static void Rendering__FreezeGameRendering();

	/** Unfreeze Game Render. This is not an unpause function, it's just for actual screen rendering */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Game Window")
	static void Rendering__UnFreezeGameRendering();

	/** Compare Source Vector against Array of Vectors. Returns: Returns the Closest Vector to Source and what that closest Distance is, or -1 if there was an error such as array being empty. Ignores: Ignores Source if source is in the array */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
	static float Calcs__ClosestPointToSourcePoint(const FVector & Source, const TArray<FVector>& OtherPoints, FVector& ClosestPoint);

	/** Takes in an actor (for convenience) and tries to cast it to Character and return an array of Vectors of all of the current bone locations of the character's Mesh. Locations are in World Space. Returns: false if the operation could not occur. Requires: Character Mesh Component must be valid */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Actor")
	static bool Data__GetCharacterBoneLocations(AActor * TheCharacter, TArray<FVector>& BoneLocations);

	/** Retrieves the "Mesh" component of a Character. IsValid lets you know if the data is valid, make sure to check if it is! Requires: the passed in Actor must be a Character */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Character")
	static USkeletalMeshComponent* Accessor__GetCharacterSkeletalMesh(AActor* TheCharacter, bool& IsValid);


	/**
	 * Get All Bone Names Below Bone, requires a physics asset, by Rama
	 *
	 * @param StartingBoneName The name of the bone to find all bones below.
	 *
	 * @param BoneNames , all of the bone names below the starting bone.
	 *
	 * @return total number of bones found
	 */
	UFUNCTION(BlueprintCallable, Category="Victory BP Library|Components|SkinnedMesh")
	static int32 GetAllBoneNamesBelowBone(USkeletalMeshComponent* SkeletalMeshComp, FName StartingBoneName,  TArray<FName>& BoneNames );

	/** Does Not Do A Trace, But Obtains the Start and End for doing a Trace:\n\nTakes in an actor (for convenience) and tries to cast it to Character. Takes in a socket name to find on the Character's Mesh component, the socket location will be the start of the trace.\n\nAlso takes in the Angle / Rotator and the length of the trace you want to do. Option to draw the trace with variable thickness as it occurs.\n\nReturns what the Trace Start and End should be so you can plug these into any existing trace node you want.\n\nRequires: Character Mesh Component must be valid. Returns False if trace could not be done */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Traces")
	static bool TraceData__GetTraceDataFromCharacterSocket(FVector& TraceStart, FVector& TraceEnd, AActor * TheCharacter, const FRotator& TraceRotation, float TraceLength = 10240, FName Socket = "SocketName", bool DrawTraceData = true, FLinearColor TraceDataColor = FLinearColor(1, 0, 0, 1), float TraceDataThickness = 7);

	/** Does Not Do A Trace, But Obtains the Start and End for doing a Trace:\n\nTakes in a Skeletal Mesh Component and a socket name to trace from. Also takes in the Angle / Rotator and the length of the trace you want to do.\n\nOption to draw the trace as a variable thickness line\n\nReturns what the Trace Start and End should be so you can plug these into any existing trace node you want.\n\n Requires: Mesh must be valid. Returns False if trace could not be done */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Traces")
		static bool TraceData__GetTraceDataFromSkeletalMeshSocket(FVector& TraceStart, FVector& TraceEnd, USkeletalMeshComponent* Mesh, const FRotator& TraceRotation, float TraceLength = 10240, FName Socket = "SocketName", bool DrawTraceData = true, FLinearColor TraceDataColor = FLinearColor(1, 0, 0, 1), float TraceDataThickness = 7);

	/** Does a simple line trace given Trace Start and End, and if a Character is hit by the trace, then a component trace is performed on that character's mesh. Trace Owner is ignored when doing the trace.\n\nReturns the Character that was hit, as an Actor, as well as the name of the bone that was closest to the impact point of the trace. Also returns the impact point itself as well as the impact normal.\n\nUsing component trace ensures accuracy for testing against bones and sockets.\n\nIsValid: Will be true only if the component trace also hit someting. But the Returned Actor will contain an actor if any actor at all was hit by the simple trace. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Traces")
		static AActor* Traces__CharacterMeshTrace___ClosestBone(AActor* TraceOwner, const FVector& TraceStart, const FVector& TraceEnd, FVector& OutImpactPoint, FVector& OutImpactNormal, FName& ClosestBoneName, FVector & ClosestBoneLocation, bool&IsValid);

	/** Does a simple line trace given Trace Start and End, and if a Character is hit by the trace, then a component trace is performed on that character's mesh. Returns the name of the socket that was closest to the impact point of the trace. Also returns the impact point itself as well as the impact normal. Also returns the Socket Location. Using component trace ensures accuracy for testing against bones and sockets.*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Traces",meta=(WorldContext="WorldContextObject"))
		static AActor* Traces__CharacterMeshTrace___ClosestSocket(UObject* WorldContextObject, const AActor * TraceOwner, const FVector& TraceStart, const FVector& TraceEnd, FVector& OutImpactPoint, FVector& OutImpactNormal, FName& ClosestSocketName, FVector & SocketLocation, bool&IsValid);

	/** Returns the float as a String with Precision, Precision 0 = no decimal value. Precison 1 = 1 decimal place. The re-precisioned result is rounded appropriately. */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Conversion")
	static void StringConversion__GetFloatAsStringWithPrecision(float TheFloat, FString & TheString, int32 Precision = 2, bool IncludeLeadingZero=true);

	/** Rotator out value is the degrees of difference between the player camera and the direction of player to light source. Returns false if the operation could not occur. */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
		static bool LensFlare__GetLensFlareOffsets(APlayerController* PlayerController, AActor* LightSource, float& PitchOffset, float& YawOffset, float& RollOffset);
	
	/** Retrieve Distance of given point to any Surface point on a Static Mesh Actor. Returns the distance as well as the exact closest point on the mesh surface to the given point. Returns -1 if an error occurred*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
		static float DistanceToSurface__DistaceOfPointToMeshSurface(AStaticMeshActor* TheSMA, const FVector& TestPoint, FVector& ClosestSurfacePoint);

	/** Change the Mobility of a Static Mesh Component, can be used in Constructor Script or in Event Graph! Returns false if operation could not occur.*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Misc")
		static bool Mobility__SetSceneCompMobility(USceneComponent* SceneComp, EComponentMobility::Type NewMobility);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			  			Paths

	/** InstallDir/GameName/Binaries/Win64 */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__Win64Dir_BinaryLocation();

	/** InstallDir/WindowsNoEditor/ */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__WindowsNoEditorDir();

	/** InstallDir/GameName */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__GameRootDirectory();

	/** InstallDir/GameName/Saved */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__SavedDir();

	/** InstallDir/GameName/Saved/Config/ */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__ConfigDir();

	/** InstallDir/GameName/Saved/Screenshots/Windows */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__ScreenShotsDir();

	/** InstallDir/GameName/Saved/Logs */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Paths")
	static FString VictoryPaths__LogsDir();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			  Graphics Settings Sample

	//~~~~~~~~~~~~~~~~~~
	//		FullScreen
	//~~~~~~~~~~~~~~~~~~

	/** Get Full Screen Type */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Graphics Settings")
	static TEnumAsByte<EJoyGraphicsFullScreen::Type> JoyGraphicsSettings__FullScreen_Get();

	/** Set Full Screen Type */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Graphics Settings")
		static void JoyGraphicsSettings__FullScreen_Set(TEnumAsByte<EJoyGraphicsFullScreen::Type> NewSetting);



	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			  CPP FUNCTION LIBRARY

	static FORCEINLINE void JoyCC(const FString& Str, const int32 Value)
	{
		TObjectIterator<APlayerController> PC;
		if (!PC) return;
		//~~~~~~

		PC->ConsoleCommand(Str + " " + FString::FromInt(Value));
	}
	static FORCEINLINE void JoyGraphics_FullScreen_SetFullScreenType(int32 Value)
	{
		JoyCC("r.FullScreenMode", Value);
	}
	static FORCEINLINE int32 JoyGraphics_FullScreen_GetFullScreenType()
	{
		static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FullScreenMode"));
		return CVar->GetValueOnGameThread();
	}
	static FORCEINLINE void JoyGraphics_SetFullScreen_NonWindowed()
	{
		/*"r.FullScreenMode"),
			2,
			TEXT("Defines how we do full screen when requested (e.g. command line option -fullscreen or in ini [SystemSettings] fullscreen=true)\n")
			*/
		JoyCC("r.FullScreenMode", 0);
	}
	static FORCEINLINE void JoyGraphics_SetFullScreen_Windowed()
	{
		//TEXT(" 2: windowed full screen, specified resolution (like 1 but no unintuitive performance cliff, can be blurry, default)\n")
		JoyCC("r.FullScreenMode", 2);
	}
	static FORCEINLINE void JoyGraphics_SetFullScreen_WindowedHighestQuality()
	{
		//TEXT(" 1: windowed full screen, desktop resolution (quick switch between applications and window mode, full quality)\n")
		JoyCC("r.FullScreenMode", 1);
	}



	//Min and Max of Array
	static FORCEINLINE float Min(const TArray<float>& Values, int32* MinIndex = NULL)
	{
		if (MinIndex)
		{
			*MinIndex = 0;
		}

		if (Values.Num() <= 0)
		{
			return -1;
		}

		float CurMin = Values[0];
		for (const float EachValue : Values)
		{
			CurMin = FMath::Min(CurMin, EachValue);
		}

		if (MinIndex)
		{
			*MinIndex = Values.Find(CurMin);
		}
		return CurMin;
	}
	static FORCEINLINE float Max(const TArray<float>& Values, int32* MaxIndex = NULL)
	{
		if (MaxIndex)
		{
			*MaxIndex = 0;
		}

		if (Values.Num() <= 0)
		{
			return -1;
		}

		float CurMax = Values[0];
		for (const float EachValue : Values)
		{
			CurMax = FMath::Max(CurMax, EachValue);
		}

		if (MaxIndex)
		{
			*MaxIndex = Values.Find(CurMax);
		}
		return CurMax;
	}

	static FORCEINLINE int32 Min(const TArray<int32>& Values, int32* MinIndex = NULL)
	{
		if (MinIndex)
		{
			*MinIndex = 0;
		}

		if (Values.Num() <= 0)
		{
			return -1;
		}

		int32 CurMin = Values[0];
		for (const int32 EachValue : Values)
		{
			CurMin = FMath::Min(CurMin, EachValue);
		}

		if (MinIndex)
		{
			*MinIndex = Values.Find(CurMin);
		}
		return CurMin;
	}
	static FORCEINLINE int32 Max(const TArray<int32>& Values, int32* MaxIndex = NULL)
	{
		if (MaxIndex)
		{
			*MaxIndex = 0;
		}

		if (Values.Num() <= 0)
		{
			return -1;
		}

		int32 CurMax = Values[0];
		for (const int32 EachValue : Values)
		{
			CurMax = FMath::Max(CurMax, EachValue);
		}

		if (MaxIndex)
		{
			*MaxIndex = Values.Find(CurMax);
		}
		return CurMax;
	}



	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//			  Contributed by Others

	/**
	*
	* Contributed by: Mindfane
	*
	* Split a string into an array of substrings based on the given delimitter.
	* Unlike ParseIntoArray() function which expects single character delimitters, this function can accept a delimitter that is also a string.
	*
	* @param InputString - The string that is to be exploded.
	* @param Separator - The delimitter that is used for splitting (multi character strings are allowed)
	* @param limit - If greater than zero, returns only the first x strings. Otherwsie returns all the substrings
	* @param bTrimElelements - If True, then each subsctring is processed and any leading or trailing whitespcaes are trimmed.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Victory BP Library|String", Keywords = "split explode string"), Category = String)
	static void String__ExplodeString(TArray<FString>& OutputStrings, FString InputString, FString Separator = ",", int32 limit = 0, bool bTrimElements = false);


	//NOT QUITE WORKING, REQUIRES INVESTIGATION
	/** Load a Texture 2D from a DDS file! Contributed by UE4 forum member n00854180t! */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="image DDS"))
	static UTexture2D* LoadTexture2D_FromDDSFile(const FString& FullFilePath);

	/** Load a Texture2D from a JPG,PNG,BMP,ICO,EXR,ICNS file! IsValid tells you if file path was valid or not. Enjoy! -Rama */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="image png jpg jpeg bmp bitmap ico icon exr icns"))
	static UTexture2D* Victory_LoadTexture2D_FromFile(const FString& FullFilePath,EJoyImageFormats ImageFormat,bool& IsValid, int32& Width, int32& Height);

	/** Load a Texture2D from a JPG,PNG,BMP,ICO,EXR,ICNS file! IsValid tells you if file path was valid or not. Enjoy! -Rama */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="image png jpg jpeg bmp bitmap ico icon exr icns"))
	static UTexture2D* Victory_LoadTexture2D_FromFile_Pixels(const FString& FullFilePath,EJoyImageFormats ImageFormat,bool& IsValid, int32& Width, int32& Height, TArray<FLinearColor>& OutPixels);

	/** Retrieve a pixel color value given the pixel array, the image height, and the coordinates. Returns false if the coordinates were not valid. Pixel coordinates start from upper left corner as 0,0. X= horizontal, Y = vertical */
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="image coordinate index map value"))
	static bool Victory_Get_Pixel(const TArray<FLinearColor>& Pixels, int32 ImageHeight, int32 x, int32 y, FLinearColor& FoundColor);

	/** Save an array of pixels to disk as a PNG! It is very important that you supply the curret width and height of the image! Returns false if Width * Height != Array length or file could not be saved to the location specified. I return an ErrorString to clarify what the exact issue was. -Rama */
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="create image png jpg jpeg bmp bitmap ico icon exr icns"))
	static bool Victory_SavePixels(const FString& FullFilePath,int32 Width, int32 Height, const TArray<FLinearColor>& ImagePixels, bool SaveAsBMP, bool sRGB, FString& ErrorString);

	/** This will modify the original T2D to remove sRGB and change compression to VectorDisplacementMap to ensure accurate pixel reading. -Rama*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="create image png jpg jpeg bmp bitmap ico icon exr icns"))
	static bool Victory_GetPixelFromT2D(UTexture2D* T2D, int32 X, int32 Y, FLinearColor& PixelColor);

	/** This will modify the original T2D to remove sRGB and change compression to VectorDisplacementMap to ensure accurate pixel reading. -Rama*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="create image png jpg jpeg bmp bitmap ico icon exr icns"))
	static bool Victory_GetPixelsArrayFromT2D(UTexture2D* T2D, int32& TextureWidth, int32& TextureHeight,TArray<FLinearColor>& PixelArray);

	/** This will modify the original T2D to remove sRGB and change compression to VectorDisplacementMap to ensure accurate pixel reading. -Rama*/
	//UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Load Texture From File",meta=(Keywords="create image png jpg jpeg bmp bitmap ico icon exr icns"))
	static bool Victory_GetPixelsArrayFromT2DDynamic(UTexture2DDynamic* T2D, int32& TextureWidth, int32& TextureHeight,TArray<FLinearColor>& PixelArray);

	/** Contributed by UE4 forum member n00854180t! Plays a .ogg sound from file, attached to and following the specified component. This is a fire and forget sound. Replication is also not handled at this point.
	* @param FilePath - Path to sound file to play
	* @param AttachComponent - Component to attach to.
	* @param AttachPointName - Optional named point within the AttachComponent to play the sound at
	* @param Location - Depending on the value of Location Type this is either a relative offset from the attach component/point or an absolute world position that will be translated to a relative offset
	* @param LocationType - Specifies whether Location is a relative offset or an absolute world position
	* @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	* @param VolumeMultiplier - Volume multiplier
	* @param PitchMultiplier - PitchMultiplier
	* @param AttenuationSettings - Override attenuation settings package to play sound with
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Sound", meta = (VolumeMultiplier = "1.0", PitchMultiplier = "1.0", AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true"))
	static class UAudioComponent* PlaySoundAttachedFromFile(const FString& FilePath, class USceneComponent* AttachToComponent, FName AttachPointName = NAME_None, FVector Location = FVector(ForceInit), EAttachLocation::Type LocationType = EAttachLocation::SnapToTarget, bool bStopWhenAttachedToDestroyed = false, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL);

	/** Contributed by UE4 forum member n00854180t! Plays a .ogg sound at the given location. This is a fire and forget sound and does not travel with any actor. Replication is also not handled at this point.
	*
	* NOT SUPPORTED ON PS4.
	*
	* @param FilePath - Path to sound file to play
	* @param Location - World position to play sound at
	* @param World - The World in which the sound is to be played
	* @param VolumeMultiplier - Volume multiplier
	* @param PitchMultiplier - PitchMultiplier
	* @param AttenuationSettings - Override attenuation settings package to play sound with
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Sound", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", VolumeMultiplier = "1.0", PitchMultiplier = "1.0", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true"))
	static void PlaySoundAtLocationFromFile(UObject* WorldContextObject, const FString& FilePath, FVector Location, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = NULL);

	/** Contributed by UE4 forum member n00854180t! Creates a USoundWave* from file path.
	* Read .ogg header file and refresh USoundWave metadata.
	*
	* NOT SUPPORTED ON PS4.
	*
	* @param FilePath		path to file to create sound wave from
	*/
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Sound")
	static class USoundWave* GetSoundWaveFromFile(const FString& FilePath);

#if !PLATFORM_PS4
private:
	// Thanks to @keru for the base code for loading an Ogg into a USoundWave:
	// https://forums.unrealengine.com/showthread.php?7936-Custom-Music-Player&p=97659&viewfull=1#post97659

	     /**
        * Read .ogg header file and refresh USoundWave metadata. NOT SUPPORTED BY PS4
        * @param sw             wave to put metadata
        * @param rawFile        pointer to src file in memory
        * @return 0     if everything is ok
        *                 1 if couldn't read metadata.
        */
        static int32 fillSoundWaveInfo(USoundWave* sw, TArray<uint8>* rawFile);



        /**
        * Tries to find out FSoundSource object associated to the USoundWave. NOT SUPPORTED BY PS4
        * @param sw     wave, search key
        * @return 0 if wave found and correctly set
        *        -1 if error: sound device not set
        *        -2 if error: sound wave not found
        */
		static int32 findSource(class USoundWave* sw, class FSoundSource* out_audioSource);
#endif //PLATFORM_PS4



		/**
		* Contributed by: SaxonRah
		* Better random numbers. Seeded with a random device. if the random device's entropy is 0; defaults to current time for seed.
		* can override with seed functions;
		*/
//----------------------------------------------------------------------------------------------BeginRANDOM
public:
		/** Construct a random device, returns either a random device or the default random engine; system dependant;
		*/
		UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Random")
			static void constructRand();

		/** Seed Rand with value passed
		* @param seed - value to pass to the prng as the seed
		*/
		UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Random")
			static void seedRand(int32 seed);

		/** Seed Rand with current time
		*/
		UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Random")
			static void seedRandWithTime();

		/** Seed Rand with entropy
		* @param seed - value to pass to the prng as the seed
		*/
		UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Random")
			static void seedRandWithEntropy();

		/** Random Bool - Bernoulli distribution
		* @param fBias - Bias of Bernoulli distribution
		* @return uniformly distributed bool based on bias parameter
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static bool RandBool_Bernoulli(float fBias);

		/** Random Integer - Zero to One Uniform distribution
		* @return int32 - uniform distribution from 0 to 1
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static int32 RandInt_uniDis();

		/** Random Integer - MIN to MAX Uniform distribution
		* @param iMin - Minimum value of uniform distribution
		* @param iMax - Maximum value of uniform distribution
		* @return int32 - uniform distribution from iMin to iMax parameters
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static int32 RandInt_MINMAX_uniDis(int32 iMin, int32 iMax);

		/** Random Double - Zero to One Uniform distribution
		* @return double - uniform distribution from 0 to 1
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static float RandFloat_uniDis();

		/** Random Double - Uniform distribution based on MIN to MAX parameters
		* @param iMin - Minimum value of uniform distribution
		* @param iMax - Maximum value of uniform distribution
		* @return double - uniform distribution from iMin to iMax parameters
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static float RandFloat_MINMAX_uniDis(float iMin, float iMax);

		/** Random Bool - Bernoulli distribution - Mersenne Twister
		* @param fBias - Bias of Bernoulli distribution
		* @return uniformly distributed bool based on bias parameter
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static bool RandBool_Bernoulli_MT(float fBias);

		/** Random Integer - Zero to One Uniform distribution - Mersenne Twister
		* @return int32 - uniform distribution from 0 to 1
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static int32 RandInt_uniDis_MT();

		/** Random Integer - MIN to MAX Uniform distribution - Mersenne Twister
		* @param iMin - Minimum value of uniform distribution
		* @param iMax - Maximum value of uniform distribution
		* @return int32 - uniform distribution from iMin to iMax parameters
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static int32 RandInt_MINMAX_uniDis_MT(int32 iMin, int32 iMax);

		/** Random Float - Zero to One Uniform distribution -  Mersenne Twister
		* @return float - uniform distribution from 0 to 1
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static float RandFloat_uniDis_MT();

		/** Random Float - Uniform distribution based on MIN to MAX parameters - Mersenne Twister
		* @param iMin - Minimum value of uniform distribution
		* @param iMax - Maximum value of uniform distribution
		* @return float - uniform distribution from iMin to iMax parameters
		*/
		UFUNCTION(BlueprintPure, Category = "Victory BP Library|Random")
			static float RandFloat_MINMAX_uniDis_MT(float iMin, float iMax);
//----------------------------------------------------------------------------------------------ENDRANDOM

/** Inspired by Sahkan! */
UFUNCTION(BlueprintPure, Category = "Victory BP Library|Actor|Get Immediate Attached Actors")
static void Actor__GetAttachedActors(AActor* ParentActor,TArray<AActor*>& ActorsArray);

/** Modify the bloom intensity of a post process volume, by Community Member Sahkan */
UFUNCTION(BlueprintCallable, Category = "Victory BP Library|Post Process")
static void SetBloomIntensity(APostProcessVolume* PostProcessVolume,float Intensity);




//~~~ Kris ~~~

	/*
	 *See if index is a valid index for this array
	 *
	 *@param    TargetArray        The array to perform the operation on
	 *@param    Index            The index to check.
	 *@return    Bool if integer is valid index for this array
	*/
	UFUNCTION(Category="Victory BP Library|Utilities|Array", BlueprintPure, CustomThunk, meta=(DisplayName = "Valid Index", CompactNodeTitle = "VALID INDEX", ArrayParm = "TargetArray"))
	static bool Array_IsValidIndex(const TArray<int32>& TargetArray, int32 Index);

	static bool GenericArray_IsValidIndex(void* TargetArray, const UArrayProperty* ArrayProp, int32 Index);

	DECLARE_FUNCTION(execArray_IsValidIndex)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<UArrayProperty>(NULL);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		P_GET_PROPERTY(UIntProperty, Index);
		P_FINISH;

		bool WasValid = GenericArray_IsValidIndex(ArrayAddr, ArrayProperty, Index);
		*(bool*)RESULT_PARAM = WasValid;
	}

	/** Get the time target actor was created. */
	UFUNCTION(Category = "Victory BP Library|Actor", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static float GetCreationTime(const AActor* Target);

	/** Get the time target actor has been alive. */
	UFUNCTION(Category = "Victory BP Library|Actor", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static float GetTimeAlive(const AActor* Target);

	/** Contributed by Community Member Kris! */
	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure)
	static bool CaptureComponent2D_Project(class USceneCaptureComponent2D* Target, FVector Location, FVector2D& OutPixelLocation);

	/** Contributed by Community Member Kris! */
	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintPure, Meta = (DefaultToSelf = "Target"))
	static bool Capture2D_Project(class ASceneCapture2D* Target, FVector Location, FVector2D& OutPixelLocation);

	/** Currently the only supported format for this function is B8G8R8A8. Make sure to include the appropriate image extension in your file path! Recommended: .bmp, .jpg, .png. Contributed by Community Member Kris! */
	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintCallable)
	static bool CaptureComponent2D_SaveImage(class USceneCaptureComponent2D* Target, const FString ImagePath, const FLinearColor ClearColour);

	/** Currently the only supported format for this function is B8G8R8A8. Make sure to include the appropriate image extension in your file path! Recommended: .bmp, .jpg, .png. Contributed by Community Member Kris! */
	UFUNCTION(Category = "Victory BP Library|SceneCapture", BlueprintCallable, Meta = (DefaultToSelf = "Target"))
	static bool Capture2D_SaveImage(class ASceneCapture2D* Target, const FString ImagePath, const FLinearColor ClearColour);

	/** Make sure your image path has a valid extension! Supported types can be seen in the BP node Victory_LoadTexture2D_FromFile. Contributed by Community Member Kris! */
	UFUNCTION(Category = "Victory BP Library|Load Texture From File", BlueprintCallable)
	static UTexture2D*  LoadTexture2D_FromFileByExtension(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);

	/**
	 * Find first widget of a certain class and return it.
	 * @param WidgetClass The widget class to filter by.
	 * @param TopLevelOnly Only a widget that is a direct child of the viewport will be returned.
	 */
	UFUNCTION(Category = "Victory BP Library|UMG", BlueprintCallable, BlueprintCosmetic, Meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "WidgetClass"))
	static UUserWidget* GetFirstWidgetOfClass(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, bool TopLevelOnly);

	/**
	 * Recurses up the list of parents and returns true if this widget is a descendant of the PossibleParent
	 * @return true if this widget is a child of the PossibleParent
	 */
	UFUNCTION(Category = "Victory BP Library|UMG", BlueprintCallable, BlueprintCosmetic, Meta = (DefaultToSelf = "ChildWidget"))
	static bool WidgetIsChildOf(UWidget* ChildWidget, UWidget* PossibleParent);

	/**
	 * Recurses up the list of parents until it finds a widget of WidgetClass.
	 * @return widget that is Parent of ChildWidget that matches WidgetClass.
	 */
	UFUNCTION(Category = "Victory BP Library|UMG", BlueprintCallable, BlueprintCosmetic, Meta = (DefaultToSelf = "ChildWidget", DeterminesOutputType = "WidgetClass"))
	static UUserWidget* WidgetGetParentOfClass(UWidget* ChildWidget, TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(Category = "Victory BP Library|UMG", BlueprintCallable, BlueprintCosmetic, Meta = (DefaultToSelf = "ParentWidget", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "ChildWidgets"))
	static void WidgetGetChildrenOfClass(UWidget* ParentWidget, TArray<UUserWidget*>& ChildWidgets, TSubclassOf<UUserWidget> WidgetClass, bool bImmediateOnly);

	UFUNCTION(Category = "Victory BP Library|UMG", BlueprintCallable, BlueprintCosmetic, Meta = (DefaultToSelf = "ParentUserWidget"))
	static UWidget* GetWidgetFromName(UUserWidget* ParentUserWidget, const FName& Name);

	UFUNCTION(Category = "Victory BP Library|Team", BlueprintPure)
	static uint8 GetGenericTeamId(AActor* Target);

	UFUNCTION(Category = "Victory BP Library|Team", BlueprintCallable)
	static void SetGenericTeamId(AActor* Target, uint8 NewTeamId);

	UFUNCTION(Category = "Victory BP Library|LevelStreaming", BlueprintCallable)
	static FLevelStreamInstanceInfo GetLevelInstanceInfo(ULevelStreamingKismet* LevelInstance);

	UFUNCTION(Category = "Victory BP Library|LevelStreaming", BlueprintCallable, Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void AddToStreamingLevels(UObject* WorldContextObject, const FLevelStreamInstanceInfo& LevelInstanceInfo);

	UFUNCTION(Category = "Victory BP Library|LevelStreaming", BlueprintCallable, Meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void RemoveFromStreamingLevels(UObject* WorldContextObject, const FLevelStreamInstanceInfo& LevelInstanceInfo);

	UFUNCTION(Category = "Victory BP Library|LevelStreaming", BlueprintCallable, Meta = (keywords="remove"))
	static void HideStreamingLevel(ULevelStreamingKismet* LevelInstance)
	{
		if(LevelInstance) LevelInstance->bShouldBeVisible = false;
	}

	UFUNCTION(Category = "Victory BP Library|LevelStreaming", BlueprintCallable, Meta = (keywords="remove"))
	static void UnloadStreamingLevel(ULevelStreamingKismet* LevelInstance)
	{
		if(LevelInstance) LevelInstance->bShouldBeLoaded = false;
	}

	static bool GenericArray_SortCompare(const UProperty* LeftProperty, void* LeftValuePtr, const UProperty* RightProperty, void* RightValuePtr);

	/**
	 *	Sort the elements of an array by FString, FName, FText, float, int or boolean.
	 *	Supports struct and object based arrays.
	 *
	 *	@param	TargetArray		The array to sort.
	 *	@param	bAscendingOrder	If true, sort by ascending order.
	 *	@param	VariableName	If a struct or object based array, the name of the variable to sort by.
	 */
	UFUNCTION(Category = "Victory BP Library|Utilities|Array", BlueprintCallable, CustomThunk, Meta = (DisplayName = "Sort", ArrayParm = "TargetArray", AdvancedDisplay = "bAscendingOrder,VariableName"))
	static void Array_Sort(const TArray<int32>& TargetArray, bool bAscendingOrder = true, FName VariableName = NAME_None);

	static void GenericArray_Sort(void* TargetArray, const UArrayProperty* ArrayProp, bool bAscendingOrder = true, FName VariableName = NAME_None);

	DECLARE_FUNCTION(execArray_Sort)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<UArrayProperty>(NULL);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_GET_UBOOL(bAscendingOrder);

		P_GET_PROPERTY(UNameProperty, VariableName);

		P_FINISH;
		P_NATIVE_BEGIN;
		GenericArray_Sort(ArrayAddr, ArrayProperty, bAscendingOrder, VariableName);
		P_NATIVE_END;
	}

	/**
	 *	Calls PrestreamTextures() for all the actor's meshcomponents.
	 *	PrestreamTextures() tells the streaming system to start loading all textures with all mip-levels.
	 *	@param Seconds - Number of seconds to force all mip-levels to be resident
	 *	@param bEnableStreaming	- Whether to start (true) or stop (false) streaming
	 *	@param CinematicTextureGroups - Bitfield indicating which texture groups that use extra high-resolution mips
	 */
	UFUNCTION(Category = "Victory BP Library|Actor", BlueprintCallable, Meta = (DisplayName = "PrestreamTextures (Actor)"))
	static void Actor_PrestreamTextures(AActor* Target, float Seconds = 1.0f, bool bEnableStreaming = true, int32 CinematicTextureGroups = 0);

	/**
	 *	Tells the streaming system to start loading all textures with all mip-levels.
	 *	@param Seconds - Number of seconds to force all mip-levels to be resident
	 *	@param bEnableStreaming	- Whether to start (true) or stop (false) streaming
	 *	@param CinematicTextureGroups - Bitfield indicating which texture groups that use extra high-resolution mips
	 */
	UFUNCTION(Category = "Victory BP Library|Components", BlueprintCallable, Meta = (DisplayName = "PrestreamTextures (Component)"))
	static void Component_PrestreamTextures(UMeshComponent* Target, float Seconds = 1.0f, bool bEnableStreaming = true, int32 CinematicTextureGroups = 0);

	/**
	 * Converts the screen position (primary screen's top left corner) supplied by pointer events or similar
	 * to the local space of viewport related to WorldContextObject.
	 *
	 * @param WorldContextObject	World context.
	 * @param ScreenPosition		Coordinates from FPointerEvent GetScreenSpacePosition() or similar.
	 * @param OutViewportPosition	Coordinates based on the local viewport (fullscreen or otherwise).
	 *
	 * @return True if OutViewportPosition is not 0,0.
	 */
	UFUNCTION(Category = "Victory BP Library|Game|Viewport", BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static bool GetViewportPosition(UObject* WorldContextObject, const FVector2D& ScreenPosition, FVector2D& OutViewportPosition);

	/**
	 * Does a line collision trace based the viewport position and returns the first blocking hit encountered.
	 * This trace finds the objects that RESPOND to the given TraceChannel
	 *
	 * @param WorldContextObject	World context.
	 * @param ViewportPosition		Local space of viewport from GetViewportPosition() or similar.
	 * @param TraceChannel
	 * @param bTraceComplex			True to test against complex collision, false to test against simplified collision.
	 * @param OutHitResult			Properties of the trace hit.
	 *
	 * @return True if there was a hit, false otherwise.
	 */
	UFUNCTION(Category = "Victory BP Library|Game|Viewport", BlueprintCallable, Meta = (WorldContext="WorldContextObject", bTraceComplex = true, TraceChannel = ECC_Visibility))
	static bool GetViewportPositionHitResultByChannel(UObject* WorldContextObject, const FVector2D& ViewportPosition, ECollisionChannel TraceChannel, bool bTraceComplex, FHitResult& OutHitResult);

	/**
	 * Transforms the viewport position into a world space origin and direction.
	 *
	 * @param WorldContextObject	World context.
	 * @param ViewportPosition		Local space of viewport from GetViewportPosition() or similar.
	 * @param OutWorldOrigin		Corresponding 3D location in world space.
	 * @param OutWorldDirection		World space direction vector away from the camera at the given 2d point.
	 *
	 * @return false if something went wrong during the deproject process.
	 */
	UFUNCTION(Category = "Victory BP Library|Game|Viewport", BlueprintCallable, Meta = (WorldContext="WorldContextObject"))
	static bool ViewportPositionDeproject(UObject* WorldContextObject, const FVector2D& ViewportPosition, FVector& OutWorldOrigin, FVector& OutWorldDirection);

	/**
	 *	Inserts child widget into panel widget at given location.
	 *	NOTE: The child widgets "Construct" event will be fired again!
	 *
	 *	@param	Parent	- The panel to insert the child into.
	 *	@param	Index	- Where to insert the new widget.
	 *	@param	Content - The child widget to insert.
	 *	@return slot assigned to content.
	 */
	UFUNCTION(Category = "Victory BP Library|Widget|Panel", BlueprintCallable)
	static class UPanelSlot* InsertChildAt(class UWidget* Parent, int32 Index, UWidget* Content);

	/** Flushes the current key state for target player controller. */
	UFUNCTION(Category = "Victory BP Library|Input", BlueprintCallable)
	static void FlushPressedKeys(class APlayerController* PlayerController);

	UFUNCTION(Category = "Victory BP Library|Vector", BlueprintPure)
	static FVector GetVectorRelativeLocation(FVector ParentLocation, FRotator ParentRotation, FVector ChildLocation);

	UFUNCTION(Category = "Victory BP Library|Components", BlueprintPure, Meta = (DefaultToSelf = "ChildComponent"))
	static FVector GetComponentRelativeLocation(class USceneComponent* ParentComponent, class USceneComponent* ChildComponent);

	UFUNCTION(Category = "Victory BP Library|Actor", BlueprintPure, Meta = (DefaultToSelf = "ChildActor"))
	static FVector GetActorRelativeLocation(class AActor* ParentActor, class AActor* ChildActor);

	UFUNCTION(Category = "Victory BP Library|Rotator", BlueprintPure)
	static FRotator GetRotatorRelativeRotation(FRotator ParentRotation, FRotator ChildRotation);

	UFUNCTION(Category = "Victory BP Library|Components", BlueprintPure, Meta = (DefaultToSelf = "ChildComponent"))
	static FRotator GetComponentRelativeRotation(class USceneComponent* ParentComponent, class USceneComponent* ChildComponent);

	UFUNCTION(Category = "Victory BP Library|Actor", BlueprintPure, Meta = (DefaultToSelf = "ChildActor"))
	static FRotator GetActorRelativeRotation(class AActor* ParentActor, class AActor* ChildActor);

	/**
	 *	Helper function to calculate vertical FOV from horizontal FOV and aspect ratio.
	 *	Useful to for determining distance from camera fit in-game objects to the width of the screen.
	 */
	UFUNCTION(Category = "Victory BP Library|Game|Viewport", BlueprintPure)
	static float HorizontalFOV(float VerticalFOV, float AspectRatio);

	/**
	 *	Helper function to calculate vertical FOV from horizontal FOV and aspect ratio.
	 *	Useful to for determining distance from camera fit in-game objects to the height of the screen.
	 */
	UFUNCTION(Category = "Victory BP Library|Game|Viewport", BlueprintPure)
	static float VerticalFOV(float HorizontalFOV, float AspectRatio);

	UFUNCTION(Category = "Victory BP Library|Utilities|String", BlueprintPure, Meta = (DisplayName = "IsEmpty"))
	static bool StringIsEmpty(const FString& Target);

//~~~~~~~~~

//~~~ KeyToTruth ~~~

//.h
/* Addition of strings (A + B) with pins. Contributed by KeyToTruth */
UFUNCTION(BlueprintPure, meta = (DisplayName = "Append Multiple", Keywords = "concatenate combine append strings", CommutativeAssociativeBinaryOperator = "true"), Category = "Victory BP Library|String")
static FString AppendMultiple(FString A, FString B);

//~~~ Mhousse ~~~

};

