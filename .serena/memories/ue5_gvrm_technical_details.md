# UE5 GVRM実装 - 技術詳細

## 1. アーキテクチャ概要

```
┌─────────────────────────────────────────────────────┐
│                    UE5 GVRM System                  │
├─────────────────────────────────────────────────────┤
│                                                      │
│  ┌────────────────┐      ┌─────────────────────┐  │
│  │  GVRM Actor    │      │ Skeletal Mesh Comp  │  │
│  │                │◄─────┤  (VRM4U)            │  │
│  │  - Binding Data│      │  - VRM Model        │  │
│  │  - Niagara Ref │      │  - Animation        │  │
│  └────────┬───────┘      └──────────┬──────────┘  │
│           │                           │             │
│           │        ┌──────────────────┘             │
│           ▼        ▼                                │
│  ┌─────────────────────────────┐                   │
│  │   Niagara Data Interface    │                   │
│  │         (NDI_GVRM)           │                   │
│  │                              │                   │
│  │  - GetBoneMatrix()           │                   │
│  │  - GetVertexPosition()       │                   │
│  │  - GetSkinningData()         │                   │
│  └────────────┬─────────────────┘                   │
│               │                                     │
│               ▼                                     │
│  ┌──────────────────────────────┐                  │
│  │    Niagara Emitter           │                  │
│  │   (GVRM Splat System)        │                  │
│  │                               │                  │
│  │  - Particle Spawn             │                  │
│  │  - Skinning Module (HLSL)    │                  │
│  │  - GS Renderer                │                  │
│  └──────────────────────────────┘                  │
│                                                      │
└─────────────────────────────────────────────────────┘
```

---

## 2. データフロー

### 2.1 初期化フロー

```
1. GVRMファイル読み込み
   └─> Python: gvrm_to_ue5.py
       ├─> model.vrm → VRM4Uでインポート
       ├─> model.ply → XVERSEでインポート
       └─> data.json → UGVRMBindingData生成

2. アクター初期化 (BeginPlay)
   └─> AGVRMActor::BeginPlay()
       ├─> VRMスケルタルメッシュをロード
       ├─> バインディングデータをロード
       ├─> Niagaraシステムを生成
       │   ├─> NDI_GVRMをセットアップ
       │   ├─> スプラットパーティクルをSpawn
       │   └─> 各パーティクルにバインディング情報を設定
       └─> GPUバッファを初期化
```

### 2.2 ランタイムフロー（毎フレーム）

```
Tick:
1. Animation Update
   └─> USkeletalMeshComponent::TickComponent()
       └─> Skeleton updated

2. NDI Update (AGVRMActor::Tick)
   └─> NDI_GVRM::UpdateGPUBuffers()
       ├─> ボーン行列を取得
       │   └─> Skeleton->GetComposedRefPoseMatrix()
       ├─> 頂点位置を取得（スキニング済み）
       │   └─> SkinnedMeshComponent->GetSkinnedVertexPosition()
       ├─> GPUバッファに転送
       │   ├─> BoneMatricesBuffer.Update()
       │   ├─> VertexPositionsBuffer.Update()
       │   └─> SkinningDataBuffer.Update()
       └─> Niagara system notified

3. Niagara Simulation
   └─> GVRM_SplatEmitter::SimulationStage_GPU()
       ├─> For each particle:
       │   ├─> Read VertexIndex
       │   ├─> Read RelativePosition
       │   ├─> Call ComputeSkinnedPosition() [HLSL]
       │   │   └─> LBS calculation
       │   └─> Update Particle.Position
       └─> Render Gaussian Splats

4. Rendering
   └─> GaussianSplatRenderer::Render()
       └─> Draw elliptical Gaussians
```

---

## 3. Linear Blend Skinning (LBS) 実装

### 3.1 理論

スプラットの最終位置は以下の式で計算：

