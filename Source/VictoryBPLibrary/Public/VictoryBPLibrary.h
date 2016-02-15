/*
	By Rama
*/
#pragma once

#include "ModuleManager.h"

class FVictoryBPLibraryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};