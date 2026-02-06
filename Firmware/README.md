## FULLMONI-WIDE：Firmware
Renesas e2Studio + GNU GCC-RX + SEGGER emWIN<br>
順次更新予定
***
## サードパーティライセンスについて
- 本プロジェクトの**オリジナルコード**（例: `Firmware/src` の自作 `.c/.h` 等）は **MIT License** です。
- ただし、以下の**第三者提供物**は各社の条件が優先されます（＝MITの適用外）:
  - **Renesas FIT / 生成コード**（`smc_gen` 等）
    - Renesas製品上での利用を前提とする無保証ライセンス。著作権表記・免責コメントを改変せず保持してください。
  - **SEGGER emWin**（GUIライブラリ）
    - 本リポジトリでは **再配布しません**。スマートコンフィグレータからコード生成を実行し`r_emwin_rx`をプロジェクトに追加してください。
    - emWinは再配布禁止条項があり、SEGGERの明示許諾なしに第三者へ配布できません。
### ライセンスの適用範囲
- **MIT**: オリジナルコード全般
- **Renesasライセンス**: FIT/生成コード
- **SEGGERライセンス**: emWin（再配布不可）

詳細は `THIRD_PARTY_NOTICES.md` を参照してください。

- The **original code** of this project (e.g., self-made `.c/.h` files in `Firmware/src`) is licensed under the **MIT License**.
- However, the following **third-party materials** are subject to the terms and conditions of each company (i.e., not covered by the MIT License):
- **Renesas FIT / Generated Code** (e.g., `smc_gen`)
- A no-warranty license intended for use on Renesas products. Please retain the copyright notices and disclaimer comments without modification.
  - **SEGGER emWin** (GUI library)
- This repository does not redistribute emWin. Please generate code using the Smart Configurator and add `r_emwin_rx` to your project.
- emWin has a redistribution prohibition clause, and cannot be distributed to third parties without explicit permission from SEGGER.
### Scope of License Application
- **MIT**: Original code in general
- **Renesas License**: FIT/generated code
- **SEGGER License**: emWin (redistribution prohibited)

For details, refer to `THIRD_PARTY_NOTICES.md`.

***
## CAN data frame setting

### CAN設定のカスタマイズ（Issue #65）

FULLMONI-WIDEは、CAN受信ID・データフィールドをカスタマイズ可能です。
設定はEEPROMに保存され、電源OFF後も永続化されます。

#### コンソールコマンド

| コマンド | 説明 | 例 |
|---------|------|-----|
| `can_list` | 現在のCAN設定一覧表示 | `can_list` |
| `can_ch <n> <id> <en>` | チャンネル設定 | `can_ch 1 0x3E8 1` |
| `can_field <n> <ch> <byte> <size> <type> <end> <var> <mul> <div>` | フィールド設定 | |
| `can_preset <id>` | プリセット適用 | `can_preset 0` (MoTeC) |
| `can_save` | EEPROMに保存 | `can_save` |

#### 対応プリセット

| ID | ECU | CAN ID範囲 |
|----|-----|-----------|
| 0 | MoTeC M100/M400/M800 | 0x3E8-0x3ED |
| (将来) | LINK G4+/G4X | TBD |
| (将来) | AEM EMS | TBD |

詳細は [docs/issue_65_can_customization_design.md](../docs/issue_65_can_customization_design.md) を参照。

### デフォルトCAN設定（MoTeC M100互換）

