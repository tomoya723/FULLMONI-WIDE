# EEZ Studio 移行メモ

## 生成ファイル
- `tools/sls_to_eez.py` - SLS→EEZ 変換スクリプト
- `Firmware/eez/fullmoni.eez-project` - 生成された EEZ プロジェクト（49ウィジェット、4フォント、16ビットマップ）
- `Firmware/eez/*.png` + `*.ttf` - `lv002/SLS_Project/assets/` からコピーしたアセットファイル

## 主な設計判断
- LVGL バージョン: 8.3（プロジェクト設定で指定）
- flowSupport: false（EEZ Flowロジックなし、UIレイアウトのみ）
- すべてのウィジェット識別子は `ui_dashboard.c` の extern 参照と完全一致
- 座標は CENTER アライメント（SLS出力に合わせた）

## ウィジェット数
- パネル 4個（ContainerDashboard, ContainerOpening, ContainerTelltale, NotifyBox）
- 画像 12個（背景 + 針 + 警告アイコン）
- バー 7個（WaterTemp, IAT, OilTemp, MAP, OilPress, Battery, FUEL）
- アーク 1個（RPM）
- ラベル 25個（値 + 単位 + 名称）

## ビルド統合（完了）
- EEZ の build.files を修正（空の [] だった）→ 12個のテンプレートエントリを追加
- flowSupport=false（EEZ Flow ランタイムなし）
- EEZ が生成するファイル: screens.c/h, ui.c/h, images.c/h, fonts.h, styles.c/h, actions.h, vars.h, structs.h
- 画像ファイル: SLS のオリジナルファイル（RLE圧縮、約50%小さい）を使用し、images.h でシンボルエイリアスを定義
  - SLS は `ui_img_back5_png` を定義、EEZ は `img_ui_img_back5_png` を期待 → #define エイリアス
- フォントファイル: EEZ 生成ファイルを使用（SLS と同程度のサイズ）
  - SLS `ui_font_Font1` → EEZ `ui_font_ui_font_font1`
- eez_compat.h: SLS ウィジェット名（ui_LblRPM）→ EEZ の objects.member パターンへの49個のマクロ定義
- ファームウェアビルド成功: text=1.95MB、4MB ROM に収まる

## EEZ Flow 移行（実機検証済み ✅ 2026-03-25）

### アーキテクチャ
- EEZ Studio が `eez-flow.cpp` 単一ファイルバンドル（約360KB）を生成、EEZ Framework 全体を含む
- `libeez-framework.a`（別ライブラリ）は **廃止** — eez-flow.cpp バンドルに置き換え
- `Firmware/lib/eez-framework/` クローンはビルドに不要

### 完了項目
- EEZ Studio Flow モード: `flowSupport: true`, `generateSourceCodeForEezFramework: true`
- EEZ Studio `destinationFolder: "../src/ui"`（eez/ フォルダからの相対パス → Firmware/src/ui/ に出力）
- 生成ファイル: `eez-flow.cpp`, `eez-flow.h`, `ui.c`（assets[3228] + eez_flow_init/tick）, `vars.h`（FLOW_GLOBAL_VARIABLE_RPM）
- `Firmware/makefile.defs`: eez-flow.cpp を rx-elf-g++（C++14）でコンパイル、USER_OBJS に追加
- `eez-flow.cpp` にパッチ適用: `#if LV_USE_CALENDAR` / `#if LV_USE_TABVIEW` ガードとスタブ関数
- `__dso_handle` リンクエラー回避のため `-fno-use-cxa-atexit` フラグを追加（ベアメタル C++）
- `ui_image_ui_img_fmw_op0_png.c` を makefile.defs で追加（e2 studio の subdir.mk には未登録）
- **ファームウェアビルド成功**: text=2,012,393 data=5,880 bss=119,646（合計約2.14MB、4MB ROM に収まる）

### 主なビルド修正
1. Calendar/Tabview: eez-flow.cpp に `#if LV_USE_CALENDAR`/`#if LV_USE_TABVIEW` ガードを追加（lv_conf.h で LV_USE_CALENDAR=0）
2. `__dso_handle`: g++ フラグに `-fno-use-cxa-atexit` を追加（ベアメタル C++ の静的デストラクタ問題）
3. 画像シンボル名の不一致: EEZ Flow は `ui_image_*` ファイルを `img_*` シンボルで生成（非Flow版と命名規則が異なる）

### ビルドコマンド
- ファームウェア: 通常の `Build (GCC-RX)` タスク（makefile.defs が eez-flow.cpp を自動処理）
- 別途ライブラリビルドは不要

### MCU フラグ（一致必須）
- `-mcpu=rx72t -misa=v3 -mlittle-endian-data`
- ファームウェア C ファイル: `-std=gnu99`、eez-flow.cpp: `-std=gnu++14`

### 注意: EEZ Studio 再ビルドで eez-flow.cpp のパッチが消える
- EEZ Studio でビルドするたびに Calendar/Tabview ガードを再適用する必要がある
- TODO: 自動パッチスクリプトを作成して対応する

### 今後の作業
1. linker_script.ld に EEZ_RESOURCES 領域を追加（~0xFFD00000、1MB）
2. 生成された ui.c を変更し、固定 Flash アドレスからアセットを読み込むようにする
3. eez-flow.cpp の Calendar/Tabview ガード用パッチスクリプトを作成
4. 実機テスト
5. e2 studio プロジェクトをリフレッシュして新しい `ui_image_*` ファイルを subdir.mk に反映

## 残りのクリーンアップ（SLS 移行時の残骸）
1. 旧 SLS ファイルの削除: ui_helpers.c/h, ui_events.h, screens/, components/, filelist.txt
2. EEZ 重複画像ファイルの削除: ui_image_ui_img_*.c（SLS オリジナルを使用中）
3. 旧 SLS フォントファイルの削除: ui_font_F*.c（EEZ フォントを使用中）
4. 不要フォルダの削除: `aw001/`, `aw002/`, `lv002/SLS_Project/`
5. emWin の削除: `smc_gen/r_emwin_rx/`, `qe_emwin_config.h`
6. 実機でのハードウェアテスト（LCD出力の確認）
