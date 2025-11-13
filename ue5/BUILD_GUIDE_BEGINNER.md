# GVRM Runtime - 完全初心者向けビルドガイド

**対象者:** UE5プラグイン開発が初めての方
**所要時間:** 2-3時間（初回）
**難易度:** ★★☆☆☆

このガイドでは、ゼロから順番に進めていけば必ずビルドできるように書いています。焦らず1つずつ確認しながら進めてください。

---

## 📋 事前準備チェックリスト

開始前に以下を確認してください：

- [ ] Windows 10 または 11 を使用している
- [ ] ストレージ空き容量が **50GB以上** ある
- [ ] インターネット接続がある
- [ ] 管理者権限でログインしている

**所要時間:** 確認のみ（5分）

---

## Phase 1: 必要なソフトウェアのインストール

### Step 1.1: Visual Studio 2022 のインストール

**所要時間:** 30-60分（ダウンロード含む）

#### 1.1.1 ダウンロード

1. ブラウザで以下にアクセス:
   ```
   https://visualstudio.microsoft.com/ja/downloads/
   ```

2. **Visual Studio 2022 Community** の「無料ダウンロード」をクリック

3. ダウンロードした `VisualStudioSetup.exe` を実行

#### 1.1.2 インストールオプション選択

Visual Studio Installerが起動したら：

1. **「ワークロード」タブ** を選択

2. 以下2つに **チェックを入れる:**
   ```
   ☑ .NETデスクトップ開発
   ☑ C++によるデスクトップ開発
   ☑ C++によるゲーム開発 (オプション)
   ```

3. 右下の **「インストール」** をクリック

4. 完了まで待つ（30-60分）

**✅ 確認ポイント:**
- スタートメニューに「Visual Studio 2022」が表示される
- 起動すると「サインイン」画面が表示される（スキップ可能）

---

### Step 1.2: Unreal Engine 5.4 のインストール

**所要時間:** 45-90分（ダウンロード含む）

#### 1.2.1 Epic Games Launcher インストール

1. ブラウザで以下にアクセス:
   ```
   https://www.epicgames.com/store/ja/download
   ```

2. **「Epic Games Launcher をダウンロード」** をクリック

3. ダウンロードした `EpicInstaller.msi` を実行

4. 画面の指示に従ってインストール

#### 1.2.2 Unreal Engine 5.4 インストール

1. Epic Games Launcher を起動

2. 左メニューから **「Unreal Engine」** を選択

3. 上部タブから **「ライブラリ」** を選択

4. **「エンジンバージョン」セクション** で **「+」** ボタンをクリック

5. バージョン選択で **「5.4.x」** (最新の5.4) を選択

6. インストール先を確認（デフォルト推奨）:
   ```
   C:\Program Files\Epic Games\UE_5.4
   ```

7. **「インストール」** をクリック

8. 完了まで待つ（45-90分、ネット速度による）

**✅ 確認ポイント:**
- Epic Games Launcher の「ライブラリ」に UE_5.4 が表示される
- 「起動」ボタンが押せる状態になる

---

### Step 1.3: Git のインストール

**所要時間:** 5-10分

#### 1.3.1 ダウンロード & インストール

1. ブラウザで以下にアクセス:
   ```
   https://git-scm.com/download/win
   ```

2. **「64-bit Git for Windows Setup」** をクリック

3. ダウンロードした `Git-xxx-64-bit.exe` を実行

4. インストール画面：
   - すべて **デフォルトのまま「Next」** を連打でOK
   - 最後に「Finish」をクリック

**✅ 確認ポイント:**
```powershell
# PowerShellまたはコマンドプロンプトで実行
git --version
# 出力例: git version 2.43.0.windows.1
```

---

## Phase 2: プロジェクトのセットアップ

### Step 2.1: リポジトリのクローン

**所要時間:** 5分

#### 2.1.1 作業フォルダを決める

推奨フォルダ（例）:
```
C:\UE5Projects\
```

