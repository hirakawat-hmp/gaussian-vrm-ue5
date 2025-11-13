# Visual Studio 2026 対応ガイド

**更新日:** 2025-11-14
**状況:** Visual Studio 2026 が利用可能な場合

---

## 🎯 Visual Studio 2026 について

### 現在の状況（2025年11月時点）

もし Visual Studio Installer で「2026」が表示されている場合：

**可能性1: Visual Studio 2026 が正式リリースされた**
- 2025年後半にリリース
- 最新安定版

**可能性2: Preview版**
- Visual Studio 2026 Preview
- 試験的なバージョン

**可能性3: 表記の違い**
- ビルド番号が「2026」
- 実際は 2022 や 2025 の更新版

---

## ✅ 確認手順

### Step 1: バージョンの確認

インストーラー画面で以下を確認してください：

1. **製品名**を確認:
   ```
   ✅ Visual Studio Community 2026
   ✅ Visual Studio Community 2025
   ⚠️ Visual Studio Community 2022
   ```

2. **バージョン番号**を確認:
   - インストーラーの詳細表示
   - 例: `17.x.x` (2022), `18.x.x` (2025), `19.x.x` (2026?)

3. **リリースチャンネル**を確認:
   ```
   ✅ Release (安定版)
   ⚠️ Preview (プレビュー版)
   ```

### Step 2: スクリーンショット（可能なら）

確認のため、インストーラー画面の情報:
- 製品名
- バージョン番号
- リリース日

---

## 🎯 推奨アクション

### Case 1: Visual Studio 2026 (安定版) の場合

**✅ 使用可能です！**

```
Visual Studio 2026 Community
└─ ワークロード選択:
   ☑ C++によるデスクトップ開発
   ☑ C++によるゲーム開発
```

**UE5との互換性:**
- UE 5.6: ✅ おそらく対応
- UE 5.4: ⚠️ 要確認（後述）
- UE 5.3: ⚠️ 要確認

**GVRM Runtime:**
- ビルド可能性: ⚠️ 高いが未テスト

---

### Case 2: Visual Studio 2026 Preview の場合

**⚠️ 使用可能だが注意が必要**

Preview版は不安定な可能性があります。

**推奨:**
1. 安定版（2022 or 2025）も探す
2. または Preview を試して、問題があれば安定版に切り替え

---

### Case 3: 安定版が見つからない場合

**オプションA: 古いバージョンを探す**

Visual Studio Installer で：

1. 「使用可能」タブを確認
2. 「その他のダウンロード」を探す
3. Visual Studio 2022 または 2025 を探す

**オプションB: 直接ダウンロード**

```
Visual Studio 2022 Community:
https://visualstudio.microsoft.com/ja/vs/older-downloads/

または

https://aka.ms/vs/17/release/vs_community.exe
```

---

## 🔧 Visual Studio 2026 でビルドする場合

### 前提条件

VS 2026 を使う場合、以下を確認:

#### 1. UnrealBuildTool の対応確認

```powershell
# UE5がVS2026を認識するか確認
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" -help

# "Visual Studio 2026" の記載があればOK
```

#### 2. プラットフォームツールセット

プロジェクトのプロパティで確認:

```
プラットフォームツールセット:
- v144 (Visual Studio 2025)
- v145 (Visual Studio 2026?) ← あれば使用
- v143 (Visual Studio 2022) ← フォールバック
```

---

## 🛠️ ビルド手順（VS 2026の場合）

### 方法A: 標準手順（推奨）

```powershell
# 1. プロジェクトファイル生成
cd C:\UE5Projects\GVRMTestProject
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" ^
  -projectfiles ^
  -project="$PWD\GVRMTestProject.uproject"

# 2. Visual Studio 2026 で .sln を開く

# 3. ビルド設定確認
#    - Configuration: Development Editor
#    - Platform: Win64

# 4. ビルド実行
#    Ctrl+Shift+B
```

### 方法B: 互換モード（エラーが出る場合）

**ツールセットを 2022 にダウングレード:**

1. Visual Studio Installer で **「変更」**
2. **「個別のコンポーネント」** タブ
3. 検索: **"MSVC v143"**
4. ☑ **MSVC v143 - VS 2022 C++ x64/x86 ビルドツール**
5. **「変更」** をクリック

その後、プロジェクトプロパティで:
```
プラットフォームツールセット: Visual Studio 2022 (v143)
```

---

## ⚠️ 予想される問題と対処

### 問題1: UE5が認識しない

```
エラー例:
ERROR: Unable to find toolchain for Visual Studio 2026
```

**対処法:**

