// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GVRMSkinningData.h"
#include "GVRMActor.generated.h"

/**
 * GVRM Actor - Combines VRM skeletal mesh with Gaussian Splat animation.
 *
 * This actor manages:
 * - VRM skeletal mesh (animated character)
 * - Niagara system (Gaussian splats)
 * - GVRM binding data (splat-to-vertex mappings)
 *
 * Usage:
 * 1. Place actor in level
 * 2. Assign VRM skeletal mesh
 * 3. Assign GVRM binding data asset
 * 4. Assign Niagara system for splat rendering
 * 5. (Optional) Play animation on skeletal mesh
 */
UCLASS(BlueprintType, Blueprintable)
class GVRMRUNTIME_API AGVRMActor : public AActor
{
	GENERATED_BODY()

public:
	AGVRMActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ============================================
	// Components
	// ============================================

	/** VRM skeletal mesh component (character skeleton) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GVRM|Components")
	TObjectPtr<USkeletalMeshComponent> VRMSkeletalMesh;

	/** Niagara component for rendering Gaussian splats */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GVRM|Components")
	TObjectPtr<UNiagaraComponent> SplatNiagaraSystem;

	// ============================================
	// Configuration
	// ============================================

	/** GVRM binding data (generated from GVRM file) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Configuration")
	TObjectPtr<UGVRMBindingData> BindingData;

	/** Niagara system asset for splat rendering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Configuration")
	TObjectPtr<UNiagaraSystem> SplatNiagaraSystemAsset;

	/** Auto-activate Niagara system on BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Configuration")
	bool bAutoActivateSplats = true;

	/** Apply bone operations from binding data on initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Configuration")
	bool bApplyBoneOperations = true;

	/** Apply model scale from binding data on initialization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Configuration")
	bool bApplyModelScale = true;

	// ============================================
	// Runtime State
	// ============================================

	/** Whether the GVRM system is initialized and ready */
	UPROPERTY(BlueprintReadOnly, Category = "GVRM|State")
	bool bIsInitialized = false;

	/** Number of splats currently active */
	UPROPERTY(BlueprintReadOnly, Category = "GVRM|State")
	int32 ActiveSplatCount = 0;

	// ============================================
	// Initialization Functions
	// ============================================

	/**
	 * Initialize the GVRM system.
	 * Called automatically on BeginPlay.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	bool InitializeGVRM();

	/**
	 * Apply bone operations from binding data to VRM skeleton.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	void ApplyBoneOperations();

	/**
	 * Apply model scale from binding data to VRM skeleton.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	void ApplyModelScale();

	// ============================================
	// Runtime Control Functions
	// ============================================

	/**
	 * Activate the Gaussian splat rendering.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	void ActivateSplats();

	/**
	 * Deactivate the Gaussian splat rendering.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	void DeactivateSplats();

	/**
	 * Set the binding data and reinitialize.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	bool SetBindingData(UGVRMBindingData* NewBindingData);

	/**
	 * Get current FPS of the actor's tick.
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM")
	float GetCurrentFPS() const;

	// ============================================
	// Debug Visualization
	// ============================================

	/** Show debug information (bones, vertices) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Debug")
	bool bShowDebugInfo = false;

	/** Show bone visualization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Debug")
	bool bShowBones = false;

	/** Show vertex visualization (sampled) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GVRM|Debug")
	bool bShowVertices = false;

	/**
	 * Draw debug visualization (bones, vertices).
	 */
	UFUNCTION(BlueprintCallable, Category = "GVRM|Debug")
	void DrawDebugVisualization();

	// ============================================
	// Events
	// ============================================

	/** Called when GVRM initialization succeeds */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGVRMInitialized);
	UPROPERTY(BlueprintAssignable, Category = "GVRM|Events")
	FOnGVRMInitialized OnGVRMInitialized;

	/** Called when GVRM initialization fails */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGVRMInitializationFailed, const FString&, ErrorMessage);
	UPROPERTY(BlueprintAssignable, Category = "GVRM|Events")
	FOnGVRMInitializationFailed OnGVRMInitializationFailed;

protected:
	/**
	 * Setup Niagara Data Interface with skeletal mesh reference.
	 */
	bool SetupNiagaraDataInterface();

	/**
	 * Update performance stats.
	 */
	void UpdatePerformanceStats(float DeltaTime);

private:
	/** FPS tracking */
	float LastFrameTime = 0.0f;
	float CurrentFPS = 0.0f;

	/** Frame counter for stats */
	int32 FrameCounter = 0;
};
