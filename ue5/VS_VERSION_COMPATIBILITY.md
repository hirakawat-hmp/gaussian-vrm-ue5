# Visual Studio バージョン互換性ガイド

## 🎯 推奨バージョン

### ✅ Visual Studio 2022（最も安定）

**対応状況:**
- UE 5.3: ✅ 完全対応
- UE 5.4: ✅ 完全対応
- UE 5.6: ✅ 完全対応
- GVRM Runtime: ✅ テスト済み

**ダウンロード:**
```
https://visualstudio.microsoft.com/ja/vs/
```

**推奨エディション:**
- Community（無料）で十分
- Professional / Enterprise でも可

**必須コンポーネント:**
- C++によるデスクトップ開発
- C++によるゲーム開発（推奨）
- MSVC v143 - VS 2022 C++ ビルドツール
- Windows 10 SDK (10.0.19041.0 以降)

---

## ⚠️ Visual Studio 2025（使用可能だが注意）

**リリース状況:**
- Preview版: 2024年11月リリース
- 安定版: 未リリース（2025年予定）

**対応状況:**
- UE 5.3: ⚠️ 非公式（動くかも）
- UE 5.4: ⚠️ 非公式（動くかも）
- UE 5.6: ✅ 対応予定
- GVRM Runtime: ⚠️ 未テスト

### VS2025 で試す場合の手順

#### 1. 互換性モードの設定

UE5プロジェクトは通常VS2022向けに生成されるため、VS2025でビルドする場合は設定が必要です。

**方法A: プロジェクト設定を変更**

`GVRMTestProject.uproject` をテキストエディタで開き、以下を追加:

```json
{
    "EngineAssociation": "5.4",
    "Category": "",
    "Description": "",
    "Modules": [
        {
            "Name": "GVRMTestProject",
            "Type": "Runtime",
            "LoadingPhase": "Default"
        }
    ],
    "Plugins": [
        {
            "Name": "Niagara",
            "Enabled": true
        }
    ],
    "TargetPlatforms": [
        "Windows"
    ],
    "EpicSampleNameHash": "0"
}
```

**方法B: ビルド設定ファイルを編集**

`Source/GVRMTestProject.Target.cs` を開き、以下を追加:

```csharp
public class GVRMTestProjectTarget : TargetRules
{
    public GVRMTestProjectTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4; // または V5
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange(new string[] { "GVRMTestProject" });

        // VS2025 用の設定（必要に応じて）
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            WindowsPlatform.Compiler = WindowsCompiler.VisualStudio2022; // 2025はまだ無い
        }
    }
}
```

#### 2. Platform Toolset の確認

VS2025で開いた場合、プロジェクトのプロパティで以下を確認:

1. プロジェクトを右クリック → **プロパティ**
2. **構成プロパティ** → **全般**
3. **プラットフォーム ツールセット**:
   ```
   推奨: Visual Studio 2022 (v143)
   使用可能: Visual Studio 2025 (v144) ← あれば
   ```

#### 3. ビルドエラーが出た場合

**エラー: C++20 機能が使えない**
```
プロジェクトプロパティ → C/C++ → 言語
C++ 言語標準: ISO C++20 標準 (/std:c++20)
```

**エラー: Windows SDK バージョン不一致**
```
プロジェクトプロパティ → 全般
Windows SDK バージョン: 10.0.19041.0 以降を選択
```

**エラー: MSVC ツールチェーン未検出**
```
Visual Studio Installer で以下をインストール:
- MSVC v143 (VS2022) または v144 (VS2025)
- Windows 10 SDK
```

---

## ❌ Visual Studio 2019（非推奨）

**対応状況:**
- UE 5.0-5.2: ✅ 対応
- UE 5.3以降: ❌ 非推奨
- GVRM Runtime: ❌ 未対応

**理由:**
- C++17/20 機能のサポートが不完全
- UE5.3以降は VS2022 を前提に最適化
- ビルドエラーが発生する可能性大

**VS2019 しか無い場合:**

1. **UE 5.2 以前を使う**（推奨しない）
2. **VS2022 にアップグレード**（推奨）

---

## 📊 バージョン比較表

| VS Version | UE 5.3 | UE 5.4 | UE 5.6 | GVRM | 推奨度 |
|------------|--------|--------|--------|------|--------|
| **VS 2022** | ✅ | ✅ | ✅ | ✅ | ⭐⭐⭐⭐⭐ |
| **VS 2025** | ⚠️ | ⚠️ | ✅ | ⚠️ | ⭐⭐⭐ |
| VS 2019 | ⚠️ | ❌ | ❌ | ❌ | ⭐ |
| VS 2017 | ❌ | ❌ | ❌ | ❌ | ❌ |

