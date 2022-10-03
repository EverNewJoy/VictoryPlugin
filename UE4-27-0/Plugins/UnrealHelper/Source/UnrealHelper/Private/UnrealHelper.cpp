// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

#include "UnrealHelper.h"

#define LOCTEXT_NAMESPACE "FUnrealHelperModule"

void FUnrealHelperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FUnrealHelperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealHelperModule, UnrealHelper)