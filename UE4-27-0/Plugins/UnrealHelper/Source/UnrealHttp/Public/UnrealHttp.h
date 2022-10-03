// Copyright 2020 Disney Direct-to-Consumer and International. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
//

class FUnrealHttpModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
