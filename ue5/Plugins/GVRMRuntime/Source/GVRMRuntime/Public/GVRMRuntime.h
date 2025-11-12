// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * GVRM Runtime Module
 * Provides Niagara integration for Gaussian-VRM animated avatars
 */
class FGVRMRuntimeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
