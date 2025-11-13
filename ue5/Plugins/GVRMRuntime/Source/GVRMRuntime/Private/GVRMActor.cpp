// Copyright (c) 2025 gaussian-vrm-ue5/ue5/Plugins/GVRMRuntime/Source/GVRMRuntime/Private/GVRMActor.cpp community
// Licensed under the MIT License.

#include "GVRMActor.h"
#include "NiagaraDataInterfaceGVRM.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AGVRMActor::AGVRMActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);

	// Create VRM skeletal mesh component
	VRMSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VRMSkeletalMesh"));
	VRMSkeletalMesh->SetupAttachment(RootComponent);

	// Create Niagara component for splats
	SplatNiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SplatNiagaraSystem"));
	SplatNiagaraSystem->SetupAttachment(VRMSkeletalMesh);
	SplatNiagaraSystem->bAutoActivate = false; // We'll activate manually after initialization
}

void AGVRMActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize GVRM system
	if (bAutoActivateSplats)
	{
		if (!InitializeGVRM())
		{
			UE_LOG(LogTemp, Error, TEXT("AGVRMActor::BeginPlay - Failed to initialize GVRM"));
		}
	}
}

void AGVRMActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update performance stats
	UpdatePerformanceStats(DeltaTime);

	// Draw debug visualization if enabled
	if (bShowDebugInfo)
	{
		DrawDebugVisualization();
	}
}

bool AGVRMActor::InitializeGVRM()
{
	// Validate components and data
	if (!VRMSkeletalMesh)
	{
		const FString ErrorMsg = TEXT("VRM Skeletal Mesh component is not valid");
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::InitializeGVRM - %s"), *ErrorMsg);
		OnGVRMInitializationFailed.Broadcast(ErrorMsg);
		return false;
	}

	if (!BindingData)
	{
		const FString ErrorMsg = TEXT("Binding Data is not assigned");
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::InitializeGVRM - %s"), *ErrorMsg);
		OnGVRMInitializationFailed.Broadcast(ErrorMsg);
		return false;
	}

	// Validate binding data
	FString ValidationError;
	if (!BindingData->ValidateBindings(ValidationError))
	{
		const FString ErrorMsg = FString::Printf(TEXT("Binding data validation failed: %s"), *ValidationError);
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::InitializeGVRM - %s"), *ErrorMsg);
		OnGVRMInitializationFailed.Broadcast(ErrorMsg);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("AGVRMActor::InitializeGVRM - Binding data validated: %d splats"), BindingData->GetSplatCount());

	// Apply model scale
	if (bApplyModelScale)
	{
		ApplyModelScale();
	}

	// Apply bone operations
	if (bApplyBoneOperations)
	{
		ApplyBoneOperations();
	}

	// Setup Niagara system
	if (SplatNiagaraSystemAsset)
	{
		SplatNiagaraSystem->SetAsset(SplatNiagaraSystemAsset);
	}

	if (!SplatNiagaraSystem->GetAsset())
	{
		const FString ErrorMsg = TEXT("Niagara System asset is not assigned");
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::InitializeGVRM - %s"), *ErrorMsg);
		OnGVRMInitializationFailed.Broadcast(ErrorMsg);
		return false;
	}

	// Setup Niagara Data Interface
	if (!SetupNiagaraDataInterface())
	{
		const FString ErrorMsg = TEXT("Failed to setup Niagara Data Interface");
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::InitializeGVRM - %s"), *ErrorMsg);
		OnGVRMInitializationFailed.Broadcast(ErrorMsg);
		return false;
	}

	// Activate Niagara system
	if (bAutoActivateSplats)
	{
		ActivateSplats();
	}

	// Mark as initialized
	bIsInitialized = true;
	ActiveSplatCount = BindingData->GetSplatCount();

	UE_LOG(LogTemp, Log, TEXT("AGVRMActor::InitializeGVRM - Initialization successful (%d splats)"), ActiveSplatCount);
	OnGVRMInitialized.Broadcast();

	return true;
}

bool AGVRMActor::SetupNiagaraDataInterface()
{
	if (!SplatNiagaraSystem || !VRMSkeletalMesh)
	{
		return false;
	}

	// Find GVRM Data Interface in Niagara system
	// Note: This assumes the Niagara system has a user parameter named "GVRM_NDI" of type UNiagaraDataInterfaceGVRM
	// UE5.3+ API: Use UNiagaraComponent::GetDataInterface to retrieve data interface directly
	// GetDataInterface expects FString, not FName
	UNiagaraDataInterfaceGVRM* GVRMNDI = Cast<UNiagaraDataInterfaceGVRM>(
		SplatNiagaraSystem->GetDataInterface(FString("GVRM_NDI"))
	);

	if (!GVRMNDI)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGVRMActor::SetupNiagaraDataInterface - GVRM_NDI not found in Niagara system"));
		return false;
	}

	// Set skeletal mesh component reference
	GVRMNDI->SkeletalMeshComponent = VRMSkeletalMesh;

	UE_LOG(LogTemp, Log, TEXT("AGVRMActor::SetupNiagaraDataInterface - NDI configured with skeletal mesh"));
	return true;
}

