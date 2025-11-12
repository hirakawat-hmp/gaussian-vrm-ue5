// Copyright (c) 2025 gaussian-vrm community
// Licensed under the MIT License.

#include "NiagaraDataInterfaceGVRM.h"
#include "NiagaraShader.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraRenderer.h"
#include "ShaderParameterUtils.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"

// Function name constants
const FName UNiagaraDataInterfaceGVRM::GetVertexPositionName(TEXT("GetVertexPosition"));
const FName UNiagaraDataInterfaceGVRM::GetVertexNormalName(TEXT("GetVertexNormal"));
const FName UNiagaraDataInterfaceGVRM::GetVertexBoneIndicesName(TEXT("GetVertexBoneIndices"));
const FName UNiagaraDataInterfaceGVRM::GetVertexBoneWeightsName(TEXT("GetVertexBoneWeights"));
const FName UNiagaraDataInterfaceGVRM::GetBoneTransformName(TEXT("GetBoneTransform"));
const FName UNiagaraDataInterfaceGVRM::GetNumVerticesName(TEXT("GetNumVertices"));

UNiagaraDataInterfaceGVRM::UNiagaraDataInterfaceGVRM()
	: SkeletalMeshComponent(nullptr)
	, MaxBoneInfluences(4)
{
}

void UNiagaraDataInterfaceGVRM::GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)
{
	// GetVertexPosition(int VertexIndex) -> float3
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetVertexPositionName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("VertexIndex")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Position")));
		OutFunctions.Add(Sig);
	}

	// GetVertexNormal(int VertexIndex) -> float3
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetVertexNormalName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("VertexIndex")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Normal")));
		OutFunctions.Add(Sig);
	}

	// GetVertexBoneIndices(int VertexIndex) -> int4
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetVertexBoneIndicesName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("VertexIndex")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("BoneIndex0")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("BoneIndex1")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("BoneIndex2")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("BoneIndex3")));
		OutFunctions.Add(Sig);
	}

	// GetVertexBoneWeights(int VertexIndex) -> float4
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetVertexBoneWeightsName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("VertexIndex")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec4Def(), TEXT("Weights")));
		OutFunctions.Add(Sig);
	}

	// GetBoneTransform(int BoneIndex) -> float4x4
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetBoneTransformName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("BoneIndex")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetMatrix4Def(), TEXT("Transform")));
		OutFunctions.Add(Sig);
	}

	// GetNumVertices() -> int
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetNumVerticesName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("GVRM")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("NumVertices")));
		OutFunctions.Add(Sig);
	}
}

void UNiagaraDataInterfaceGVRM::GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc)
{
	if (BindingInfo.Name == GetVertexPositionName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetVertexPosition);
	}
	else if (BindingInfo.Name == GetVertexNormalName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetVertexNormal);
	}
	else if (BindingInfo.Name == GetVertexBoneIndicesName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetVertexBoneIndices);
	}
	else if (BindingInfo.Name == GetVertexBoneWeightsName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetVertexBoneWeights);
	}
	else if (BindingInfo.Name == GetBoneTransformName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetBoneTransform);
	}
	else if (BindingInfo.Name == GetNumVerticesName)
	{
		OutFunc = FVMExternalFunction::CreateUObject(this, &UNiagaraDataInterfaceGVRM::VMGetNumVertices);
	}
}

bool UNiagaraDataInterfaceGVRM::Equals(const UNiagaraDataInterface* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UNiagaraDataInterfaceGVRM* OtherTyped = CastChecked<const UNiagaraDataInterfaceGVRM>(Other);
	return OtherTyped->SkeletalMeshComponent == SkeletalMeshComponent
		&& OtherTyped->MaxBoneInfluences == MaxBoneInfluences;
}

bool UNiagaraDataInterfaceGVRM::CopyToInternal(UNiagaraDataInterface* Destination) const
{
	if (!Super::CopyToInternal(Destination))
	{
		return false;
	}

	UNiagaraDataInterfaceGVRM* DestTyped = CastChecked<UNiagaraDataInterfaceGVRM>(Destination);
	DestTyped->SkeletalMeshComponent = SkeletalMeshComponent;
	DestTyped->MaxBoneInfluences = MaxBoneInfluences;
	return true;
}