#### 2.1.2 リポジトリをクローン

1. **PowerShell** を開く（スタートメニューで「PowerShell」と検索）

2. 作業フォルダに移動:
   ```powershell
   cd C:\UE5Projects
   ```

3. リポジトリをクローン:
   ```powershell
   git clone https://github.com/hirakawat-hmp/gaussian-vrm-ue5.git
   ```

4. フォルダに移動:
   ```powershell
   cd gaussian-vrm-ue5
   ```

**✅ 確認ポイント:**
```powershell
dir
# 以下のフォルダが表示される:
#   gvrm-format/
#   apps/
#   assets/
#   ue5/
#   README.md
```

---

### Step 2.2: UE5プロジェクトの作成

**所要時間:** 10分

#### 2.2.1 新規プロジェクト作成

1. Epic Games Launcher を起動

2. 左メニュー **「Unreal Engine」** → 上部タブ **「ライブラリ」**

3. UE_5.4 の **「起動」** ボタンをクリック

4. **Unreal Project Browser** が開く

5. **「ゲーム」カテゴリ** を選択

6. **「サードパーソン」** テンプレートを選択

7. プロジェクト設定:
   ```
   プロジェクトタイプ: C++ (重要！)
   プロジェクト名: GVRMTestProject
   保存場所: C:\UE5Projects\
   スターターコンテンツ: なし (チェック外す)
   レイトレーシング: オフ (チェック外す)
   ```

8. **「作成」** をクリック

9. プロジェクトが開くまで待つ（3-5分）

10. **エディタが開いたら一旦閉じる** （重要！）

**✅ 確認ポイント:**
```
C:\UE5Projects\GVRMTestProject\
  ├── GVRMTestProject.uproject  ← このファイルがある
  ├── Source/
  ├── Content/
  └── Config/
```

---

### Step 2.3: 必須プラグインのインストール

**所要時間:** 10-15分

#### 2.3.1 Pluginsフォルダを作成

1. エクスプローラーで以下を開く:
   ```
   C:\UE5Projects\GVRMTestProject\
   ```

2. 右クリック → **「新規作成」** → **「フォルダー」**

3. フォルダ名: `Plugins`

**フォルダ構造（現時点）:**
```
GVRMTestProject/
├── Content/
├── Source/
├── Plugins/          ← 今作った
└── GVRMTestProject.uproject
```

#### 2.3.2 VRM4U プラグインのインストール

1. PowerShell で以下を実行:
   ```powershell
   cd C:\UE5Projects\GVRMTestProject\Plugins
   git clone https://github.com/ruyo/VRM4U.git
   ```

2. 完了まで待つ（1-2分）

**✅ 確認ポイント:**
```
Plugins\VRM4U\
  ├── VRM4U.uplugin
  ├── Source/
  └── Resources/
```

#### 2.3.3 XVERSE (Gaussian Splatting) プラグインのインストール

1. 同じPowerShellで実行:
   ```powershell
   git clone https://github.com/xverse-engine/XScene-UEPlugin.git
   ```

2. 必要な部分だけコピー:
   ```powershell
   xcopy /E /I "XScene-UEPlugin\UEPlugin\Plugin\XV3DGS" "XV3DGS\"
   ```

3. 元のフォルダは削除（不要）:
   ```powershell
   rmdir /S /Q XScene-UEPlugin
   ```

**✅ 確認ポイント:**
```
Plugins\XV3DGS\
  ├── XV3DGS.uplugin
  ├── Source/
  └── Shaders/
```

#### 2.3.4 GVRM Runtime プラグインのインストール

**方法A: シンボリックリンク（推奨）**

開発中はこちらが便利です。

1. **管理者権限で PowerShell を開く**
   - スタートメニューで「PowerShell」を右クリック
   - **「管理者として実行」** を選択

2. Pluginsフォルダに移動:
   ```powershell
   cd C:\UE5Projects\GVRMTestProject\Plugins
   ```