frame No1 address : 1000(dec) , 0x3E8(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3E8|1|0|RPM High byte|[rpm]|1|
|0x3E8|1|1|RPM Lo byte||1|
|0x3E8|1|2|Throttle Positon H|[%]|0.1|
|0x3E8|1|3|Throttle Positon L|[%]|0.1|
|0x3E8|1|4|Manifold pressure H|[kPa]|0.1|
|0x3E8|1|5|Manifold pressure L|[kPa]|0.1|
|0x3E8|1|6|Inlet Air Temp H|[degC]|0.1|
|0x3E8|1|7|Inlet Air Temp L|[degC]|0.1|

frame No2 address : 1001(dec) , 0x3E9(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3E9|2|0|Engine Temp H|[degC]|0.1|
|0x3E9|2|1|Engine Temp L|[degC]|0.1|
|0x3E9|2|2|Lambda1 H|[-]|0.0147(to PetrolAFR)|
|0x3E9|2|3|Lambda1 L|[-]|0.0147|
|0x3E9|2|4|Lambda2 H|[-]|0.0147|
|0x3E9|2|5|Lambda2 L|[-]|0.0147|
|0x3E9|2|6|Exhaust MAP H|[kPa]|0.1|
|0x3E9|2|7|Exhaust MAP L|[kPa]|0.1|


frame No3 address : 1002(dec) , 0x3EA(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EA|3|0|Mass Air Flow H|[g/s]|0.1?|
|0x3EA|3|1|Mass Air Flow L|[g/s]|0.1?|
|0x3EA|3|2|Fuel Temp H|[degC]|0.1|
|0x3EA|3|3|Fuel Temp L|[degC]|0.1|
|0x3EA|3|4|Fuel Pressure H|[kPa]|0.1|
|0x3EA|3|5|Fuel Pressure L|[kPa]|0.1|
|0x3EA|3|6|Oil Temp H|[degC]|0.1|
|0x3EA|3|7|Oil Temp L|[degC]|0.1|

frame No4 address : 1003(dec) , 0x3EB(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EB|4|0|Oil Pressure H|[kPa]|0.1|
|0x3EB|4|1|Oil Pressure L|[kPa]|0.1|
|0x3EB|4|2|Exh Gas Temp1 H|[degC]|0.1|
|0x3EB|4|3|Exh Gas Temp1 L|[degC]|0.1|
|0x3EB|4|4|Exh Gas Temp2 H|[degC]|0.1|
|0x3EB|4|5|Exh Gas Temp2 L|[degC]|0.1|
|0x3EB|4|6|Battery Voltage H|[V]|0.01|
|0x3EB|4|7|Battery Voltage L|[V]|0.01|

frame No5 address : 1004(dec) , 0x3EC(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3EC|5|0|ECU internal Temp H|[degC]|0.1|
|0x3EC|5|1|ECU internal Temp L|[degC]|0.1|
|0x3EC|5|2|Ground Speed H|[km/h]|1?|
|0x3EC|5|3|Ground Speed L|[km/h]|1?|
|0x3EC|5|4|La1 Aim value H|[-]|0.0147(PetrolAFR)|
|0x3EC|5|5|La1 Aim value L|[-]|0.0147|
|0x3EC|5|6|La2 Aim value H|[-]|0.0147|
|0x3EC|5|7|La2 Aim value L|[-]|0.0147|

frame No6 address : 1005(dec) , 0x3ED(hex)
|CAN Address|frame no| byte |assign|unit|multiplier(LSB)|
|---|---|---|---|---|---|
|0x3ED|6|0|Ignition advance H|[degBTDC]|0.1|
|0x3ED|6|1|Ignition advance L|[degBTDC]|0.1|
|0x3ED|6|2|Fuel Inj Duty Cycle H|[%]|0.1|
|0x3ED|6|3|Fuel Inj Duty Cycle L|[%]|0.1|
|0x3ED|6|4|Gear H|[-]|1|
|0x3ED|6|5|Gear L|[-]|1|
|0x3ED|6|6|NoData||
|0x3ED|6|7|NoData||
***
## emWin code generation
・Open the scfg file from the project tree and launch Smart Configurator.<br>
・Confirm that the middleware r_emwin_rx is already registered.<br>
・Click “Code Generation.”<br>
![image](https://github.com/user-attachments/assets/01f0507d-a2b9-420b-afb1-4e5bb2bbb660)<br>
***
## emWin AppWizard resource switch setting

The project provides multiple screen resources (variants), so select the one you want to use.

### 利用可能なバリアント

| ID | テーマ | 説明 |
|----|--------|------|
| aw001 | Standard (Blue Theme) | 標準テーマ（青系）- デフォルト |
| aw002 | Racing (Red Theme) | レーシングテーマ（赤系） |

### 自動ビルド（推奨）

`tools/build_variants.ps1` を使用すると、複数バリアントを自動でビルドできます：

```powershell
# 両バリアントをビルド
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1 -Version "0.1.2"

# 特定バリアントのみ
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1 -Variants aw001
```

出力先: `test-release/Firmware_v{version}_{variant}.bin`

詳細は [tools/README.md](../tools/README.md#build_variantsps1) を参照。

### 手動ビルド（e2 studio）

バリアント切り替えには **Junction** 方式を使用します。`Firmware/aw` フォルダが `aw001` または `aw002` へのリンクとして機能します。

#### 初回セットアップ

1. **Junction を作成**（VS Code ターミナルまたはコマンドプロンプト）:
   ```cmd
   cd Firmware
   mklink /J aw aw002
   ```

2. **e2 studio でソースフォルダを追加**:
   - プロジェクト右クリック → Properties → C/C++ General → Paths and Symbols → Source Location
   - `aw/Resource` と `aw/Source` を追加
   - Apply and Close

#### バリアント切り替え方法

**方法1: バッチファイル（推奨）**

Explorer で以下のファイルをダブルクリック:
- `tools/switch_to_aw001.bat` → aw001（青テーマ）に切り替え
- `tools/switch_to_aw002.bat` → aw002（赤テーマ）に切り替え

**方法2: e2 studio External Tools**

事前に External Tools に登録しておくと、メニューから切り替え可能:
1. Run → External Tools → External Tools Configurations
2. Program → New Configuration
3. 設定:
   - Name: `Switch to aw001`
   - Location: `C:\Windows\System32\cmd.exe`
   - Working Directory: `${project_loc:Firmware}`
   - Arguments: `/c "..\tools\switch_to_aw001.bat"`

#### 切り替え後の操作

1. e2 studio でプロジェクトを **Refresh**（F5）
2. **Clean** → **Build**
3. デバッグ実行

***
## Startup Image Transfer Protocol (imgread/imgwrite)

FULLMONI-WIDEは、USB CDC経由で起動画面の読み取り・書き込みをサポートしています。

### コマンド一覧

| コマンド | 説明 |
|---------|------|
| `imgwrite` | 起動画面書き込みモード開始 |
| `imgread` | 起動画面読み取りモード開始 |

### 画像フォーマット

| 項目 | 値 |
|------|-----|
| 解像度 | 765×256 ピクセル |
| 色深度 | RGB565 (16bit/pixel) |
| ヘッダ | 16バイト (SEGGER AppWizard形式) |
| 総サイズ | 391,696バイト |

### 転送プロトコル（書き込み: imgwrite）

1. **コマンド送信**: `imgwrite\r\n`
2. **応答待ち**: `IMGWRITE_READY`
3. **サイズ送信**: 4バイト (リトルエンディアン、391,696)
4. **データ転送**: 256バイトチャンク × ACK(0x06)応答待ち
5. **CRC送信**: CRC-16/CCITT-FALSE (2バイト)
6. **終端マーカー**: `0xED 0x0F 0xAA 0x55`
7. **完了応答**: `IMGWRITE_OK` または `IMGWRITE_FAIL`

### 転送プロトコル（読み取り: imgread）

1. **コマンド送信**: `imgread\r\n`
2. **応答待ち**: `IMGREAD_READY`
3. **サイズ受信**: 4バイト (リトルエンディアン)
4. **データ受信**: ストリーミング (約3-5秒)
5. **CRC受信**: CRC-16/CCITT-FALSE (2バイト)
6. **終端マーカー**: `0xED 0x0F 0xAA 0x55`

### CRC計算

```c
// CRC-16/CCITT-FALSE
// 多項式: 0x1021, 初期値: 0xFFFF
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}
```

### メモリ配置

| 領域 | アドレス | サイズ |
|------|----------|--------|
| 起動画面データ | 0xFFE00000 (Linear Flash) | 391,696バイト |
| RAM2 (転送バッファ) | 0x00800000 | 8KB (4KB関数 + 4KB prefix) |

#### 固定アドレス配置について

起動画像データ(acmtc配列)は、リンカスクリプトにより固定アドレス`0xFFE00000`に配置されます。
これにより、**ファームウェア更新後もカスタム起動画像が保持されます**。

```
フラッシュメモリ配置:
0xFFC20000  ファームウェアヘッダ
0xFFC20040  .text, .rodata等 (コード/データ)
            ※ mtc.oは除外
            :
0xFFE00000  .startup_image セクション ← 起動画像固定領域
            acmtc配列 (384KB)          imgwriteの書き込み先
            :
0xFFFFFF80  .exvectors
```

リンカスクリプト(`linker_script.ld`)の設定:
- `EXCLUDE_FILE(*mtc.o)` で `.rodata` から除外
- `.startup_image` セクションで `mtc.o` を固定アドレスに配置

AppWizard生成の `mtc.c` は変更不要です。

### 実装ファイル

- `Firmware/src/startup_image_write.c` - imgwrite/imgread コマンド処理
- `HostApp/.../Services/StartupImageService.cs` - ホスト側転送サービス

***
## Development with Visual Studio Code

This project supports development using Visual Studio Code alongside e² studio. The VSCode environment provides IntelliSense, building, and hardware debugging capabilities.

### Prerequisites

**1. Install VSCode**<br>
Download from [https://code.visualstudio.com/](https://code.visualstudio.com/)<br>
<br>
**2. Install Renesas Extensions**<br>
Open the Extensions view (`Ctrl+Shift+X`) and install the following extensions:<br>
![image](https://github.com/user-attachments/assets/392ed514-0642-4526-b4b6-d237901e3c16)<br>
- Renesas Platform (`renesas.renesas-platform`)
- Renesas Debug (`renesas.renesas-debug`)
- Renesas Build Utilities (`renesas.renesas-build-utilities`)
- C/C++ Extension Pack (`ms-vscode.cpptools-extension-pack`)

After installation, the Renesas tab will appear in the sidebar.<br>
![image](https://github.com/user-attachments/assets/a0024cd8-c971-4ba2-8986-3e9b0a3c8680)<br>
<br>
**3. Install GCC-RX Toolchain**<br>
Ensure GCC for Renesas RX is installed (same toolchain as e² studio)<br>
<br>
**4. Install E2 Lite Debugger Driver**<br>
Install the USB driver for E2 Lite emulator from [Renesas website](https://www.renesas.com/eu/en/software-tool/e2-emulator-rte0t00020kce00000r#downloads)<br>

### Building the Project

**1. Open the project folder**<br>
`File` → `Open Folder` → Select `FULLMONI-WIDE` directory<br>
<br>
**2. Start the build**<br>
Press `Ctrl+Shift+B` to open the build task menu, then select `Build (GCC-RX)`<br>
<br>
**3. Build progress and results**<br>
The terminal will show the build progress. Upon success, the binary size is displayed.<br>
![image](https://github.com/user-attachments/assets/d5392bb6-5fd8-4927-bdaf-78af3955b23f)<br>
The compiled binary will be generated at `Firmware/HardwareDebug/Firmware.elf`<br>
<br>
**Other Build Tasks:**<br>
- `Clean (GCC-RX)`: Remove all build artifacts
- `Rebuild (GCC-RX)`: Clean and rebuild the entire project
- `Size Info (GCC-RX)`: Display memory usage information

### Debugging with E2 Lite

**1. Hardware Connection**<br>
- Connect the E2 Lite debugger to your PC via USB
- Connect the E2 Lite to the target board
- Ensure the target board is powered externally

**2. Open Run and Debug view and start debugging**<br>
Click the Run and Debug icon in the sidebar or press `Ctrl+Shift+D`, then select the debug configuration "Debug with Renesas E2 Lite (RX72T)" and press `F5` or click the green play button<br>
![image](https://github.com/user-attachments/assets/ac58c95a-f3c1-4db6-a730-4f3636520719)<br>
<br>
**3. Debugging session**<br>
The debugger will automatically build the project, download the firmware, and stop at `main()` function.<br>
You can use breakpoints, step execution, and variable inspection.<br>
![image](https://github.com/user-attachments/assets/4e0c627a-ebed-4ed4-9eb4-0e46145382d6)<br>
<br>
**Debug Configuration:** The launch configuration is located at `.vscode/launch.json` with the following key settings:
- Device: `R5F572NN` (RX72T)
- Debugger: `E2LITE`
- Debug interface parameters match e² studio settings

### IntelliSense and Code Navigation

VSCode is configured with IntelliSense for the RX72T microcontroller.<br>
<br>
**Code completion**<br>
Type to get suggestions for Renesas BSP functions, peripheral registers, and macros.<br>
![image](https://github.com/user-attachments/assets/0f267222-b0d5-4be4-b42f-6e5e37a4ab09)<br>
<br>
**Hover documentation**<br>
Hover over functions or registers to see documentation.<br>
![image](https://github.com/user-attachments/assets/666177fb-42c7-4113-b856-e80769cc6f46)<br>
<br>
**Navigation features:**<br>
- Go to definition: `F12`
- Find all references: `Shift+F12`
- Peek definition: `Alt+F12`

**Configuration File:** `.vscode/c_cpp_properties.json` contains compiler paths, include directories, and RX72T-specific defines.

### Coexistence with e² studio

Both environments can be used interchangeably:<br>
<br>
![image](https://github.com/user-attachments/assets/1efdbae7-e9a6-42cd-8a7d-188e0ac32e4a)<br>
<br>
- **Shared Build System**: Both VSCode and e² studio use the same Makefile
- **Shared Source Code**: No duplicate code or configuration
- **Independent Settings**: IDE-specific settings are separated (`.vscode/` vs `.settings/`)
- **Smart Configurator**: Use e² studio for peripheral configuration, code generation is compatible with both environments

### Notes

- Smart Configurator code generation should be performed in e² studio
- ⚠️ **注意**: Smart Configuratorでコード生成すると、`Config_SCI9_user.c` のユーザーコード部分が初期化されます。UART受信処理（`r_Config_SCI9_callback_receiveend` 内のトリガーフラグ設定など）を手動で復元してください。
- The `.vscode/` directory is version-controlled for team collaboration
- Build artifacts in `Firmware/HardwareDebug/` are shared between both IDEs
- E2 Lite debugger can be used by only one IDE at a time