int32 UNiagaraDataInterfaceGVRM::PerInstanceDataSize() const
{
	return sizeof(FNiagaraDataInterfaceGVRMInstanceData);
}

bool UNiagaraDataInterfaceGVRM::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FNiagaraDataInterfaceGVRMInstanceData* InstanceData = new (PerInstanceData) FNiagaraDataInterfaceGVRMInstanceData();
	InstanceData->CachedSkeletalMeshComponent = SkeletalMeshComponent;
	return true;
}

void UNiagaraDataInterfaceGVRM::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FNiagaraDataInterfaceGVRMInstanceData* InstanceData = static_cast<FNiagaraDataInterfaceGVRMInstanceData*>(PerInstanceData);
	InstanceData->~FNiagaraDataInterfaceGVRMInstanceData();
}

bool UNiagaraDataInterfaceGVRM::PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds)
{
	FNiagaraDataInterfaceGVRMInstanceData* InstanceData = static_cast<FNiagaraDataInterfaceGVRMInstanceData*>(PerInstanceData);

	if (InstanceData && SkeletalMeshComponent.Get())
	{
		InstanceData->UpdateCache(SkeletalMeshComponent.Get(), MaxBoneInfluences);
		return true;
	}

	return false;
}

#if WITH_EDITORONLY_DATA
void UNiagaraDataInterfaceGVRM::GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL)
{
	OutHLSL += TEXT("Buffer<float3> {ParameterName}_VertexPositions;\n");
	OutHLSL += TEXT("Buffer<float3> {ParameterName}_VertexNormals;\n");
	OutHLSL += TEXT("Buffer<int4> {ParameterName}_BoneIndices;\n");
	OutHLSL += TEXT("Buffer<float4> {ParameterName}_BoneWeights;\n");
	OutHLSL += TEXT("Buffer<float4x4> {ParameterName}_BoneMatrices;\n");
	OutHLSL += TEXT("int {ParameterName}_NumVertices;\n");
	OutHLSL += TEXT("int {ParameterName}_NumBones;\n");
}

bool UNiagaraDataInterfaceGVRM::GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL)
{
	if (FunctionInfo.DefinitionName == GetVertexPositionName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(int VertexIndex, out float3 Position)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    Position = {ParameterName}_VertexPositions[VertexIndex];\n"));
		OutHLSL += TEXT("}\n");
		return true;
	}
	else if (FunctionInfo.DefinitionName == GetVertexNormalName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(int VertexIndex, out float3 Normal)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    Normal = {ParameterName}_VertexNormals[VertexIndex];\n"));
		OutHLSL += TEXT("}\n");
		return true;
	}
	else if (FunctionInfo.DefinitionName == GetVertexBoneIndicesName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(int VertexIndex, out int BoneIndex0, out int BoneIndex1, out int BoneIndex2, out int BoneIndex3)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    int4 Indices = {ParameterName}_BoneIndices[VertexIndex];\n"));
		OutHLSL += TEXT("    BoneIndex0 = Indices.x;\n");
		OutHLSL += TEXT("    BoneIndex1 = Indices.y;\n");
		OutHLSL += TEXT("    BoneIndex2 = Indices.z;\n");
		OutHLSL += TEXT("    BoneIndex3 = Indices.w;\n");
		OutHLSL += TEXT("}\n");
		return true;
	}
	else if (FunctionInfo.DefinitionName == GetVertexBoneWeightsName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(int VertexIndex, out float4 Weights)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    Weights = {ParameterName}_BoneWeights[VertexIndex];\n"));
		OutHLSL += TEXT("}\n");
		return true;
	}
	else if (FunctionInfo.DefinitionName == GetBoneTransformName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(int BoneIndex, out float4x4 Transform)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    Transform = {ParameterName}_BoneMatrices[BoneIndex];\n"));
		OutHLSL += TEXT("}\n");
		return true;
	}
	else if (FunctionInfo.DefinitionName == GetNumVerticesName)
	{
		OutHLSL += FString::Printf(TEXT("void %s(out int NumVertices)\n{\n"), *FunctionInfo.InstanceName);
		OutHLSL += FString::Printf(TEXT("    NumVertices = {ParameterName}_NumVertices;\n"));
		OutHLSL += TEXT("}\n");
		return true;
	}

	return false;
}
#endif