3. シンボリックリンク作成:
   ```powershell
   cmd /c mklink /D GVRMRuntime "C:\UE5Projects\gaussian-vrm-ue5\ue5\Plugins\GVRMRuntime"
   ```

**方法B: フォルダコピー（簡単）**

シンボリックリンクがうまくいかない場合:

```powershell
cd C:\UE5Projects\GVRMTestProject\Plugins
xcopy /E /I "C:\UE5Projects\gaussian-vrm-ue5\ue5\Plugins\GVRMRuntime" "GVRMRuntime\"
```

**✅ 確認ポイント:**
```
Plugins\
├── VRM4U\          ✓
├── XV3DGS\         ✓
└── GVRMRuntime\    ✓ ← 新しく追加
    ├── GVRMRuntime.uplugin
    ├── Source/
    └── Shaders/
```

---

## Phase 3: プロジェクトのビルド

### Step 3.1: Visual Studioプロジェクトファイルの生成

**所要時間:** 2-3分

#### 3.1.1 プロジェクトファイル生成

1. エクスプローラーで以下を開く:
   ```
   C:\UE5Projects\GVRMTestProject\
   ```

2. **`GVRMTestProject.uproject`** を **右クリック**

3. メニューから **「Generate Visual Studio project files」** を選択
   - このメニューが無い場合は、UE5が正しくインストールされていません

4. 処理完了まで待つ（2-3分）
   - コマンドプロンプトが開いて処理が走る
   - 自動的に閉じる

**✅ 確認ポイント:**
```
GVRMTestProject\
├── GVRMTestProject.sln  ← 新しく生成される
├── GVRMTestProject.uproject
└── Intermediate\        ← 新しく生成される
```

#### 3.1.2 よくあるエラーと対処法

**エラー1: 「Generate Visual Studio project files」が無い**
```
原因: UE5のファイル関連付けが失敗している
対処: Epic Games Launcher → UE_5.4 → 「確認」ボタンを押す
```

**エラー2: 「Could not find NetFxSDK」**
```
原因: .NET Framework SDKが無い
対処: Visual Studio Installerで「.NETデスクトップ開発」を再インストール
```

---

### Step 3.2: Visual Studioでビルド

**所要時間:** 10-20分（初回）

#### 3.2.1 Visual Studio で開く

1. エクスプローラーで **`GVRMTestProject.sln`** をダブルクリック

2. Visual Studio 2022 が起動する

3. 初回起動時の設定:
   - 「サインイン」画面 → **「スキップ」可能**
   - カラーテーマ → お好みで選択
   - 「Visual Studioの起動」ボタンをクリック

#### 3.2.2 ビルド設定の確認

Visual Studio が開いたら：

1. 上部のツールバーで以下を確認:
   ```
   ┌─────────────────┬──────┐
   │ Development Editor │ Win64 │
   └─────────────────┴──────┘
   ```

   - 左側（構成）: **Development Editor**
   - 右側（プラットフォーム）: **Win64**

2. もし違う設定になっている場合:
   - ドロップダウンから選択して変更

**これが最重要！** 間違えるとビルドに失敗します。

#### 3.2.3 ビルド実行

1. メニューバーから **「ビルド」** → **「ソリューションのビルド」**
   - またはキーボードで **Ctrl + Shift + B**

2. 下部の **「出力」ウィンドウ** でビルドの進行状況を確認
   ```
   1>------ ビルド開始: プロジェクト: GVRMTestProject, 構成: Development Editor x64 ------
   ```

3. **初回ビルドは時間がかかります:**
   - 高性能PC: 5-10分
   - 標準的なPC: 10-20分
   - 低スペックPC: 20-30分

4. **コーヒーブレイク推奨** ☕

#### 3.2.4 ビルド完了の確認

ビルドが完了すると、出力ウィンドウに以下のような表示が出ます:

