# EEZ デザイン切替システム

## 概要

複数のUIデザイン（eez001 / eez002）を1つのリポジトリで管理し、ビルドターゲットを切り替える仕組み。
Windows Directory Junction（シンボリックリンクの一種）を使い、Makefileを変更せずにデザインを差し替える。

## ディレクトリ構成

```
Firmware/
  eez/
    eez001/                          ← デザイン1 (Standard / aw001)
    │  eez001.eez-project            ← EEZ Studio プロジェクト
    │  eez001.eez-project-ui-state
    │  *.png, *.ttf                  ← 画像・フォントアセット
    │
    ├── ui/                          ← EEZ Studio 生成コード
    │     screens.c/h, images.c/h, styles.c/h, ui.c/h
    │     eez_compat.h, actions.h, vars.h, fonts.h, ...
    │     images/*.c, fonts/*.c
    │
    └── ui_binding/                  ← データバインディング（手書き）
          ui_dashboard.c/h
    
    eez002/                          ← デザイン2 (Racing / aw002)
      （eez001と同じ構造）

  src/
    ui/          → Junction → ../eez/eez00X/ui
    ui_binding/  → Junction → ../eez/eez00X/ui_binding
```

## 各ディレクトリの役割

| パス | 内容 | 管理者 |
|------|------|--------|
| `eez/eez00X/*.eez-project` | EEZ Studio プロジェクトファイル | EEZ Studio |
| `eez/eez00X/*.png, *.ttf` | UI用の画像・フォントアセット | EEZ Studio |
| `eez/eez00X/ui/` | EEZ Studio が生成する C ソースコード | EEZ Studio (Build) |
| `eez/eez00X/ui_binding/` | ウィジェットとファームウェアデータのバインディング | 手書き |
| `src/ui/` | Junction（eez00X/ui を指す） | switch_design.ps1 |
| `src/ui_binding/` | Junction（eez00X/ui_binding を指す） | switch_design.ps1 |

## 切替方法

### VS Code

タスクパレット（`Ctrl+Shift+P` → `Tasks: Run Task`）から：
- **Switch to eez001** — デザイン1に切替
- **Switch to eez002** — デザイン2に切替

切替後、**Clean (GCC-RX)** → **Build (GCC-RX)** でリビルド。

### e2 studio

**Run → External Tools** から：
- **Switch to eez001**
- **Switch to eez002**

設定値：
| 項目 | 値 |
|------|-----|
| ロケーション | `C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe` |
| 作業ディレクトリー | `C:\Users\tomoy\git\FULLMONI-WIDE` |
| 引数 (eez001) | `-ExecutionPolicy Bypass -File "C:\Users\tomoy\git\FULLMONI-WIDE\tools\switch_design.ps1" -Design eez001` |
| 引数 (eez002) | `-ExecutionPolicy Bypass -File "C:\Users\tomoy\git\FULLMONI-WIDE\tools\switch_design.ps1" -Design eez002` |

### コマンドライン

```powershell
powershell -ExecutionPolicy Bypass -File tools\switch_design.ps1 -Design eez002
```

## スクリプトの動作 (switch_design.ps1)

1. 既存の Junction (`Firmware/src/ui`, `Firmware/src/ui_binding`) を削除
2. 選択したデザインフォルダへの新しい Junction を作成
3. `Firmware/src/variant_id.h` を更新（eez001→aw001, eez002→aw002）
4. 結果を表示

## EEZ Studio での編集ワークフロー

1. 編集したいデザインの `.eez-project` を EEZ Studio で開く
   - eez001: `Firmware/eez/eez001/eez001.eez-project`
   - eez002: `Firmware/eez/eez002/eez002.eez-project`
2. EEZ Studio 上でウィジェットを編集
3. EEZ Studio の **Build** を実行 → 同フォルダ内の `ui/` に生成コードが出力
4. 他方のデザインには影響しない（`destinationFolder: "./ui"` で自フォルダに出力）

## ui_binding について

`ui_binding/ui_dashboard.c` は EEZ 生成ウィジェットの名前（`ui_ArcRPM` 等）に直接依存する。
デザインごとにウィジェット構成が異なる場合、このファイルもデザイン別に管理する必要があるため、各デザインフォルダに配置している。

## バリアント ID マッピング

| デザイン | バリアント ID | 用途 |
|---------|-------------|------|
| eez001 | aw001 | Standard テーマ |
| eez002 | aw002 | Racing テーマ |

`variant_id.h` に `BUILD_VARIANT_ID` と `BUILD_DESIGN_ID` として定義され、ファームウェアヘッダーに埋め込まれる。

## 新しいデザインを追加する手順

1. 既存デザインフォルダをコピー：`eez/eez002` → `eez/eez003`
2. `.eez-project` をリネーム：`eez003.eez-project`
3. `tools/switch_design.ps1` の `ValidateSet` に `eez003` を追加
4. `switch_design.ps1` の `$variantMap` に `'eez003' = 'aw003'` を追加
5. VS Code タスク / e2 studio 外部ツールに Switch to eez003 を追加

## .gitignore

Junction 自体は Git 追跡しない（`.gitignore` で除外）：
```
Firmware/src/ui
Firmware/src/ui_binding
```

クローン後は `switch_design.ps1` を1回実行して Junction を作成する必要がある。

## Makefile との関係

Makefile (`HardwareDebug/src/ui/subdir.mk` 等) は `../src/ui/screens.c` のようにパスを参照する。
Junction が `src/ui` → `eez/eez00X/ui` に解決するため、**Makefile の変更は不要**。

ただし、デザイン間で画像やフォントのファイル数が異なる場合は、`images/subdir.mk` や `fonts/subdir.mk` の再生成が必要になる場合がある（e2 studio で「ソース・フォルダの更新」を実行）。