// VM function implementations (CPU fallback)
void UNiagaraDataInterfaceGVRM::VMGetVertexPosition(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIInputParam<int32> VertexIndexParam(Context);
	FNDIOutputParam<FVector3f> OutPosition(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const int32 VertexIndex = VertexIndexParam.GetAndAdvance();
		FVector3f Position = FVector3f::ZeroVector;

		if (InstanceData->bCacheValid && VertexIndex >= 0 && VertexIndex < InstanceData->NumVertices)
		{
			Position = InstanceData->CachedVertexPositions[VertexIndex];
		}

		OutPosition.SetAndAdvance(Position);
	}
}

void UNiagaraDataInterfaceGVRM::VMGetVertexNormal(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIInputParam<int32> VertexIndexParam(Context);
	FNDIOutputParam<FVector3f> OutNormal(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const int32 VertexIndex = VertexIndexParam.GetAndAdvance();
		FVector3f Normal = FVector3f::ZAxisVector;

		if (InstanceData->bCacheValid && VertexIndex >= 0 && VertexIndex < InstanceData->NumVertices)
		{
			Normal = InstanceData->CachedVertexNormals[VertexIndex];
		}

		OutNormal.SetAndAdvance(Normal);
	}
}

void UNiagaraDataInterfaceGVRM::VMGetVertexBoneIndices(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIInputParam<int32> VertexIndexParam(Context);
	FNDIOutputParam<int32> OutBoneIndex0(Context);
	FNDIOutputParam<int32> OutBoneIndex1(Context);
	FNDIOutputParam<int32> OutBoneIndex2(Context);
	FNDIOutputParam<int32> OutBoneIndex3(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const int32 VertexIndex = VertexIndexParam.GetAndAdvance();
		FIntVector4 BoneIndices(0, 0, 0, 0);

		if (InstanceData->bCacheValid && VertexIndex >= 0 && VertexIndex < InstanceData->NumVertices)
		{
			BoneIndices = InstanceData->CachedBoneIndices[VertexIndex];
		}

		OutBoneIndex0.SetAndAdvance(BoneIndices.X);
		OutBoneIndex1.SetAndAdvance(BoneIndices.Y);
		OutBoneIndex2.SetAndAdvance(BoneIndices.Z);
		OutBoneIndex3.SetAndAdvance(BoneIndices.W);
	}
}

void UNiagaraDataInterfaceGVRM::VMGetVertexBoneWeights(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIInputParam<int32> VertexIndexParam(Context);
	FNDIOutputParam<FVector4f> OutWeights(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const int32 VertexIndex = VertexIndexParam.GetAndAdvance();
		FVector4f Weights(1.0f, 0.0f, 0.0f, 0.0f);

		if (InstanceData->bCacheValid && VertexIndex >= 0 && VertexIndex < InstanceData->NumVertices)
		{
			Weights = InstanceData->CachedBoneWeights[VertexIndex];
		}

		OutWeights.SetAndAdvance(Weights);
	}
}

void UNiagaraDataInterfaceGVRM::VMGetBoneTransform(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIInputParam<int32> BoneIndexParam(Context);
	FNDIOutputParam<FMatrix44f> OutTransform(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const int32 BoneIndex = BoneIndexParam.GetAndAdvance();
		FMatrix44f Transform = FMatrix44f::Identity;

		if (InstanceData->bCacheValid && BoneIndex >= 0 && BoneIndex < InstanceData->NumBones)
		{
			Transform = InstanceData->CachedBoneMatrices[BoneIndex];
		}

		OutTransform.SetAndAdvance(Transform);
	}
}

void UNiagaraDataInterfaceGVRM::VMGetNumVertices(FVectorVMExternalFunctionContext& Context)
{
	VectorVM::FUserPtrHandler<FNiagaraDataInterfaceGVRMInstanceData> InstanceData(Context);
	FNDIOutputParam<int32> OutNumVertices(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		OutNumVertices.SetAndAdvance(InstanceData->NumVertices);
	}
}

