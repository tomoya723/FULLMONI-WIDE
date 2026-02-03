# GitHub Copilot ルール

このワークスペースで作業する際の必須ルール：

## 禁止事項

### 1. Smart Configuratorで管理するファイルを直接編集しない
以下のファイル/フォルダは Smart Configurator (e2 studio) が自動生成・管理するため、**絶対に直接編集しないこと**：
- `src/smc_gen/` 配下のすべてのファイル
- `src/smc_gen/r_config/` 配下の設定ファイル
- `qe_emwin_config.h`
- `*.scfg` ファイル

これらのパラメータを変更する必要がある場合は、ユーザーに「Smart Configuratorで変更してください」と伝えること。

### 2. 勝手にGIT操作をしない
以下のGITコマンドは**ユーザーの明示的な許可なく実行しないこと**：
- `git checkout`
- `git reset`
- `git revert`
- `git stash`
- `git merge`
- `git rebase`
- `git cherry-pick`
- `git restore`

`git status`, `git diff`, `git log` などの読み取り専用コマンドは可。

### 3. リンカスクリプトのメモリ領域変更は慎重に
`linker_script.ld` のメモリ領域（RAM, RAM2, ROM等）を変更する場合は、LCDフレームバッファ等の既存配置と重複しないことを必ず確認すること。

## 開発環境
- MCU: Renesas RX72N
- IDE: e2 studio (Smart Configurator)
- GUI: emWin (AppWizard)
- ツールチェーン: GCC-RX

## AndroidアプリのPCデバッグ手順

ユーザーが「Androidアプリをデバッグ」「エミュレーターでテスト」等と言った場合、**まず現在の状態を確認**してから必要な手順のみ実行すること。

### 1. 状態確認（最初に必ず実行）
```powershell
# ADBパスを通す（毎回必要）
$env:Path += ";$env:LOCALAPPDATA\Android\Sdk\platform-tools"

# エミュレーター確認
adb devices

# TCPブリッジ確認
netstat -ano | Select-String ":9999"

# COMポート確認
Get-WMIObject Win32_SerialPort | Select-Object DeviceID, Description
```

### 2. 不足分のみ起動
| 状態 | 対応 |
|------|------|
| エミュレーターが無い | `Start-Process "$env:LOCALAPPDATA\Android\Sdk\emulator\emulator.exe" -ArgumentList "-avd", "Medium_Phone_API_36.1"` |
| adb reverse未設定 | `adb reverse tcp:9999 tcp:9999` |
| TCPブリッジ未起動 | `cd tools; powershell -ExecutionPolicy Bypass -File .\com_bridge.ps1 -ComPort COM19` |
| APK未インストール | `adb install -r "HostApp\FULLMONI-WIDE-Android\app\build\outputs\apk\debug\app-debug.apk"` |

### 3. アプリ起動
```powershell
adb shell am start -n com.fullmoni.terminal/.MainActivity
```

### 4. 接続
アプリのHomeScreenで **「TCP Bridge (Real)」** ボタンをタップ

### 注意事項
- **ゼロから全部セットアップし直さない**
- 状態確認で動いているものは再起動しない
- ExecutionPolicyエラーが出たら `-ExecutionPolicy Bypass` を付ける
- ポート競合時は `Get-NetTCPConnection -LocalPort 9999` で確認