```
P_final = SkinMatrix * (V_pos + R_pos)

where:
  SkinMatrix = Σ(w_i * M_bone_i)  (i = 0..3)
  V_pos = 頂点のベース位置
  R_pos = スプラットの相対位置
  w_i = ボーンウェイト
  M_bone_i = ボーン行列
```

### 3.2 gvrm.jsからの移植

**元のGLSLコード (gvrm.js:622-650):**
```glsl
// このレポジトリの実装
mat4 boneMatX0 = getBoneMatrix0( skinIndex.x );
mat4 boneMatY0 = getBoneMatrix0( skinIndex.y );
mat4 boneMatZ0 = getBoneMatrix0( skinIndex.z );
mat4 boneMatW0 = getBoneMatrix0( skinIndex.w );

mat4 skinMatrix0 = mat4( 0.0 );
skinMatrix0 += skinWeight.x * boneMatX0;
skinMatrix0 += skinWeight.y * boneMatY0;
skinMatrix0 += skinWeight.z * boneMatZ0;
skinMatrix0 += skinWeight.w * boneMatW0;
skinMatrix0 = bindMatrixInverse0 * skinMatrix0 * bindMatrix0;

// ... 同様に現在のボーン行列でskinMatrix1を計算

// 相対回転の計算
mat4 relativeRotation = skinMatrix1 * inverse(skinMatrix0);
vec4 quat = quatFromMat3(mat3(relativeRotation));

// 最終的なスプラット中心位置
vec3 transformed_skinned = (skinMatrix1 * vec4(transformed, 1.0)).xyz;
vec3 splatCenter = transformed_skinned + relativePos;
```

**UE5 HLSL移植版:**
```hlsl
// Niagara Custom HLSL Module: GVRM_Skinning

// Input parameters
int VertexIndex;
float3 RelativePosition;

// Output
float3 OutSplatPosition;
float4 OutSplatRotation;

// Main function
{
    // 1. 頂点のスキニング情報を取得
    float4 SkinIndices = GVRM.GetVertexSkinIndices(VertexIndex);
    float4 SkinWeights = GVRM.GetVertexSkinWeights(VertexIndex);
    
    // 2. 各ボーンの行列を取得
    float4x4 BoneMat0 = GVRM.GetBoneMatrix(SkinIndices.x);
    float4x4 BoneMat1 = GVRM.GetBoneMatrix(SkinIndices.y);
    float4x4 BoneMat2 = GVRM.GetBoneMatrix(SkinIndices.z);
    float4x4 BoneMat3 = GVRM.GetBoneMatrix(SkinIndices.w);
    
    // 3. スキニング行列の計算（LBS）
    float4x4 SkinMatrix = 
        SkinWeights.x * BoneMat0 +
        SkinWeights.y * BoneMat1 +
        SkinWeights.z * BoneMat2 +
        SkinWeights.w * BoneMat3;
    
    // 4. バインドポーズ変換（必要に応じて）
    // SkinMatrix = mul(mul(BindMatrixInverse, SkinMatrix), BindMatrix);
    
    // 5. 頂点位置を取得
    float3 VertexPos = GVRM.GetVertexPosition(VertexIndex);
    
    // 6. 最終位置計算
    float3 LocalPos = VertexPos + RelativePosition;
    OutSplatPosition = mul(SkinMatrix, float4(LocalPos, 1.0)).xyz;
    
    // 7. 回転計算（オプション、楕円ガウスの向き調整用）
    float3x3 RotMat = (float3x3)SkinMatrix;
    OutSplatRotation = MatrixToQuaternion(RotMat);
}

// Helper: 行列からクォータニオン変換
float4 MatrixToQuaternion(float3x3 m)
{
    float trace = m[0][0] + m[1][1] + m[2][2];
    float4 q;
    
    if (trace > 0.0)
    {
        float s = 0.5 / sqrt(trace + 1.0);
        q.w = 0.25 / s;
        q.x = (m[2][1] - m[1][2]) * s;
        q.y = (m[0][2] - m[2][0]) * s;
        q.z = (m[1][0] - m[0][1]) * s;
    }
    else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
    {
        float s = 2.0 * sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]);
        q.w = (m[2][1] - m[1][2]) / s;
        q.x = 0.25 * s;
        q.y = (m[0][1] + m[1][0]) / s;
        q.z = (m[0][2] + m[2][0]) / s;
    }
    else if (m[1][1] > m[2][2])
    {
        float s = 2.0 * sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]);
        q.w = (m[0][2] - m[2][0]) / s;
        q.x = (m[0][1] + m[1][0]) / s;
        q.y = 0.25 * s;
        q.z = (m[1][2] + m[2][1]) / s;
    }
    else
    {
        float s = 2.0 * sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]);
        q.w = (m[1][0] - m[0][1]) / s;
        q.x = (m[0][2] + m[2][0]) / s;
        q.y = (m[1][2] + m[2][1]) / s;
        q.z = 0.25 * s;
    }
    
    return q;
}
```

