# UE5 GVRM開発戦略 - レポジトリ構成と開発フロー

## アプローチの比較

### オプション1: このレポジトリ内で実装（推奨 - プロトタイプ段階）

```
gaussian-vrm/
├── gvrm-format/           # 既存: Three.js実装
├── apps/                  # 既存: Web applications
├── lib/                   # 既存: ライブラリ
├── assets/                # 既存: サンプルデータ
├── ue5/                   # 新規: UE5実装
│   ├── Plugins/
│   │   └── GVRMRuntime/
│   │       ├── Source/
│   │       ├── Content/
│   │       └── GVRMRuntime.uplugin
│   ├── Tools/
│   │   ├── gvrm_to_ue5.py
│   │   └── batch_convert.py
│   └── Samples/
│       └── GVRMDemo/      # サンプルUE5プロジェクト
└── tools/                 # 新規: 共通ツール
    ├── data_validator.py
    └── format_converter.py
```

**利点:**
- ✅ Web実装とUE5実装を並行比較できる
- ✅ 共通のGVRMフォーマット仕様を共有
- ✅ assetsフォルダのサンプルデータを両方で使用
- ✅ 研究プロジェクトとして統一された構成
- ✅ Three.jsのシェーダーコードとUE5のHLSLを並べて比較しやすい

**欠点:**
- ❌ レポジトリサイズが大きくなる
- ❌ Web開発者とUE5開発者で興味範囲が異なる
- ❌ .gitignoreの管理が複雑（.uassetなど）

**適している場合:**
- プロトタイプ開発段階
- 研究・実験的なプロジェクト
- 両実装の比較・検証が重要
- 小〜中規模チーム

---

### オプション2: 別レポジトリで実装（推奨 - プロダクション段階）

```
gaussian-vrm/              # このレポジトリ（Web実装）
└── (既存のまま)

gaussian-vrm-ue5/          # 新規レポジトリ
├── Plugins/
│   └── GVRMRuntime/
├── Content/
├── Source/
├── Tools/
│   └── gvrm_to_ue5.py
├── Samples/
└── Docs/
```

**依存関係の管理:**
```bash
# gaussian-vrm-ue5 に gaussian-vrm をサブモジュールとして追加
cd gaussian-vrm-ue5
git submodule add https://github.com/naruya/gaussian-vrm.git ThirdParty/gaussian-vrm

# サンプルGVRMファイルを参照
ln -s ThirdParty/gaussian-vrm/assets Assets/GVRM/Samples
```

**利点:**
- ✅ 明確な責任分離（Web vs Game Engine）
- ✅ UE5特有の.gitignore、CI/CD設定
- ✅ プラグインとして配布しやすい
- ✅ Unreal Marketplace / Fabへの提出が容易

**欠点:**
- ❌ コードの重複可能性（変換ツールなど）
- ❌ サンプルデータの同期が必要
- ❌ 仕様変更時の同期作業

**適している場合:**
- プロダクション段階
- Marketplace配布を目指す
- 大規模チーム・独立した開発
- CI/CDパイプラインを分けたい

---

### オプション3: モノレポ構造（中間案）

```
gaussian-vrm/
├── packages/
│   ├── web/               # Three.js実装
│   │   ├── gvrm-format/
│   │   ├── apps/
│   │   └── package.json
│   ├── ue5/               # UE5実装
│   │   └── Plugins/
│   └── tools/             # 共通ツール
│       ├── python/
│       └── specs/         # フォーマット仕様
├── assets/                # 共通サンプルデータ
└── lerna.json / pnpm-workspace.yaml
```

**利点:**
- ✅ すべてが1つのレポジトリ
- ✅ 共通ツール・仕様を効率的に共有
- ✅ バージョン管理の一元化

**欠点:**
- ❌ 複雑なビルドシステム
- ❌ 大規模なレポジトリ

**適している場合:**
- 複数プラットフォーム展開を計画
- 統一されたバージョニングが重要

---

## 推奨開発フロー

### フェーズ1: プロトタイプ（このレポジトリ内）

**期間:** 4-6週間

**ディレクトリ構成:**
```bash
# このレポジトリに追加
mkdir -p ue5/Plugins/GVRMRuntime
mkdir -p ue5/Tools
mkdir -p ue5/Samples
```

**開発手順:**
```bash
# 1. ブランチを切る
git checkout -b feature/ue5-implementation

# 2. .gitignoreに追加
cat >> .gitignore << EOF
# Unreal Engine
ue5/Samples/**/*.uasset
ue5/Samples/**/*.umap
ue5/Samples/Intermediate/
ue5/Samples/Saved/
ue5/Samples/Binaries/
ue5/Plugins/GVRMRuntime/Intermediate/
ue5/Plugins/GVRMRuntime/Binaries/
EOF

# 3. プラグイン構造を作成
cd ue5/Plugins/GVRMRuntime
# (プラグインファイルを作成)

# 4. 定期的にコミット
git add ue5/
git commit -m "Add UE5 prototype implementation"
```

**マイルストーン:**
- Week 1-2: 基本NDI実装 → Web版と動作比較
- Week 3-4: スキニング実装 → gvrm.jsと結果比較
- Week 5-6: 統合テスト → 同じGVRMファイルで動作確認

---

### フェーズ2: プロダクション（別レポジトリに移行）

**期間:** フェーズ1完了後

