# UE5でのPython使用ガイド

## Pythonの使用範囲

### ✅ 使える場所

#### 1. **エディタスクリプト（推奨）**
UE5エディタ内でPythonを実行可能

**用途:**
- アセットのインポート自動化
- バッチ処理
- エディタUIカスタマイズ
- プロジェクトセットアップ

**セットアップ:**
```
Edit → Plugins → Scripting → Python Editor Script Plugin を有効化
```

**例:**
```python
# UE5エディタ内で実行
import unreal

# アセットをインポート
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
import_task = unreal.AssetImportTask()
import_task.filename = "C:/path/to/file.fbx"
import_task.destination_path = "/Game/Assets"
asset_tools.import_asset_tasks([import_task])
```

#### 2. **オフラインツール（このプロジェクトで主に使用）**
UE5とは別にコマンドラインで実行

**用途:**
- GVRMファイルの変換
- データ前処理
- バリデーション
- バッチ変換

**例:**
```bash
# UE5の外で実行
python tools/gvrm_to_ue5.py assets/author.gvrm -o output/
```

---

### ❌ 使えない場所

#### **ランタイム（ゲーム実行中）**
- ゲームがパッケージ化されて実行される時
- プレイヤーがプレイしている時
- 独立した.exeとして配布された後

**ランタイムで使える言語:**
- C++（ネイティブコード）
- Blueprint（ビジュアルスクリプト）
- **Pythonは使えない**

---

## このプロジェクトでのPython使用戦略

### アーキテクチャ

```
┌─────────────────────────────────────────────────┐
│           開発時（オフライン）                    │
├─────────────────────────────────────────────────┤
│                                                  │
│  Python Tools (ue5/Tools/)                      │
│  ├─ gvrm_to_ue5.py           ← オフライン変換   │
│  ├─ batch_convert.py                            │
│  └─ validate_gvrm.py                            │
│         ↓                                        │
│  変換されたデータ (.csv, .json)                  │
│         ↓                                        │
│  UE5 Data Assets (.uasset)                      │
│                                                  │
└─────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────┐
│        エディタ時（オプション）                   │
├─────────────────────────────────────────────────┤
│                                                  │
│  Python Editor Scripts (ue5/EditorScripts/)     │
│  ├─ import_gvrm_asset.py     ← エディタ内実行   │
│  └─ setup_project.py                            │
│                                                  │
└─────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────┐
│      ランタイム（ゲーム実行中）                   │
├─────────────────────────────────────────────────┤
│                                                  │
│  C++ Plugin (GVRMRuntime)    ← C++のみ          │
│  ├─ NiagaraDataInterfaceGVRM.cpp                │
│  ├─ GVRMActor.cpp                               │
│  └─ GVRMComponent.cpp                           │
│                                                  │
│  Blueprints                   ← Blueprintのみ   │
│  └─ BP_GVRMActor                                │
│                                                  │
└─────────────────────────────────────────────────┘
```

---

## 推奨ワークフロー

### パターンA: オフライン変換（シンプル）

**開発者のマシンで:**
```bash
# 1. GVRMをCSVに変換（Python）
python ue5/Tools/gvrm_to_ue5.py assets/author.gvrm -o ue5/Import/

# 2. UE5エディタを開く
# 3. CSVファイルをData Tableとしてインポート（手動 or エディタスクリプト）
# 4. Data AssetとしてUE5プロジェクトに保存
```

**結果:**
- `.uasset`ファイルとして保存される
- ランタイムで使用可能（Pythonは不要）

---

### パターンB: エディタスクリプト統合（高度）

**エディタ内で自動化:**
```python
# ue5/EditorScripts/import_gvrm.py
import unreal
import sys
import os

# Pythonツールへのパスを追加
sys.path.append(os.path.join(unreal.Paths.project_dir(), 'Tools'))
from gvrm_converter import GVRMConverter

@unreal.uclass()
class GVRMImporter(unreal.AssetActionUtility):
    
    @unreal.ufunction(params=[str])
    def import_gvrm_file(self, gvrm_path):
        """エディタ内でGVRMファイルをインポート"""
        
        # 1. Pythonツールで変換
        converter = GVRMConverter(gvrm_path, "/Temp")
        csv_path, metadata_path = converter.convert()
        
        # 2. UE5にインポート
        data_table = self._create_data_table_from_csv(csv_path)
        
        # 3. Data Assetを作成
        binding_data = self._create_binding_data_asset(data_table, metadata_path)
        
        unreal.log(f"Imported GVRM: {binding_data.get_path_name()}")
    
    def _create_data_table_from_csv(self, csv_path):
        # CSV → Data Table変換ロジック
        pass
```

