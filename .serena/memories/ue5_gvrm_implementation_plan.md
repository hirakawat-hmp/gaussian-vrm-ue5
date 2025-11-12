# Unreal Engine 5 GVRM実装計画

## プロジェクト概要

**目的:** このレポジトリ（gaussian-vrm）のGVRM技術をUnreal Engine 5に移植し、VRMモデルにバインドされたGaussian Splattingアバターのアニメーションを実現する

**ベースとなる技術:**
- XVERSE XScene-UEPlugin（Gaussian Splatting表示）
- VRM4U（VRMモデルサポート）
- カスタムNiagara Data Interface（スケルタルメッシュ連携）

**開発期間見積もり:**
- フルタイム: 4-5週間
- パートタイム: 7-9週間

---

## Phase 1: 環境構築・検証 (1-2週間)

### 1.1 必要なソフトウェア
- Unreal Engine 5.3+ (推奨: 5.4)
- Visual Studio 2022 (C++ development tools)
- Python 3.8+ (データ変換用)

### 1.2 プラグインのインストール

**XVERSE XScene-UEPlugin:**
```bash
# GitHubからクローン
git clone https://github.com/xverse-engine/XScene-UEPlugin.git

# UE5プロジェクトのPluginsフォルダにコピー
cp -r XScene-UEPlugin/UEPlugin/Plugin/XV3DGS YourProject/Plugins/

# プロジェクトを開いてプラグインを有効化
```

**VRM4U:**
- Unreal Engine Marketplace / GitHubからインストール
- https://github.com/ruyo/VRM4U

### 1.3 動作検証

**Gaussian Splatting表示テスト:**
1. サンプルPLYファイルをインポート
2. レベルに配置して表示確認
3. カメラ移動、LOD動作を確認

**VRMモデルテスト:**
1. このレポジトリの`assets/sotai.vrm`をインポート
2. アニメーションをインポート（FBXまたはVRMアニメーション）
3. Sequencerでアニメーション再生確認

**成功基準:**
- GSとVRMが両方とも正常に表示される
- VRMアニメーションが60FPS以上で動作
- GSのレンダリングが安定している

---

## Phase 2: データパイプライン構築 (1週間)

### 2.1 GVRMパーサーの実装

**Python実装 (`tools/gvrm_to_ue5.py`):**
```python
import zipfile
import json
import csv
import struct

def parse_gvrm(gvrm_path, output_dir):
    """
    GVRMファイルを解析してUE5用データを出力
    
    出力ファイル:
    - splat_binding.csv: スプラットのバインディング情報
    - model.vrm: VRMモデル（そのまま）
    - model.ply: PLYファイル（そのまま）
    - metadata.json: その他のメタデータ
    """
    with zipfile.ZipFile(gvrm_path, 'r') as z:
        # data.jsonを読み込み
        data = json.loads(z.read('data.json'))
        
        # VRMとPLYを抽出
        z.extract('model.vrm', output_dir)
        z.extract('model.ply', output_dir)
        
        # バインディング情報をCSV化
        export_binding_data(data, f"{output_dir}/splat_binding.csv")
        
        # メタデータ
        export_metadata(data, f"{output_dir}/metadata.json")

def export_binding_data(data, csv_path):
    """スプラットバインディング情報をCSV出力"""
    with open(csv_path, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            'SplatIndex',
            'VertexIndex', 
            'BoneIndex',
            'RelativePosX', 
            'RelativePosY', 
            'RelativePosZ'
        ])
        
        vertex_indices = data['splatVertexIndices']
        bone_indices = data.get('splatBoneIndices', [])
        relative_poses = data['splatRelativePoses']
        
        for i in range(len(vertex_indices)):
            vertex_idx = vertex_indices[i]
            bone_idx = bone_indices[i] if bone_indices else -1
            rel_pos = relative_poses[i]
            
            writer.writerow([
                i,
                vertex_idx,
                bone_idx,
                rel_pos['x'],
                rel_pos['y'],
                rel_pos['z']
            ])
```

### 2.2 UE5へのインポート

**Data Asset作成:**
- カスタムData Asset型を定義: `UGVRMBindingData`
- CSVをインポートしてData Assetに変換
- エディタツールでワンクリック変換

**必要なデータ構造:**
```cpp
USTRUCT(BlueprintType)
struct FSplatBindingInfo
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 SplatIndex;
    
    UPROPERTY()
    int32 VertexIndex;
    
    UPROPERTY()
    int32 BoneIndex;
    
    UPROPERTY()
    FVector RelativePosition;
};

UCLASS()
class UGVRMBindingData : public UDataAsset
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    TArray<FSplatBindingInfo> Bindings;
    
    UPROPERTY(EditAnywhere)
    float ModelScale;
    
    UPROPERTY(EditAnywhere)
    TArray<FTransform> BoneOperations;
};
```

---

## Phase 3: カスタムNDI実装 (2-3週間)

### 3.1 NDIクラスの作成

**ファイル構成:**
```
Plugins/GVRMRuntime/
├── Source/
│   ├── GVRMRuntime/
│   │   ├── Public/
│   │   │   ├── NiagaraDataInterfaceGVRM.h
│   │   │   └── GVRMComponent.h
│   │   └── Private/
│   │       ├── NiagaraDataInterfaceGVRM.cpp
│   │       └── GVRMComponent.cpp
│   └── GVRMRuntime.Build.cs
└── GVRMRuntime.uplugin
```

### 3.2 実装する関数

**NDIで公開する関数:**
1. `GetSkinnedVertexPosition(int VertexIndex)` → FVector
2. `GetBoneMatrix(int BoneIndex)` → FMatrix
3. `GetVertexSkinningData(int VertexIndex)` → FSkinningData
4. `ComputeSkinnedPosition(int VertexIndex, FVector RelativePos)` → FVector