**移行手順:**
```bash
# 1. 新規レポジトリ作成
mkdir gaussian-vrm-ue5
cd gaussian-vrm-ue5
git init

# 2. UE5ディレクトリを移動
cp -r ../gaussian-vrm/ue5/* .

# 3. gaussian-vrmをサブモジュール化
git submodule add https://github.com/naruya/gaussian-vrm.git ThirdParty/gaussian-vrm

# 4. README、License等を整備
# 5. GitHub Actionsでビルドパイプライン構築
```

**構成:**
```
gaussian-vrm-ue5/
├── Plugins/
│   └── GVRMRuntime/           # プラグイン本体
├── Source/                     # サンプルプロジェクト
├── Content/
├── ThirdParty/
│   └── gaussian-vrm/           # Submodule
├── Tools/
│   ├── gvrm_to_ue5.py
│   └── CI/
│       └── build_plugin.py
├── Docs/
│   ├── Installation.md
│   └── API.md
└── .github/
    └── workflows/
        └── build.yml
```

---

## 共通ツールの管理

### tools/ディレクトリ（このレポジトリ）

**共通ツールを配置:**
```
gaussian-vrm/
└── tools/
    ├── gvrm_validator.py      # GVRMファイル検証
    ├── format_spec.json        # フォーマット仕様
    ├── data_converter/
    │   ├── to_ue5.py
    │   ├── to_unity.py
    │   └── to_blender.py
    └── README.md
```

**UE5レポジトリから参照:**
```bash
# gaussian-vrm-ue5/Tools/
ln -s ../ThirdParty/gaussian-vrm/tools common_tools

# または直接importで使用
import sys
sys.path.append('ThirdParty/gaussian-vrm/tools')
from gvrm_validator import validate_gvrm
```

---

## 開発環境セットアップ

### このレポジトリでUE5開発を始める場合

```bash
# 1. レポジトリをクローン
git clone https://github.com/naruya/gaussian-vrm.git
cd gaussian-vrm

# 2. UE5開発ブランチを作成
git checkout -b feature/ue5-dev

# 3. ディレクトリ作成
mkdir -p ue5/{Plugins/GVRMRuntime,Tools,Samples}

# 4. サンプルUE5プロジェクトを作成
# Unreal Editor で新規プロジェクトを ue5/Samples/GVRMDemo に作成

# 5. プラグインをプロジェクトにリンク
cd ue5/Samples/GVRMDemo/Plugins
ln -s ../../../Plugins/GVRMRuntime GVRMRuntime

# 6. Pythonツールをセットアップ
cd ue5/Tools
python -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

### requirements.txt
```txt
# ue5/Tools/requirements.txt
numpy>=1.24.0
pandas>=2.0.0
Pillow>=10.0.0
```

---

## CI/CDパイプライン

### GitHub Actions（このレポジトリ）

```yaml
# .github/workflows/ue5-build.yml
name: UE5 Plugin Build

on:
  push:
    paths:
      - 'ue5/**'
  pull_request:
    paths:
      - 'ue5/**'

jobs:
  build-plugin:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup Unreal Engine
        uses: UnrealEngine/setup-unreal-engine@v1
        with:
          version: '5.3'
      
      - name: Build Plugin
        run: |
          cd ue5/Plugins/GVRMRuntime
          ../../Tools/build_plugin.bat
      
      - name: Run Tests
        run: |
          cd ue5/Samples/GVRMDemo
          ../../Tools/run_tests.bat
      
      - name: Upload Artifacts
        uses: actions/upload-artifact@v3
        with:
          name: GVRMRuntime-Plugin
          path: ue5/Plugins/GVRMRuntime/Binaries/
```

---

## バージョン管理戦略

### ブランチ戦略

```
main
├── web/stable              # Three.js安定版
├── ue5/prototype           # UE5プロトタイプ
└── ue5/stable              # UE5安定版（フェーズ2以降）
```

### タグ戦略

```bash
# Webリリース
git tag v1.0.0-web

# UE5リリース
git tag v1.0.0-ue5

# 共通フォーマット更新
git tag gvrm-format-v2.0
```

---

## 最終推奨

### 今すぐ始めるなら

**✅ このレポジトリ内で実装（オプション1）**

**理由:**
1. 既存のgvrm.jsとの比較が容易
2. 同じassetsフォルダを使用できる
3. プロトタイプとして迅速に開発
4. 研究プロジェクトとして統一感

**最初のステップ:**
```bash
git checkout -b feature/ue5-implementation
mkdir -p ue5/{Plugins/GVRMRuntime/{Source,Content},Tools,Samples}
# 開発開始！
```

### プロダクション段階になったら

**✅ 別レポジトリに移行（オプション2）**

**タイミング:**
- プロトタイプが安定動作
- Marketplace配布を検討
- 独立したリリースサイクルが必要

**移行は簡単:**
```bash
# ディレクトリごと新レポジトリにコピー
# サブモジュールで元レポジトリを参照
```

---

## まとめ

| フェーズ | アプローチ | メリット |
|---------|----------|---------|
| **今（プロトタイプ）** | このレポジトリ内 | 迅速な開発、比較容易 |
| **後（プロダクション）** | 別レポジトリ | 配布・管理が容易 |

**段階的移行が可能**なので、まずはこのレポジトリ内で`ue5/`ディレクトリを作って始めるのがベストです！
