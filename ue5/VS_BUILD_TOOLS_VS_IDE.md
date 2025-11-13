# Visual Studio: Build Tools vs IDE の違い

## 🎯 結論（先に）

**UE5プラグイン開発の場合:**

| 用途 | Build Tools | フルIDE | 推奨 |
|------|-------------|---------|------|
| **初心者** | ❌ | ✅ | IDE |
| **デバッグしたい** | ❌ | ✅ | IDE |
| **CI/CD自動化** | ✅ | △ | Build Tools |
| **コマンドライン派** | ✅ | ✅ | どちらでも |

**→ 初めてなら IDE（フル版）を使ってください！**

---

## 📦 2つの違い

### Visual Studio 2022 (IDE) - フル版

**含まれるもの:**
```
✅ ビルドツール（コンパイラ、リンカー）
✅ 統合開発環境（IDE）
✅ デバッガー（GUI）
✅ IntelliSense（コード補完）
✅ ソリューションエクスプローラー
✅ エラーリスト表示
✅ Git統合
✅ プロジェクト管理UI
```

**サイズ:** 約 10-15 GB

**価格:**
- Community: **無料**（個人・小規模チーム）
- Professional: 有料
- Enterprise: 有料

**ダウンロード:**
```
https://visualstudio.microsoft.com/ja/downloads/
→ 「Visual Studio 2022 Community」
```

---

### Visual Studio Build Tools 2022

**含まれるもの:**
```
✅ ビルドツール（コンパイラ、リンカー）
✅ MSBuild
✅ コマンドラインツール
❌ IDE（エディタなし）
❌ GUIデバッガー
❌ IntelliSense（無し）
❌ ソリューションエクスプローラー
```

**サイズ:** 約 3-5 GB

**価格:** 完全無料

**ダウンロード:**
```
https://visualstudio.microsoft.com/ja/downloads/#build-tools-for-visual-studio-2022
→ 「Build Tools for Visual Studio 2022」
```

---

## 💡 Build Tools でできること・できないこと

### ✅ できること

#### 1. コマンドラインビルド

```powershell
# UE5プロジェクトをビルド
cd C:\UE5Projects\GVRMTestProject

# UnrealBuildTool経由でビルド
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" ^
  GVRMTestProject Win64 Development

# または MSBuild を直接使用
"C:\Program Files\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" ^
  GVRMTestProject.sln ^
  /p:Configuration="Development Editor" ^
  /p:Platform=Win64
```

#### 2. CI/CD パイプライン

```yaml
# GitHub Actions の例
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: microsoft/setup-msbuild@v1
      - name: Build UE5 Plugin
        run: |
          "C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" ...
```

#### 3. 自動ビルドスクリプト

```powershell
# build.ps1
param(
    [string]$Config = "Development Editor"
)

$UE5Path = "C:\Program Files\Epic Games\UE_5.4"
$ProjectPath = "C:\UE5Projects\GVRMTestProject"

& "$UE5Path\Engine\Build\BatchFiles\Build.bat" `
  GVRMTestProject Win64 $Config

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Build succeeded" -ForegroundColor Green
} else {
    Write-Host "❌ Build failed" -ForegroundColor Red
}
```

### ❌ できないこと（制限）

#### 1. GUIでのビルド

Build Tools には IDE がないため、以下ができません：

```
❌ Visual Studio でソリューションを開く
❌ ソリューションエクスプローラーで確認
❌ エラーリストをGUIで見る
❌ IntelliSense（コード補完）
```

**対処法:**
- VS Codeなど別のエディタを使う
- コマンドラインでエラーログを見る

#### 2. GUIデバッグ

```
❌ ブレークポイントを設定
❌ ステップ実行
❌ 変数ウォッチ
❌ コールスタック表示（GUI）
```

**対処法:**
- UE5エディタのデバッグ機能を使う
- `UE_LOG` でログ出力
- WinDbg（コマンドラインデバッガー）を使う（上級者）

#### 3. プロジェクトファイル生成

```
❌ .uproject 右クリック → "Generate VS project files"
   （このメニューが出ない）