---

## 🔧 VS2025 で発生しうる問題

### 問題1: プロジェクトファイル生成エラー

```
エラー例:
ERROR: Unable to find toolchain for Visual Studio 2025
```

**原因:** UE5がVS2025を認識していない

**対処法:**

```powershell
# UnrealBuildTool の設定ファイルを編集
# C:\Users\<YourName>\AppData\Roaming\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml

<Configuration>
    <WindowsPlatform>
        <Compiler>VisualStudio2022</Compiler>
    </WindowsPlatform>
</Configuration>
```

### 問題2: ビルドツールバージョン不一致

```
エラー例:
error MSB8020: The build tools for v144 cannot be found
```

**対処法:**

Visual Studio Installer で **VS2022 ビルドツール**も追加インストール:

1. Visual Studio Installer 起動
2. VS2025 の **「変更」**
3. **「個別のコンポーネント」** タブ
4. 検索: **"MSVC v143"**
5. ☑ **MSVC v143 - VS 2022 C++ x64/x86 ビルドツール**
6. **「変更」** をクリックしてインストール

### 問題3: C++ 標準ライブラリの違い

VS2025は新しいC++標準ライブラリを使用する場合があり、UE5との互換性問題が出る可能性があります。

**対処法:**

プロジェクトプロパティで明示的に指定:

```
C/C++ → コード生成 → ランタイム ライブラリ
→ マルチスレッド DLL (/MD) [Release]
→ マルチスレッド デバッグ DLL (/MDd) [Debug]
```

---

## 💡 推奨アプローチ

### 🎯 初心者の方

**→ Visual Studio 2022 を使用してください**

理由:
- UE5との完全互換性保証
- トラブルシューティング情報が豊富
- 公式ドキュメントがVS2022ベース

### 🧪 上級者で VS2025 を試したい方

**→ VS2022 と VS2025 を両方インストール**

インストール構成:
```
C:\Program Files\Microsoft Visual Studio\
├── 2022\           ← メイン開発用
│   └── Community\
└── 2025\           ← 実験用
    └── Preview\
```

切り替え方法:
1. `.sln` ファイルを右クリック
2. **「プログラムから開く」**
3. 使用するVSバージョンを選択

### 🏢 既にVS2025を使っている企業

**→ 互換モードを試す**

1. VS2025 でプロジェクトを開く
2. プラットフォームツールセットを **v143** に変更
3. ビルドしてみる
4. エラーが出たら上記対処法を試す

---

## ✅ 動作確認手順（VS2025の場合）

### 1. 簡易テスト

```powershell
# VS2025 のコマンドプロンプトを開く
"C:\Program Files\Microsoft Visual Studio\2025\Preview\Common7\Tools\VsDevCmd.bat"

# UE5 ビルドツールを実行
cd C:\UE5Projects\GVRMTestProject
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" ^
  GVRMTestProject Win64 Development

# エラーが出なければOK
```

### 2. プラグイン個別ビルド

```powershell
# GVRM Runtime だけビルドしてみる
cd C:\UE5Projects\GVRMTestProject\Plugins\GVRMRuntime

# .uplugin ファイルを右クリック
# → "Package Plugin" で単体ビルドテスト
```

### 3. 完全ビルド

```
Visual Studio で普通にビルド
→ エラーなく完了すればVS2025でもOK
```

---

## 🆘 どうしてもVS2025でエラーが出る場合

### オプション1: VS2022を追加インストール（推奨）

VS2025とVS2022は共存できます。

```powershell
# VS2022 をダウンロード
https://visualstudio.microsoft.com/ja/downloads/

# インストール後、.sln を VS2022 で開く
```

### オプション2: UE5.6 を待つ

UE5.6（2025年リリース予定）は正式にVS2025をサポートする可能性が高いです。

### オプション3: Docker/仮想環境

Windows Server Container でVS2022環境を構築（上級者向け）

---

## 📌 まとめ

| 状況 | 推奨 |
|------|------|
| **これから環境構築する** | VS 2022 |
| **既にVS2025がある** | VS2022も追加 |
| **VS2025だけで試したい** | 可能だが自己責任 |
| **企業で統一が必要** | VS 2022 推奨 |

---

## 🔗 参考リンク

- **UE5 Visual Studio サポート:**
  https://docs.unrealengine.com/5.4/setting-up-visual-studio-for-unreal-engine/

- **VS2022 ダウンロード:**
  https://visualstudio.microsoft.com/ja/downloads/

- **VS2025 Preview:**
  https://visualstudio.microsoft.com/ja/vs/preview/

---

**最終更新:** 2025-11-14
**推奨:** Visual Studio 2022
**試験的対応:** Visual Studio 2025 Preview