---

## 4. Niagara Data Interface詳細

### 4.1 NDIクラス定義

```cpp
// NiagaraDataInterfaceGVRM.h

#pragma once

#include "NiagaraDataInterface.h"
#include "NiagaraCommon.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraDataInterfaceGVRM.generated.h"

UCLASS(EditInlineNew, Category = "GVRM", meta = (DisplayName = "GVRM Skeletal Mesh"))
class UNiagaraDataInterfaceGVRM : public UNiagaraDataInterface
{
    GENERATED_UCLASS_BODY()

public:
    // UNiagaraDataInterface interface
    virtual void PostInitProperties() override;
    virtual void GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions) override;
    virtual void GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc) override;
    virtual bool Equals(const UNiagaraDataInterface* Other) const override;
    virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return true; }

    // GPU simulation
    virtual void GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL) override;
    virtual bool GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL) override;
    virtual void ProvidePerInstanceDataForRenderThread(void* DataForRenderThread, void* PerInstanceData, const FNiagaraSystemInstanceID& SystemInstance) override;

    // Properties
    UPROPERTY(EditAnywhere, Category = "GVRM")
    USkeletalMeshComponent* SourceComponent;

    UPROPERTY(EditAnywhere, Category = "GVRM")
    bool bRequireCurrentFrameData = true;

protected:
    // VM functions
    void VMGetVertexPosition(FVectorVMExternalFunctionContext& Context);
    void VMGetBoneMatrix(FVectorVMExternalFunctionContext& Context);
    void VMGetVertexSkinningData(FVectorVMExternalFunctionContext& Context);
    void VMComputeSkinnedPosition(FVectorVMExternalFunctionContext& Context);
};

// GPU data
struct FNDIGVRMInstanceData_RenderThread
{
    FReadBuffer BoneMatricesBuffer;
    FReadBuffer VertexPositionsBuffer;
    FReadBuffer SkinIndicesBuffer;
    FReadBuffer SkinWeightsBuffer;
    
    int32 NumBones = 0;
    int32 NumVertices = 0;
};

struct FNDIGVRMInstanceData_GameThread
{
    TWeakObjectPtr<USkeletalMeshComponent> SourceComponent;
    
    TArray<FMatrix> BoneMatrices;
    TArray<FVector> VertexPositions;
    TArray<FVector4> SkinIndices;
    TArray<FVector4> SkinWeights;
};
```

### 4.2 GPU buffer更新