### 3.3 GPUバッファ管理

**更新するバッファ:**
- BoneMatrices (FMatrix4x4 array)
- VertexPositions (FVector3 array)
- SkinIndices (FVector4 array)
- SkinWeights (FVector4 array)

**更新頻度:**
- 毎フレーム（アニメーション中）
- OnDemand（静止時）

---

## Phase 4: Niagaraモジュール実装 (1週間)

### 4.1 Emitter構成

**Emitter: GVRM_SplatEmitter**
```
Emitter Spawn:
├─ Spawn Burst Instantaneous
│   └─ Spawn Count = バインディングデータのスプラット数

Particle Spawn:
├─ Initialize Particle
├─ Set Particles.VertexIndex (from Binding Data)
├─ Set Particles.RelativePosition (from Binding Data)
└─ Set Particles.LifeTime = Infinite

Particle Update:
├─ GVRM Skinning Module (Custom)
│   └─ Particles.Position = ComputeSkinnedPosition()
└─ Update Mesh Reproduction Sprite
```

### 4.2 カスタムHLSLモジュール

**Module: GVRM Skinning**
```hlsl
// Custom HLSL
void ComputeSplatPosition(
    int VertexIndex,
    float3 RelativePosition,
    out float3 OutPosition
)
{
    // 1. 頂点のスキニング情報取得
    float4 SkinIndices = GVRM.GetVertexSkinIndices(VertexIndex);
    float4 SkinWeights = GVRM.GetVertexSkinWeights(VertexIndex);
    
    // 2. LBS (Linear Blend Skinning)
    float4x4 SkinMatrix = 
        SkinWeights.x * GVRM.GetBoneMatrix(SkinIndices.x) +
        SkinWeights.y * GVRM.GetBoneMatrix(SkinIndices.y) +
        SkinWeights.z * GVRM.GetBoneMatrix(SkinIndices.z) +
        SkinWeights.w * GVRM.GetBoneMatrix(SkinIndices.w);
    
    // 3. 頂点位置取得
    float3 VertexPos = GVRM.GetVertexPosition(VertexIndex);
    
    // 4. 最終位置計算
    float3 LocalPos = VertexPos + RelativePosition;
    OutPosition = mul(SkinMatrix, float4(LocalPos, 1.0)).xyz;
}
```

---

## Phase 5: 統合・最適化 (2週間)

### 5.1 GVRMコンポーネント作成

**AGVRMActor:**
```cpp
UCLASS()
class AGVRMActor : public AActor
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* VRMSkeleton;
    
    UPROPERTY(EditAnywhere)
    UNiagaraComponent* SplatSystem;
    
    UPROPERTY(EditAnywhere)
    UGVRMBindingData* BindingData;
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
private:
    void UpdateSplatSystem();
};
```

### 5.2 パフォーマンス最適化

**ターゲット:**
- 60 FPS以上（VR対応の場合は90 FPS）
- スプラット数: 10万〜50万

**最適化手法:**
1. **LOD実装**
   - 距離に応じてスプラット密度を削減
   - 3段階LOD（High/Medium/Low）

2. **カリング**
   - フラスタムカリング
   - オクルージョンカリング

3. **GPU Compute Shader**
   - スキニング計算をCompute Shaderに移行
   - 並列処理で高速化

4. **ストリーミング**
   - スプラットデータの遅延ロード
   - メモリ使用量の削減

### 5.3 Blueprint統合

**Blueprintインターフェース:**
```
Functions:
- LoadGVRMFile(FilePath) → bool
- PlayAnimation(AnimationAsset)
- SetPose(BoneName, Transform)
- GetCurrentFPS() → float

Events:
- OnGVRMLoaded
- OnAnimationStarted
- OnAnimationFinished
```

---

## マイルストーン

| Phase | 期間 | 主要成果物 | 完了条件 |
|-------|------|-----------|---------|
| Phase 1 | Week 1-2 | 環境構築完了 | GS/VRM両方が表示される |
| Phase 2 | Week 3 | データパイプライン | GVRMファイルをUE5形式に変換できる |
| Phase 3 | Week 4-6 | NDI実装 | Niagaraからスケルタルメッシュデータにアクセスできる |
| Phase 4 | Week 7 | Niagaraモジュール | スプラットがVRMアニメーションに追従する |
| Phase 5 | Week 8-9 | 統合・最適化 | 60FPS以上で安定動作 |

---

## リスク管理

### 技術的リスク

| リスク | 影響度 | 対策 |
|--------|--------|------|
| Niagaraパーティクル制限 | 中 | GPUバッファ使用、XVERSEの実装を参考 |
| スキニング計算の性能 | 中 | Compute Shader実装、LOD導入 |
| UE5バージョン互換性 | 低 | 5.3/5.4両対応 |
| VRM4Uとの統合問題 | 中 | 早期検証、代替手段の検討 |

### 学習曲線リスク

| 分野 | 難易度 | 学習リソース |
|------|--------|-------------|
| Niagara NDI開発 | 高 | Epic公式ドキュメント、XVERSEソースコード |
| UE5レンダリングパイプライン | 高 | UE5ソースコード、コミュニティフォーラム |
| VRM4U API | 中 | GitHub Wiki、サンプルプロジェクト |

---

## 次のステップ

1. ✅ Serenaメモリに実装計画を保存（このファイル）
2. ⬜ UE5プロジェクトを作成
3. ⬜ XVERSE/VRM4Uプラグインをインストール
4. ⬜ Pythonツール実装開始
5. ⬜ NDIプロトタイプ作成