**✅ 成功の場合:**
```
========== ビルド: 4 正常終了、0 失敗、0 更新不要、0 スキップ ==========
========== ビルド 完了時刻: XX:XX ==========
```

**❌ 失敗の場合:**
```
========== ビルド: 3 正常終了、1 失敗、0 更新不要、0 スキップ ==========
```

失敗した場合は、次のセクション「トラブルシューティング」を参照してください。

---

### Step 3.3: ビルドエラーの対処（失敗した場合）

#### 3.3.1 エラーリストの確認

1. Visual Studio 下部の **「エラー一覧」** タブをクリック

2. エラーの種類を確認:
   ```
   エラー C2039: 'xxx': は 'yyy' のメンバーではありません
   エラー LNK2019: 未解決の外部シンボル
   エラー C1083: include ファイルを開けません
   ```

#### 3.3.2 よくあるエラーと対処法

**エラーA: インクルードファイルが見つからない**
```
エラー例:
fatal error C1083: Cannot open include file: 'NiagaraDataInterface.h'
```

**原因:** Niagaraモジュールが有効化されていない

**対処法:**
1. `.uproject` ファイルをテキストエディタで開く
2. 以下を追加:
```json
"Plugins": [
    {
        "Name": "Niagara",
        "Enabled": true
    }
]
```
3. ファイルを保存
4. プロジェクトファイルを再生成（Step 3.1.1）
5. Visual Studio を再起動
6. 再ビルド

**エラーB: 未解決の外部シンボル（LNK2019）**
```
エラー例:
error LNK2019: unresolved external symbol "void __cdecl..."
```

**原因:** ビルドの依存関係が壊れている

**対処法:**
1. Visual Studio を閉じる
2. 以下のフォルダを削除:
   ```
   C:\UE5Projects\GVRMTestProject\Binaries
   C:\UE5Projects\GVRMTestProject\Intermediate
   C:\UE5Projects\GVRMTestProject\Saved
   ```
3. プロジェクトファイルを再生成（Step 3.1.1）
4. Visual Studio で再度開く
5. **「ソリューションのクリーン」** → **「ソリューションのビルド」**

**エラーC: コンパイラのバージョンエラー**
```
エラー例:
error MSB8020: The build tools for v143 (Platform Toolset = 'v143') cannot be found
```

**原因:** Visual Studio 2022のC++コンパーネントが不足

**対処法:**
1. Visual Studio Installer を起動
2. 「変更」ボタンをクリック
3. 「C++によるデスクトップ開発」を確認
4. 右側の「インストールの詳細」で以下を確認:
   - ✓ MSVC v143 - VS 2022 C++ x64/x86
   - ✓ Windows 10 SDK
5. 「変更」ボタンでインストール

#### 3.3.3 それでも解決しない場合

**完全クリーンビルド:**

```powershell
# PowerShellで実行
cd C:\UE5Projects\GVRMTestProject

# 生成ファイルを全削除
rmdir /S /Q Binaries
rmdir /S /Q Intermediate
rmdir /S /Q Saved
rmdir /S /Q Plugins\VRM4U\Binaries
rmdir /S /Q Plugins\VRM4U\Intermediate
rmdir /S /Q Plugins\XV3DGS\Binaries
rmdir /S /Q Plugins\XV3DGS\Intermediate
rmdir /S /Q Plugins\GVRMRuntime\Binaries
rmdir /S /Q Plugins\GVRMRuntime\Intermediate
del /Q *.sln

# プロジェクトファイル再生成
# （GVRMTestProject.uproject を右クリック → Generate...）
```

---

## Phase 4: Unreal Editorでの動作確認

### Step 4.1: エディタの起動

**所要時間:** 5-10分（初回）

#### 4.1.1 エディタを開く

**方法A: Visual Studio から起動**

1. Visual Studio のメニュー:
   - **「デバッグ」** → **「デバッグなしで開始」**
   - またはキーボードで **Ctrl + F5**

2. Unreal Editor が起動する（初回は5-10分かかる）