// Instance data cache update implementation
void FNiagaraDataInterfaceGVRMInstanceData::UpdateCache(USkeletalMeshComponent* SkeletalMesh, int32 MaxBoneInfluences)
{
	if (!SkeletalMesh || !SkeletalMesh->SkeletalMesh)
	{
		bCacheValid = false;
		return;
	}

	// Check if we need to update (frame number changed)
	const uint32 CurrentFrameNumber = GFrameNumber;
	if (bCacheValid && CachedFrameNumber == CurrentFrameNumber)
	{
		return; // Cache still valid for this frame
	}

	CachedFrameNumber = CurrentFrameNumber;
	CachedSkeletalMeshComponent = SkeletalMesh;

	// Get render data
	FSkeletalMeshRenderData* RenderData = SkeletalMesh->SkeletalMesh->GetResourceForRendering();
	if (!RenderData || RenderData->LODRenderData.Num() == 0)
	{
		bCacheValid = false;
		return;
	}

	// Use LOD 0
	FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[0];
	NumVertices = LODData.StaticVertexBuffers.PositionVertexBuffer.GetNumVertices();

	// Cache vertex positions and normals
	CachedVertexPositions.SetNum(NumVertices);
	CachedVertexNormals.SetNum(NumVertices);
	for (int32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
	{
		CachedVertexPositions[VertexIndex] = LODData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
		CachedVertexNormals[VertexIndex] = FVector3f(LODData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex));
	}

	// Cache bone skinning data
	CachedBoneIndices.SetNum(NumVertices);
	CachedBoneWeights.SetNum(NumVertices);

	const FSkinWeightVertexBuffer* SkinWeightBuffer = LODData.GetSkinWeightVertexBuffer();
	for (int32 VertexIndex = 0; VertexIndex < NumVertices; ++VertexIndex)
	{
		FIntVector4 BoneIndices(0, 0, 0, 0);
		FVector4f BoneWeights(1.0f, 0.0f, 0.0f, 0.0f);

		if (SkinWeightBuffer)
		{
			// Get up to MaxBoneInfluences (typically 4)
			for (int32 InfluenceIdx = 0; InfluenceIdx < FMath::Min(MaxBoneInfluences, 4); ++InfluenceIdx)
			{
				const int32 BoneIndex = SkinWeightBuffer->GetBoneIndex(VertexIndex, InfluenceIdx);
				const float BoneWeight = SkinWeightBuffer->GetBoneWeight(VertexIndex, InfluenceIdx);

				switch (InfluenceIdx)
				{
				case 0: BoneIndices.X = BoneIndex; BoneWeights.X = BoneWeight; break;
				case 1: BoneIndices.Y = BoneIndex; BoneWeights.Y = BoneWeight; break;
				case 2: BoneIndices.Z = BoneIndex; BoneWeights.Z = BoneWeight; break;
				case 3: BoneIndices.W = BoneIndex; BoneWeights.W = BoneWeight; break;
				}
			}
		}

		CachedBoneIndices[VertexIndex] = BoneIndices;
		CachedBoneWeights[VertexIndex] = BoneWeights;
	}

	// Cache bone transforms
	const TArray<FTransform>& ComponentSpaceTransforms = SkeletalMesh->GetComponentSpaceTransforms();
	NumBones = ComponentSpaceTransforms.Num();
	CachedBoneMatrices.SetNum(NumBones);

	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		CachedBoneMatrices[BoneIndex] = FMatrix44f(ComponentSpaceTransforms[BoneIndex].ToMatrixWithScale());
	}

	bCacheValid = true;
}

// GPU proxy implementations (stubs for now, will be implemented in Phase 3)
void FNiagaraDataInterfaceGVRMProxy::PreStage(const FNDIGpuComputePreStageContext& Context)
{
	// TODO: Upload buffers to GPU
}

void FNiagaraDataInterfaceGVRMProxy::PostStage(const FNDIGpuComputePostStageContext& Context)
{
	// TODO: Cleanup GPU resources
}