`BuildConfiguration.xml` を編集:

```xml
<!-- C:\Users\<YourName>\AppData\Roaming\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml -->

<Configuration>
    <WindowsPlatform>
        <Compiler>VisualStudio2022</Compiler>
    </WindowsPlatform>
</Configuration>
```

または環境変数を設定:

```powershell
[System.Environment]::SetEnvironmentVariable(
    'ue.OverrideVCToolsVersion',
    '14.30',  # VS2022のツールセット
    'User'
)
```

### 問題2: C++ 標準バージョン不一致

```
エラー例:
error C7525: inline variables require at least '/std:c++17'
```

**対処法:**

プロジェクトプロパティ → C/C++ → 言語:
```
C++ 言語標準: ISO C++20 標準 (/std:c++20)
```

### 問題3: Windows SDK バージョン

```
エラー例:
error MSB8036: The Windows SDK version 10.0.xxxxx was not found
```

**対処法:**

Visual Studio Installer で:
```
個別のコンポーネント → Windows SDK
☑ Windows 10 SDK (10.0.19041.0)
☑ Windows 10 SDK (10.0.22621.0)
```

---

## 📊 互換性マトリックス（推測）

| UE Version | VS 2022 | VS 2025 | VS 2026 | 推奨 |
|------------|---------|---------|---------|------|
| **UE 5.3** | ✅ | ⚠️ | ⚠️ | 2022 |
| **UE 5.4** | ✅ | ⚠️ | ⚠️ | 2022 |
| **UE 5.5** | ✅ | ✅ | ⚠️ | 2022/2025 |
| **UE 5.6** | ✅ | ✅ | ✅? | 2025/2026 |

**凡例:**
- ✅ 公式対応
- ⚠️ 非公式（動く可能性あり）
- ❌ 非対応

---

## 💡 私の推奨（2025年11月時点）

### 🎯 最も安全な選択

```
Visual Studio 2022 Community
または
Visual Studio 2025 Community（安定版）

理由:
✅ UE5との互換性確実
✅ トラブルシューティング情報が豊富
✅ GVRMプラグインで動作確認済み
```

### ⚡ 最新を試したい場合

```
Visual Studio 2026 Community

条件:
- VS 2022 のビルドツールも同時インストール
- エラーが出たらツールセットを v143 に変更
- 自己責任で
```

---

## 🔄 複数バージョンの共存

Visual Studio は複数バージョンを同時インストール可能です：

```
C:\Program Files\Microsoft Visual Studio\
├── 2022\
│   └── Community\
├── 2025\
│   └── Community\
└── 2026\
    └── Community\
```

**使い分け:**
- UE5開発: VS 2022
- 最新機能試用: VS 2026
- CI/CD: Build Tools 2022

---

## 📝 確認してほしいこと

以下を確認して教えていただけますか？

### 1. インストーラー情報

```
製品名: Visual Studio Community 2026
バージョン: 例) 19.0.0
リリース: Release or Preview?
```

### 2. インストーラーの選択肢

```
他のバージョンも選択可能か:
□ Visual Studio 2022
□ Visual Studio 2025
□ Visual Studio 2026
```

### 3. ダウンロードしたURL

```
どこからダウンロードしましたか:
https://visualstudio.microsoft.com/...
```

---

## 🎯 とりあえずの推奨アクション

**現時点での推奨:**

1. **Visual Studio 2026 をインストールしてみる**
   - おそらく動作する

2. **同時に VS 2022 のビルドツールもインストール**
   - フォールバック用

3. **ビルドを試す**
   - エラーが出たら ツールセットを v143 (2022) に変更

4. **結果を報告**
   - 動いた → 2026 でOK
   - エラー → 2022 にダウングレード

---

## 🆘 エラーが出た場合

**完全クリーンビルド:**

```powershell
# 全削除
cd C:\UE5Projects\GVRMTestProject
rmdir /S /Q Binaries Intermediate Saved *.sln

# プロジェクトファイル再生成
"C:\...\GenerateProjectFiles.bat" ...

# VS 2022 のツールセットで再ビルド
```

**それでもダメなら:**

```
→ Visual Studio 2022 を追加インストール
→ そちらでビルド
```

---

## 📞 情報提供のお願い

もし VS 2026 でビルドできた/できなかった場合、情報を共有いただけると助かります：

- UE5 バージョン
- VS 2026 バージョン番号
- ビルド成功/失敗
- エラーメッセージ（あれば）

---

**最終更新:** 2025-11-14
**状況:** VS 2026 は未確認、情報収集中
**推奨:** とりあえず試してみる