**方法B: Epic Games Launcher から起動**

1. Epic Games Launcher を開く
2. **「ライブラリ」** → **「最近使ったプロジェクト」**
3. `GVRMTestProject` の **「起動」** をクリック

#### 4.1.2 初回起動時の処理

初回起動時は以下の処理が走ります（自動）:

1. **シェーダーのコンパイル**
   - 画面右下に進行状況表示
   - 5-15分かかる（スペックによる）
   - **完了まで待つ**

2. **プラグインのロード**
   - 各プラグインが初期化される

3. **エディタ画面が表示される**

**✅ 起動成功のサイン:**
- 3Dビューポートにキャラクターとマップが表示される
- メニューバーが操作できる
- コンソールにエラーが無い（下部のOutput Log）

---

### Step 4.2: プラグインの確認

#### 4.2.1 プラグイン有効化の確認

1. メニューバーから **「編集」（Edit）** → **「プラグイン」（Plugins）**

2. プラグインウィンドウが開く

3. 検索ボックスに **「GVRM」** と入力

4. **「GVRM Runtime」** が表示され、**「有効」** にチェックが入っていることを確認

**✅ 確認ポイント:**
```
検索: "GVRM"
結果:
  ☑ GVRM Runtime
    Runtime support for Gaussian-VRM avatars
    バージョン: 0.1.0
```

5. 他のプラグインも確認:
   - 検索: **「VRM」** → VRM4U が有効
   - 検索: **「XV3DGS」** → XVERSE が有効
   - 検索: **「Niagara」** → Niagara が有効

#### 4.2.2 プラグインが無効の場合

もし「GVRM Runtime」が無効（チェックが外れている）場合:

1. チェックボックスをクリックして有効化
2. **「今すぐ再起動」** ボタンをクリック
3. エディタが再起動する

---

### Step 4.3: サンプルの変換とインポート

**所要時間:** 15-20分

#### 4.3.1 Python環境のセットアップ

1. PowerShell を開く

2. Python ツールのフォルダに移動:
   ```powershell
   cd C:\UE5Projects\gaussian-vrm-ue5\ue5\Tools
   ```

3. Python仮想環境を作成（初回のみ）:
   ```powershell
   python -m venv venv
   ```

4. 仮想環境を有効化:
   ```powershell
   .\venv\Scripts\Activate.ps1
   ```

   エラーが出る場合（実行ポリシー）:
   ```powershell
   Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
   # 再度実行
   .\venv\Scripts\Activate.ps1
   ```

5. 依存パッケージをインストール:
   ```powershell
   pip install -r requirements.txt
   ```

   もしrequirements.txtが無い場合:
   ```powershell
   pip install zipfile-deflate64
   ```

#### 4.3.2 サンプルGVRMファイルの変換

1. 同じPowerShellで実行:
   ```powershell
   python gvrm_to_ue5.py ..\..\assets\author.gvrm -o C:\Temp\GVRM_Output
   ```

2. 変換完了を確認:
   ```
   ✓ Conversion complete!

   Output files:
     - C:\Temp\GVRM_Output\model.vrm
     - C:\Temp\GVRM_Output\model.ply
     - C:\Temp\GVRM_Output\splat_binding.csv
     - C:\Temp\GVRM_Output\metadata.json
   ```

**✅ 確認ポイント:**
```powershell
dir C:\Temp\GVRM_Output
# ファイルが4つ以上ある
```

#### 4.3.3 UE5へのインポート（VRMモデル）

1. Unreal Editor に戻る

2. **Content Browser** (コンテンツブラウザ)で右クリック
   - **「New Folder」** → 名前: `GVRM`
   - その中にさらに **「New Folder」** → 名前: `Characters`

3. `Characters` フォルダを開いた状態で:
   - **「Import」** ボタンをクリック
   - または右クリック → **「Import to /Game/GVRM/Characters/」**

