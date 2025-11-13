// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraDataInterface.h"
#include "NiagaraCommon.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraDataInterfaceGVRM.generated.h"

/**
 * Niagara Data Interface for accessing GVRM skeletal mesh data.
 * Provides vertex positions, normals, bone indices, and bone weights
 * to Niagara particles for Linear Blend Skinning (LBS) calculations.
 */
UCLASS(EditInlineNew, Category = "GVRM", meta = (DisplayName = "GVRM Skeletal Mesh Data"))
class GVRMRUNTIME_API UNiagaraDataInterfaceGVRM : public UNiagaraDataInterface
{
	GENERATED_BODY()

public:
	UNiagaraDataInterfaceGVRM();

	// UNiagaraDataInterface Interface
	virtual void GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions) override;
	virtual void GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc) override;
	virtual bool Equals(const UNiagaraDataInterface* Other) const override;
	virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return Target == ENiagaraSimTarget::GPUComputeSim; }
	virtual bool InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	virtual void DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	virtual int32 PerInstanceDataSize() const override;
	virtual bool PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds) override;
	virtual bool HasPreSimulateTick() const override { return true; }

#if WITH_EDITORONLY_DATA
	virtual void GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL) override;
	virtual bool GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL) override;
#endif

	virtual void ProvidePerInstanceDataForRenderThread(void* DataForRenderThread, void* PerInstanceData, const FNiagaraSystemInstanceID& SystemInstance) override;

protected:
	virtual bool CopyToInternal(UNiagaraDataInterface* Destination) const override;

public:
	/** Reference to the skeletal mesh component containing vertex and bone data */
	UPROPERTY(EditAnywhere, Category = "GVRM")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	/** Maximum number of bone influences per vertex (typically 4) */
	UPROPERTY(EditAnywhere, Category = "GVRM", meta = (ClampMin = "1", ClampMax = "8"))
	int32 MaxBoneInfluences = 4;

private:
	// Function names for Niagara VM binding
	static const FName GetVertexPositionName;
	static const FName GetVertexNormalName;
	static const FName GetVertexBoneIndicesName;
	static const FName GetVertexBoneWeightsName;
	static const FName GetBoneTransformName;
	static const FName GetNumVerticesName;

	// VM function implementations (CPU fallback)
	void VMGetVertexPosition(FVectorVMExternalFunctionContext& Context);
	void VMGetVertexNormal(FVectorVMExternalFunctionContext& Context);
	void VMGetVertexBoneIndices(FVectorVMExternalFunctionContext& Context);
	void VMGetVertexBoneWeights(FVectorVMExternalFunctionContext& Context);
	void VMGetBoneTransform(FVectorVMExternalFunctionContext& Context);
	void VMGetNumVertices(FVectorVMExternalFunctionContext& Context);
};

/**
 * Per-instance data for the GVRM Data Interface.
 * Cached data that is updated per frame.
 */
struct FNiagaraDataInterfaceGVRMInstanceData
{
	/** Cached skeletal mesh component reference */
	TWeakObjectPtr<USkeletalMeshComponent> CachedSkeletalMeshComponent;

	/** Cached vertex positions in component space (before skinning) */
	TArray<FVector3f> CachedVertexPositions;

	/** Cached vertex normals in component space (before skinning) */
	TArray<FVector3f> CachedVertexNormals;

	/** Cached bone indices per vertex (4 indices per vertex) */
	TArray<FIntVector4> CachedBoneIndices;

	/** Cached bone weights per vertex (4 weights per vertex, sum to 1.0) */
	TArray<FVector4f> CachedBoneWeights;

	/** Cached bone transforms (component space to world space) */
	TArray<FMatrix44f> CachedBoneMatrices;

	/** Number of vertices in the skeletal mesh */
	int32 NumVertices = 0;

	/** Number of bones in the skeleton */
	int32 NumBones = 0;

	/** Frame counter for cache invalidation */
	uint32 CachedFrameNumber = 0;

	/** Whether the cache is valid */
	bool bCacheValid = false;

	/**
	 * Update cached skeletal mesh data.
	 * Should be called once per frame in PreSimulateTick.
	 */
	void UpdateCache(USkeletalMeshComponent* SkeletalMesh, int32 MaxBoneInfluences);

	/**
	 * Invalidate the cache, forcing a refresh on next access.
	 */
	void InvalidateCache()
	{
		bCacheValid = false;
	}
};

/**
 * GPU compute shader parameters for GVRM Data Interface.
 * Exposes skeletal mesh data as shader resources.
 */
struct FNiagaraDataInterfaceGVRMProxy : public FNiagaraDataInterfaceProxy
{
	// GPU buffers (RHI resources)
	FBufferRHIRef VertexPositionsBuffer;
	FBufferRHIRef VertexNormalsBuffer;
	FBufferRHIRef BoneIndicesBuffer;
	FBufferRHIRef BoneWeightsBuffer;
	FBufferRHIRef BoneMatricesBuffer;

	// Shader resource views for GPU access
	FShaderResourceViewRHIRef VertexPositionsSRV;
	FShaderResourceViewRHIRef VertexNormalsSRV;
	FShaderResourceViewRHIRef BoneIndicesSRV;
	FShaderResourceViewRHIRef BoneWeightsSRV;
	FShaderResourceViewRHIRef BoneMatricesSRV;

	// Buffer dimensions
	int32 NumVertices = 0;
	int32 NumBones = 0;
	int32 MaxBoneInfluences = 4;

	virtual void PreStage(const FNDIGpuComputePreStageContext& Context) override;
	virtual void PostStage(const FNDIGpuComputePostStageContext& Context) override;

	// Destructor to clean up GPU resources
	virtual ~FNiagaraDataInterfaceGVRMProxy()
	{
		VertexPositionsBuffer.SafeRelease();
		VertexNormalsBuffer.SafeRelease();
		BoneIndicesBuffer.SafeRelease();
		BoneWeightsBuffer.SafeRelease();
		BoneMatricesBuffer.SafeRelease();
	}
};