**使用方法:**
```
UE5エディタ → Content Browser → 右クリック → Scripted Actions → Import GVRM
```

---

## 具体的なファイル構成

### ue5/Tools/ (オフラインツール)

```python
# ue5/Tools/gvrm_to_ue5.py
#!/usr/bin/env python3
"""
オフライン変換ツール
UE5の外で実行、CSVとJSONを生成
"""
import zipfile
import json
import csv

class GVRMConverter:
    def convert(self, gvrm_path, output_dir):
        # GVRMファイルを解析
        # CSVとJSONを出力
        pass

if __name__ == "__main__":
    # コマンドラインで実行
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('gvrm_file')
    parser.add_argument('-o', '--output')
    args = parser.parse_args()
    
    converter = GVRMConverter()
    converter.convert(args.gvrm_file, args.output)
```

**実行:**
```bash
# UE5の外で実行
python ue5/Tools/gvrm_to_ue5.py assets/author.gvrm -o output/
```

---

### ue5/EditorScripts/ (エディタスクリプト - オプション)

```python
# ue5/EditorScripts/import_gvrm_editor.py
"""
UE5エディタ内で実行
Content Browserからワンクリックでインポート
"""
import unreal

def import_gvrm_to_project(gvrm_path, destination_path="/Game/GVRM"):
    """
    エディタ内でGVRMをインポート
    
    Args:
        gvrm_path: GVRMファイルのパス
        destination_path: インポート先のUE5パス
    """
    
    # 1. 一時ディレクトリに変換
    import tempfile
    import sys
    import os
    
    temp_dir = tempfile.mkdtemp()
    
    # Pythonツールを使用
    tools_path = os.path.join(unreal.Paths.project_dir(), 'Tools')
    sys.path.append(tools_path)
    
    from gvrm_to_ue5 import GVRMConverter
    converter = GVRMConverter(gvrm_path, temp_dir)
    converter.convert()
    
    # 2. Data Tableを作成
    # 3. Data Assetを作成
    # 4. VRM, PLYをインポート
    
    unreal.log("GVRM import complete!")

# エディタのPythonコンソールから実行:
# import_gvrm_to_project("C:/path/to/avatar.gvrm")
```

**実行方法:**
```
UE5エディタ → Output Log → Python タブ
>>> exec(open('EditorScripts/import_gvrm_editor.py').read())
>>> import_gvrm_to_project("C:/path/to/avatar.gvrm")
```

---

## C++プラグイン (ランタイム)

```cpp
// ue5/Plugins/GVRMRuntime/Source/GVRMRuntime/Private/GVRMActor.cpp
// ランタイムで実行されるコード（Pythonは一切使わない）

#include "GVRMActor.h"

void AGVRMActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Data Assetからバインディング情報を読み込み
    // （既にPythonで変換されて.uassetになっている）
    if (BindingData)
    {
        // C++でスプラットを初期化
        InitializeSplats(BindingData);
    }
}

void AGVRMActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // C++でアニメーション更新
    // Pythonは使わない
    UpdateSplatPositions();
}
```

---

## まとめ

| 用途 | 使用言語 | 実行タイミング | 必須/オプション |
|------|---------|--------------|----------------|
| **GVRMファイル変換** | Python | オフライン | **必須** |
| **エディタ自動化** | Python | エディタ内 | オプション |
| **プラグイン本体** | C++ | ランタイム | **必須** |
| **ゲームロジック** | C++/Blueprint | ランタイム | **必須** |

### 推奨アプローチ

**最もシンプル:**
1. ✅ **Pythonツール（オフライン）** でGVRMを変換
2. ✅ 手動でCSVをUE5にインポート → Data Asset作成
3. ✅ **C++プラグイン（ランタイム）** でData Assetを読み込み

**オプションで追加:**
- エディタスクリプトでインポート自動化（便利だが必須ではない）

### 重要ポイント

**Pythonは変換ツールとしてのみ使用:**
- ✅ 開発時にGVRMファイルをCSV/JSONに変換
- ✅ エディタでのアセットインポート自動化（オプション）
- ❌ ゲーム実行中は一切使わない（C++のみ）

**配布されるプラグインには:**
- C++コンパイル済みバイナリのみ
- Pythonは含まれない
- エンドユーザーはPython不要

---

## 実装計画の修正

**Phase 2: データパイプライン**
- ✅ Pythonツール (`gvrm_to_ue5.py`) を作成
- ✅ オフラインで実行してCSV/JSON生成
- ✅ エディタでData Assetとして保存

**Phase 3-5: ランタイム実装**
- ✅ 完全にC++で実装
- ✅ Pythonは不要
- ✅ Data Assetを読み込んで使用

これでクリアになりましたか？