```cpp
// NiagaraDataInterfaceGVRM.cpp

void UNiagaraDataInterfaceGVRM::ProvidePerInstanceDataForRenderThread(
    void* DataForRenderThread, 
    void* PerInstanceData, 
    const FNiagaraSystemInstanceID& SystemInstance)
{
    FNDIGVRMInstanceData_GameThread* SourceData = static_cast<FNDIGVRMInstanceData_GameThread*>(PerInstanceData);
    FNDIGVRMInstanceData_RenderThread* TargetData = new(DataForRenderThread) FNDIGVRMInstanceData_RenderThread();
    
    if (SourceData->SourceComponent.IsValid())
    {
        USkeletalMeshComponent* SkelComp = SourceData->SourceComponent.Get();
        
        // ボーン行列を取得
        TArray<FMatrix> BoneMatrices;
        const TArray<FTransform>& ComponentSpaceTransforms = SkelComp->GetComponentSpaceTransforms();
        BoneMatrices.Reserve(ComponentSpaceTransforms.Num());
        
        for (const FTransform& BoneTransform : ComponentSpaceTransforms)
        {
            BoneMatrices.Add(BoneTransform.ToMatrixWithScale());
        }
        
        // GPUバッファを更新（レンダースレッドで実行）
        ENQUEUE_RENDER_COMMAND(UpdateGVRMBuffers)(
            [TargetData, BoneMatrices](FRHICommandListImmediate& RHICmdList)
            {
                // ボーン行列バッファ
                if (BoneMatrices.Num() > 0)
                {
                    TargetData->NumBones = BoneMatrices.Num();
                    TargetData->BoneMatricesBuffer.Initialize(
                        sizeof(FMatrix), 
                        BoneMatrices.Num(), 
                        EPixelFormat::PF_A32B32G32R32F
                    );
                    
                    void* BufferData = RHILockBuffer(TargetData->BoneMatricesBuffer.Buffer, 0, BoneMatrices.Num() * sizeof(FMatrix), RLM_WriteOnly);
                    FMemory::Memcpy(BufferData, BoneMatrices.GetData(), BoneMatrices.Num() * sizeof(FMatrix));
                    RHIUnlockBuffer(TargetData->BoneMatricesBuffer.Buffer);
                }
            }
        );
    }
}
```

---

## 5. パフォーマンス最適化

### 5.1 LODシステム

```cpp
// GVRMLODManager.h

UENUM()
enum class EGVRMLODLevel : uint8
{
    High,      // 100% splats
    Medium,    // 50% splats
    Low,       // 25% splats
    VeryLow    // 10% splats
};

class FGVRMLODManager
{
public:
    static EGVRMLODLevel DetermineLODLevel(float Distance, float ScreenSize)
    {
        if (Distance < 500.0f || ScreenSize > 0.5f)
            return EGVRMLODLevel::High;
        else if (Distance < 1000.0f || ScreenSize > 0.25f)
            return EGVRMLODLevel::Medium;
        else if (Distance < 2000.0f)
            return EGVRMLODLevel::Low;
        else
            return EGVRMLODLevel::VeryLow;
    }
    
    static float GetLODSplatRatio(EGVRMLODLevel LOD)
    {
        switch (LOD)
        {
            case EGVRMLODLevel::High: return 1.0f;
            case EGVRMLODLevel::Medium: return 0.5f;
            case EGVRMLODLevel::Low: return 0.25f;
            case EGVRMLODLevel::VeryLow: return 0.1f;
            default: return 1.0f;
        }
    }
};
```

### 5.2 Compute Shader最適化

```cpp
// GVRMSkinningCS.usf (Compute Shader)

#include "/Engine/Private/Common.ush"

// Input buffers
StructuredBuffer<float4x4> BoneMatrices;
StructuredBuffer<float3> VertexPositions;
StructuredBuffer<float4> SkinIndices;
StructuredBuffer<float4> SkinWeights;
StructuredBuffer<int> SplatVertexIndices;
StructuredBuffer<float3> SplatRelativePositions;

// Output buffer
RWStructuredBuffer<float3> OutSplatPositions;

// Parameters
uint NumSplats;

[numthreads(256, 1, 1)]
void MainCS(uint3 ThreadId : SV_DispatchThreadID)
{
    uint SplatIndex = ThreadId.x;
    
    if (SplatIndex >= NumSplats)
        return;
    
    // バインディング情報取得
    int VertexIndex = SplatVertexIndices[SplatIndex];
    float3 RelativePos = SplatRelativePositions[SplatIndex];
    
    // スキニングデータ取得
    float4 SkinIdx = SkinIndices[VertexIndex];
    float4 SkinWgt = SkinWeights[VertexIndex];
    
    // LBS計算
    float4x4 SkinMatrix = 
        SkinWgt.x * BoneMatrices[SkinIdx.x] +
        SkinWgt.y * BoneMatrices[SkinIdx.y] +
        SkinWgt.z * BoneMatrices[SkinIdx.z] +
        SkinWgt.w * BoneMatrices[SkinIdx.w];
    
    // 最終位置
    float3 VertexPos = VertexPositions[VertexIndex];
    float3 LocalPos = VertexPos + RelativePos;
    OutSplatPositions[SplatIndex] = mul(float4(LocalPos, 1.0), SkinMatrix).xyz;
}
```