4. ファイル選択ダイアログで:
   - `C:\Temp\GVRM_Output\model.vrm` を選択
   - **「開く」** をクリック

5. **VRM4U インポートダイアログ** が表示される:
   - すべてデフォルトのまま **「Import」** をクリック

6. インポート完了まで待つ（10-30秒）

**✅ 確認ポイント:**
```
Content/GVRM/Characters/
  ├── model (Skeletal Mesh)
  ├── model_Physics
  ├── model_Skeleton
  └── T_model_xxx (テクスチャ複数)
```

---

## Phase 5: 簡易動作テスト

### Step 5.1: VRMモデルの表示テスト

**所要時間:** 5分

#### 5.1.1 レベルに配置

1. Content Browser で `model` (Skeletal Mesh) を探す

2. **レベルビューポート** (3D画面) にドラッグ&ドロップ

3. VRMキャラクターがレベルに配置される

4. **「Play」** ボタン（▶）をクリック

5. キャラクターが表示されればOK！

**✅ 成功:**
- VRMキャラクターが画面に表示される
- テクスチャが正しく適用されている

**❌ 失敗例と対処:**

| 症状 | 原因 | 対処法 |
|------|------|--------|
| 真っ黒 | マテリアルエラー | VRM4Uが正しくインストールされているか確認 |
| 表示されない | スケールが大きすぎる | Details パネルでScaleを0.01に |
| ピンク色 | テクスチャ未ロード | 少し待ってから再度Play |

---

## 🎉 完了チェックリスト

以下がすべて ✅ になっていれば成功です！

- [ ] Visual Studio 2022 がインストールされている
- [ ] Unreal Engine 5.4 がインストールされている
- [ ] GVRMTestProject がビルドできた（エラーなし）
- [ ] Unreal Editor が起動できた
- [ ] プラグイン「GVRM Runtime」が有効になっている
- [ ] Python変換ツールでGVRMファイルを変換できた
- [ ] VRMモデルをUE5にインポートできた
- [ ] レベルでVRMキャラクターが表示できた

---

## 次のステップ

ここまでできたら、次は **Niagara System** の設定です：

👉 **[NIAGARA_SETUP_GUIDE.md](./NIAGARA_SETUP_GUIDE.md)** を参照

---

## よくある質問（FAQ）

### Q1: ビルドに何時間もかかる

**A:** 初回ビルドは時間がかかります。以下を確認:
- SSDを使用していますか？（HDDだと2-3倍遅い）
- ウイルス対策ソフトがスキャンしていませんか？
- 他のアプリを閉じていますか？

通常の初回ビルド時間:
- 高性能PC（Ryzen 9, RTX 4090）: 5-7分
- 中性能PC（Ryzen 5, RTX 3060）: 10-15分
- 低性能PC（古いi5, GTX 1660）: 20-30分

### Q2: エディタが起動しない

**A:** 以下を順番に試してください:
1. Visual Studio を閉じる
2. Epic Games Launcher から起動してみる
3. Crash Reporterが出る場合は、ログを確認
4. `Saved/Logs/` フォルダの最新ログファイルを確認

### Q3: プラグインが表示されない

**A:**
1. Pluginsフォルダの構造を確認
2. `.uplugin` ファイルが存在するか確認
3. プロジェクトファイルを再生成
4. エディタを再起動

### Q4: Python変換ツールが動かない

**A:**
```powershell
# Pythonのバージョン確認
python --version
# 3.8以上が必要

# pipが最新か確認
python -m pip install --upgrade pip

# 依存パッケージを再インストール
pip install --force-reinstall zipfile-deflate64
```

---

## トラブル時の連絡先

- **GitHub Issues:** https://github.com/hirakawat-hmp/gaussian-vrm-ue5/issues
- **元のGVRMプロジェクト:** https://gaussian-vrm.github.io/

---

**最終更新:** 2025-11-14
**対象バージョン:** UE 5.4, GVRM Runtime 0.1.0
**作成者:** gaussian-vrm community
