// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

#include "UnrealHttp.h"

#define LOCTEXT_NAMESPACE "FUnrealHttpModule"

void FUnrealHttpModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FUnrealHttpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	//UE_LOG("")
	//FModuleManager::UnloadModule("WebSockets");
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealHttpModule, UnrealHttp)