void AGVRMActor::ApplyBoneOperations()
{
	if (!VRMSkeletalMesh || !BindingData)
	{
		return;
	}

	USkeletalMeshComponent* SkelComp = VRMSkeletalMesh;
	if (!SkelComp->SkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGVRMActor::ApplyBoneOperations - Skeletal mesh not loaded yet"));
		return;
	}

	int32 AppliedCount = 0;

	for (const FGVRMBoneOperation& BoneOp : BindingData->BoneOperations)
	{
		// Find bone index by name
		FName BoneName(*BoneOp.BoneName);
		int32 BoneIndex = SkelComp->GetBoneIndex(BoneName);

		if (BoneIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("AGVRMActor::ApplyBoneOperations - Bone not found: %s"), *BoneOp.BoneName);
			continue;
		}

		// Get current bone transform
		FTransform BoneTransform = SkelComp->GetBoneTransform(BoneIndex);

		// Apply position offset
		BoneTransform.AddToTranslation(BoneOp.PositionOffset);

		// Apply rotation
		FQuat RotationQuat = BoneOp.Rotation.Quaternion();
		BoneTransform.SetRotation(BoneTransform.GetRotation() * RotationQuat);

		// Note: SetBoneTransformByName was removed in UE5.3+
		// Bone operations should be applied through Animation Blueprints or Control Rig
		// For now, this function serves as a reference for bone operation data
		// TODO: Implement bone operations through Animation Blueprint or Control Rig system
		UE_LOG(LogTemp, Warning, TEXT("AGVRMActor::ApplyBoneOperations - Bone transform application requires Animation BP integration (bone: %s)"), *BoneName.ToString());

		AppliedCount++;
	}

	UE_LOG(LogTemp, Log, TEXT("AGVRMActor::ApplyBoneOperations - Applied %d bone operations"), AppliedCount);
}

void AGVRMActor::ApplyModelScale()
{
	if (!VRMSkeletalMesh || !BindingData)
	{
		return;
	}

	float Scale = BindingData->ModelScale;
	if (FMath::IsNearlyEqual(Scale, 1.0f))
	{
		return; // No scaling needed
	}

	FVector CurrentScale = VRMSkeletalMesh->GetRelativeScale3D();
	VRMSkeletalMesh->SetRelativeScale3D(CurrentScale * Scale);

	UE_LOG(LogTemp, Log, TEXT("AGVRMActor::ApplyModelScale - Applied scale: %.3f"), Scale);
}

void AGVRMActor::ActivateSplats()
{
	if (SplatNiagaraSystem && !SplatNiagaraSystem->IsActive())
	{
		SplatNiagaraSystem->Activate(true);
		UE_LOG(LogTemp, Log, TEXT("AGVRMActor::ActivateSplats - Niagara system activated"));
	}
}

void AGVRMActor::DeactivateSplats()
{
	if (SplatNiagaraSystem && SplatNiagaraSystem->IsActive())
	{
		SplatNiagaraSystem->Deactivate();
		UE_LOG(LogTemp, Log, TEXT("AGVRMActor::DeactivateSplats - Niagara system deactivated"));
	}
}

bool AGVRMActor::SetBindingData(UGVRMBindingData* NewBindingData)
{
	if (!NewBindingData)
	{
		UE_LOG(LogTemp, Error, TEXT("AGVRMActor::SetBindingData - Invalid binding data"));
		return false;
	}

	// Deactivate current system
	if (bIsInitialized)
	{
		DeactivateSplats();
		bIsInitialized = false;
	}

	// Set new binding data
	BindingData = NewBindingData;

	// Reinitialize
	return InitializeGVRM();
}

float AGVRMActor::GetCurrentFPS() const
{
	return CurrentFPS;
}

void AGVRMActor::DrawDebugVisualization()
{
	UWorld* World = GetWorld();
	if (!World || !VRMSkeletalMesh || !bIsInitialized)
	{
		return;
	}

	// Show bones
	if (bShowBones)
	{
		const TArray<FTransform>& BoneTransforms = VRMSkeletalMesh->GetComponentSpaceTransforms();
		for (int32 i = 0; i < BoneTransforms.Num(); ++i)
		{
			FVector BonePos = VRMSkeletalMesh->GetComponentTransform().TransformPosition(
				BoneTransforms[i].GetLocation()
			);
			DrawDebugSphere(World, BonePos, 2.0f, 8, FColor::Green, false, 0.0f, 0, 0.5f);
		}
	}

	// Show vertices (sampled every 100 vertices to avoid performance hit)
	// Note: Vertex visualization disabled in UE5.3+ due to API changes
	// TODO: Implement proper vertex visualization using UE5.3+ skeletal mesh APIs
	// Previous implementation used ComputeSkinnedPositions which requires additional parameters in UE5.3+
	if (bShowVertices && BindingData)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGVRMActor::DrawDebugVisualization - Vertex visualization not yet implemented for UE5.3+"));
	}
}

void AGVRMActor::UpdatePerformanceStats(float DeltaTime)
{
	LastFrameTime = DeltaTime;

	if (DeltaTime > 0.0f)
	{
		CurrentFPS = 1.0f / DeltaTime;
	}

	FrameCounter++;

	// Log stats every 5 seconds
	if (FrameCounter % 300 == 0 && bShowDebugInfo)
	{
		UE_LOG(LogTemp, Log, TEXT("GVRM Performance - FPS: %.1f, Splats: %d"), CurrentFPS, ActiveSplatCount);
	}
}