```

**対処法:**
```powershell
# コマンドラインで生成
cd C:\UE5Projects\GVRMTestProject

"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
  -projectfiles `
  -project="C:\UE5Projects\GVRMTestProject\GVRMTestProject.uproject" `
  -game -engine
```

---

## 🔧 Build Tools を使う場合のワークフロー

### 推奨構成: Build Tools + VS Code

**組み合わせ:**
```
ビルド: Visual Studio Build Tools 2022
編集: Visual Studio Code + C++ Extension
デバッグ: UE5 Editor の機能
```

### セットアップ手順

#### 1. Build Tools インストール

1. ダウンロード:
   ```
   https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   ```

2. インストーラー実行

3. ワークロード選択:
   ```
   ☑ C++ Build Tools
     ├─ MSVC v143 - VS 2022 C++ x64/x86
     ├─ Windows 10 SDK (10.0.19041.0 以降)
     └─ C++ CMake tools for Windows
   ```

4. インストール（30-60分）

#### 2. VS Code インストール

1. ダウンロード:
   ```
   https://code.visualstudio.com/
   ```

2. 拡張機能をインストール:
   ```
   - C/C++ (Microsoft)
   - C++ IntelliSense
   - Unreal Engine 4 Snippets (UE5でも使える)
   ```

#### 3. プロジェクトビルド

```powershell
# Developer Command Prompt for VS 2022 を開く
# （スタートメニューに追加される）

cd C:\UE5Projects\GVRMTestProject

# プロジェクトファイル生成
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" `
  -projectfiles `
  -project="$PWD\GVRMTestProject.uproject"

# ビルド
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" `
  GVRMTestProject Win64 "Development Editor"
```

#### 4. VS Code で編集

```powershell
# VS Code でプロジェクトを開く
code C:\UE5Projects\GVRMTestProject
```

`.vscode/tasks.json` を作成:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build UE5 Project",
            "type": "shell",
            "command": "C:\\Program Files\\Epic Games\\UE_5.4\\Engine\\Build\\BatchFiles\\Build.bat",
            "args": [
                "GVRMTestProject",
                "Win64",
                "Development Editor"
            ],
            "problemMatcher": "$msCompile",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```

これで **Ctrl+Shift+B** でビルドできます。

---

## 📊 比較表：どちらを選ぶべきか

| 項目 | Build Tools | フルIDE |
|------|-------------|---------|
| **ビルド速度** | 同じ | 同じ |
| **ディスク容量** | 3-5 GB | 10-15 GB |
| **初心者向け** | ❌ | ✅ |
| **GUI操作** | ❌ | ✅ |
| **デバッグ** | △ | ✅ |
| **コード編集** | ❌ (別エディタ) | ✅ |
| **エラー表示** | コマンドライン | GUI |
| **CI/CD** | ✅ | △ |
| **価格** | 無料 | 無料 (Community) |

---

## 🎯 ケース別推奨

### Case 1: 初めてUE5プラグイン開発する

**推奨:** フルIDE（Visual Studio 2022 Community）

**理由:**
- ✅ GUIで直感的
- ✅ エラーが見やすい
- ✅ デバッグが簡単
- ✅ チュートリアルがIDE前提

**初心者ガイド:**
```
→ BUILD_GUIDE_BEGINNER.md
  （フルIDE前提で書かれています）
```

---

### Case 2: コマンドラインが得意、ディスク容量節約したい

**推奨:** Build Tools + VS Code

**理由:**
- ✅ 軽量（容量節約）
- ✅ コマンドラインで完結
- ✅ VS Codeで快適に編集

**セットアップ:**
1. Build Tools インストール（上記参照）
2. VS Code + C++ Extension
3. コマンドラインビルド

---

### Case 3: CI/CD環境構築（サーバー）

**推奨:** Build Tools のみ

