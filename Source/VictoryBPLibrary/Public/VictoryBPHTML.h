/*
	
	By Rama

*/
#pragma once

#include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"
#include "VictoryBPHTML.generated.h"

// BP Library for You
//
// Written by Rama

//note about UBlueprintFunctionLibrary
// This class is a base class for any function libraries exposed to blueprints.
// Methods in subclasses are expected to be static, and no methods should be added to the base class.


UCLASS()
class VICTORYBPLIBRARY_API UVictoryBPHTML : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	/** Is the current OS HTML5? This code will only run in games packaged for HTML5, it will not run in Editor builds :) Use this to customize particle FX for HTML5 vs PC builds! Or for any custom HTML5-specific game logic! <3 Rama*/
	UFUNCTION(BlueprintPure, Category = "Victory BP Library|HTML5")
	static bool IsHTML();
	 
	UFUNCTION(BlueprintCallable, Category = "Victory BP Library|HTML5")
	static void VictoryHTML5_SetCursorVisible(bool MakeVisible);
	
}; 

