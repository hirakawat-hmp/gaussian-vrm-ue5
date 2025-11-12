// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#include "GVRMRuntime.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FGVRMRuntimeModule"

void FGVRMRuntimeModule::StartupModule()
{
	// Register shader directory
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("GVRMRuntime"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/GVRMRuntime"), PluginShaderDir);

	UE_LOG(LogTemp, Log, TEXT("GVRMRuntime module started"));
}

void FGVRMRuntimeModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("GVRMRuntime module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGVRMRuntimeModule, GVRMRuntime)