**理由:**
- ✅ GUIが不要
- ✅ ライセンス制限なし
- ✅ 自動化に最適

**例: GitHub Actions**
```yaml
- name: Install Build Tools
  run: |
    choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools"
```

---

### Case 4: 既にVS Codeユーザー

**推奨:** Build Tools + VS Code（継続）

**理由:**
- ✅ 慣れたエディタが使える
- ✅ フルIDEは重いと感じる
- ✅ ビルドはコマンドラインで問題ない

---

## ⚠️ Build Tools の落とし穴

### 問題1: UE5エディタからの統合ができない

UE5エディタの「Open in Visual Studio」ボタンが使えません。

**対処:**
- VS Codeで開く設定にする
- または手動でエディタを開く

### 問題2: プロジェクトファイル生成が不便

右クリックメニューが使えません。

**対処:**
- コマンドラインで生成（上記参照）
- バッチファイルを作成:

```batch
@echo off
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" ^
  -projectfiles ^
  -project="%CD%\GVRMTestProject.uproject"
pause
```

### 問題3: エラー解析が面倒

GUIでエラーリストが見れません。

**対処:**
- ビルドログを保存して確認:

```powershell
& "C:\...\Build.bat" ... 2>&1 | Tee-Object build.log
# build.log をエディタで開く
```

---

## 💰 価格の違い

### Visual Studio 2022 Community（フルIDE）

**無料条件:**
- 個人開発者
- オープンソースプロジェクト
- 教育目的
- 小規模チーム（5人以下）

**制限:**
- エンタープライズでは使用不可

### Build Tools 2022

**完全無料:**
- 個人・企業問わず
- 人数制限なし
- 商用利用OK

---

## 🔄 後から切り替えも可能

### Build Tools → フルIDEに変更

```
1. Visual Studio 2022 Community をインストール
2. 既存のプロジェクトをそのまま開く
3. 問題なくビルド可能
```

### フルIDE → Build Tools に変更

```
1. Visual Studio をアンインストール
2. Build Tools をインストール
3. コマンドラインでビルド
```

**注意:** プロジェクトファイルは共通なので、データは失われません。

---

## 📝 私の推奨（まとめ）

### あなたの状況は？

**Q1: UE5プラグイン開発は初めて？**
```
YES → Visual Studio 2022 Community (フルIDE)
NO  → Q2へ
```

**Q2: コマンドライン操作に慣れている？**
```
YES → Build Tools + VS Code でもOK
NO  → Visual Studio 2022 Community (フルIDE)
```

**Q3: ディスク容量が厳しい？（20GB未満）**
```
YES → Build Tools を試す（ただし難易度高）
NO  → Visual Studio 2022 Community (フルIDE)
```

**Q4: CI/CD環境を作りたい？**
```
YES → Build Tools
NO  → Visual Studio 2022 Community (フルIDE)
```

---

## 🎯 最終結論

| 対象者 | 推奨 | 理由 |
|--------|------|------|
| **初心者** | フルIDE | GUI、デバッグ、簡単 |
| **中級者** | フルIDE | デバッグが便利 |
| **上級者** | どちらでも | 好みで選んで |
| **CI/CD** | Build Tools | 軽量、自動化向き |

**迷ったら:**
```
→ Visual Studio 2022 Community
  （フルIDE、無料）
```

---

## 🔗 ダウンロードリンク

### Visual Studio 2022 Community（フルIDE）
```
https://visualstudio.microsoft.com/ja/downloads/
→ 一番上の「Community 2022」
```

### Build Tools for Visual Studio 2022
```
https://visualstudio.microsoft.com/ja/downloads/
→ 下にスクロール
→ 「すべてのダウンロード」展開
→ 「Tools for Visual Studio」セクション
→ 「Build Tools for Visual Studio 2022」
```

---

**最終更新:** 2025-11-14
**推奨:** 初心者なら フルIDE
**上級者:** Build Tools でもOK