---

## 6. デバッグ・可視化

### 6.1 デバッグ描画

```cpp
// GVRMDebugDraw.cpp

void AGVRMActor::DrawDebugVisualization()
{
    if (!bShowDebugInfo)
        return;
    
    UWorld* World = GetWorld();
    if (!World)
        return;
    
    // ボーン表示
    if (bShowBones && VRMSkeleton)
    {
        const TArray<FTransform>& BoneTransforms = VRMSkeleton->GetComponentSpaceTransforms();
        for (int32 i = 0; i < BoneTransforms.Num(); ++i)
        {
            FVector BonePos = VRMSkeleton->GetComponentTransform().TransformPosition(BoneTransforms[i].GetLocation());
            DrawDebugSphere(World, BonePos, 5.0f, 8, FColor::Green, false, 0.0f);
        }
    }
    
    // 頂点表示（サンプリング）
    if (bShowVertices && BindingData)
    {
        for (int32 i = 0; i < BindingData->Bindings.Num(); i += 100) // 100個おきに表示
        {
            int32 VertexIdx = BindingData->Bindings[i].VertexIndex;
            FVector VertexPos = VRMSkeleton->GetSkinnedVertexPosition(VertexIdx);
            DrawDebugPoint(World, VertexPos, 3.0f, FColor::Blue, false, 0.0f);
        }
    }
}
```

### 6.2 パフォーマンス計測

```cpp
DECLARE_CYCLE_STAT(TEXT("GVRM Skinning"), STAT_GVRMSkinning, STATGROUP_GVRM);
DECLARE_CYCLE_STAT(TEXT("GVRM Buffer Update"), STAT_GVRMBufferUpdate, STATGROUP_GVRM);

void UNiagaraDataInterfaceGVRM::UpdateGPUBuffers()
{
    SCOPE_CYCLE_COUNTER(STAT_GVRMBufferUpdate);
    
    // Buffer update logic...
}
```

---

## 7. 既知の制限事項と対策

| 制限 | 影響 | 対策 |
|------|------|------|
| Niagaraパーティクル数制限 | 大規模GSで問題 | GPU Compute Shader使用 |
| スキニング計算コスト | フレームレート低下 | LOD、カリング、最適化 |
| メモリ使用量 | 大規模GSで高負荷 | ストリーミング、圧縮 |
| VRM4U互換性 | 一部VRMで問題 | フォールバック実装 |

---

## 8. 参考リソース

**Unreal Engine公式:**
- Niagara Overview: https://docs.unrealengine.com/5.3/en-US/overview-of-niagara-effects-for-unreal-engine/
- Custom NDI: Epic Developer Community Forums

**プラグイン:**
- XVERSE: https://github.com/xverse-engine/XScene-UEPlugin
- VRM4U: https://github.com/ruyo/VRM4U

**研究論文:**
- GaussianAvatars (CVPR 2024)
- 3DGS-Avatar (CVPR 2024)
- Human Gaussian Splatting

**このレポジトリ:**
- gvrm.js:533-776 (シェーダーカスタマイズ)
- gvrm.js:306-356 (updateByBones実装)
- preprocess.js (前処理アルゴリズム)
