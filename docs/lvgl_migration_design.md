# LVGL移行 設計書

## 概要

FULLMONI-WIDE のディスプレイミドルウェアを emWin (AppWizard) から LVGL v8.3 へ移行した作業の設計記録。
移行は段階的に Phase 1〜4 に分けて実施し、各フェーズで実機動作確認を行った。

### 移行の背景・目的

- emWin はライセンス費用が発生するプロプライエタリライブラリ
- AppWizard (emWin の UI デザインツール) はバイナリフォーマットで可搬性が低い
- LVGL v8.3 はオープンソース (MIT) で、SquareLine Studio (SLS) による UI 設計が可能
- SLS はコードエクスポート形式のため、UI 変更がソースコード管理しやすい
- 将来的なバリアント展開（レイアウト違いの複数バージョン）に対応しやすい

---

## ハードウェア構成

| 項目 | 内容 |
|---|---|
| MCU | ルネサス RX72T (240MHz, 4MB Flash, 1MB SRAM) |
| LCD | 800×256 px, RGB565 |
| LCD I/F | GLCDC GR2 フレームバッファ (0x00800000, 400KB) |
| DMAC | r_dmaca_rx FIT, ch4 を flush 専用に使用 |

### RAM レイアウト

| 領域 | アドレス | サイズ | 用途 |
|---|---|---|---|
| Low RAM | 0x00064040 | 112KB | .bss, スタック |
| GR2 フレームバッファ | 0x00800000 | 400KB | LCD出力 (RGB565) |
| RAM2 | 0x00864000 | 114KB | .bss2 (LVGL draw buf + heap) |
| ECCRAM | 0x00FF8000 | 32KB | .data, .istack, .ustack |

---

## Phase 1: ソフトウェア memcpy flush

### 目的
LVGL の最小動作確認。emWin と並行して LVGL をビルドできる環境を構築する。

### 実施内容

**LVGL の組み込み**
- `Firmware/src/lvgl/` に LVGL v8.3 を git submodule として追加 (release/v8.3 ブランチ)
- `Firmware/src/lv_conf.h` を作成
- ポートファイルを `Firmware/src/lv_port/` に作成
  - `lv_port_disp.c/h`: ディスプレイドライバ
  - `lv_port_tick.c/h`: tick ソース

**ディスプレイポート**
- draw buffer を RAM2 (`.bss2` セクション) に配置
  - `LV_ATTRIBUTE_LARGE_RAM_ARRAY` を `__attribute__((section(".bss2")))` に定義
  - LVGL heap (`work_mem_int`) も同セクションへ
- flush_cb: `memcpy` で GR2 フレームバッファ (0x00800000) に直接コピー
- GLCDC 初期化を `lv_port_disp_init()` 内で実施
  - `R_GLCDC_Open()` + `R_GLCDC_PinSet()` + `START_DISPLAY`
  - `platform.h` を先にインクルード必須 (R_BSP_ATTRIB_STRUCT_BIT_ORDER_* マクロのため)

**tick ソース**
- `LV_TICK_CUSTOM 1` を使用
- `ap_10ms()` タイマー割り込みから `s_tick_ms += 10` でカウント
- `lv_tick_inc()` は不要（`LV_TICK_CUSTOM=1` で不要なため削除）

**切り替えスイッチ**
- `settings.h` に `#define LVGL_ENABLE 1` を追加
- `main.c` で `#if LVGL_ENABLE` / `#else` により emWin と LVGL を排他的に切り替え
- `LVGL_ENABLE 0` で emWin に完全に戻せる構造を維持

**GUIConf RAM 削減**
- `qe_emwin_config.h`: `EMWIN_GUI_NUM_BYTES` を 112640 → 28672 に削減
  - LVGL_ENABLE=1 時は emWin の GUI heap が不要なため

**ビルドシステム**
- `HardwareDebug/src/lvgl/subdir.mk` および `lv_port/subdir.mk` を追加
- GNU Make 4.4 (Windows) の注意点:
  - `$(file > $@.in,...)` はシェルコマンド実行前に評価されるため、`mkdir -p` より先に実行される
  - 解決策: LVGL は flat output (`src/lvgl/`) + `$(eval $(call LVGL_RULE,...))` 明示ルール

### 結果
- LVGL テスト画面 (`lv_test_screen.c`) の動作確認
- memcpy による flush は CPU 使用率が高く、フレームレートは低い（Phase 3 で改善）

---

## Phase 2: DRW2D (ハードウェアアクセラレータ) 評価

### 目的
RX72T 内蔵の DRW2D エンジンによる描画加速効果を評価する。

### 評価結果
- LVGL v8.3 の DRW2D バックエンドを有効化して評価
- 矩形塗りつぶし等の基本操作では効果が確認されたが、
  複合描画 (arc, image rotation 等) では CPU 描画と差異がほぼなし
- ROM/RAM コスト増加に対して描画速度改善が見合わないと判断
- **Phase 2 は評価のみとしてスキップ**、DRW2D は無効のまま継続

---

## Phase 3: ダブルバッファ + 非同期 DMAC flush

### 目的
memcpy による CPU ブロッキング flush を DMAC 転送に置き換え、描画と転送を並列化してフレームレートを改善する。

### 実施内容

**draw buffer 構成**
- `LV_PORT_BUF_LINES = 16` (lv_port_disp.h)
- buf1, buf2 各 800×16行 = 25,600 bytes を RAM2 (.bss2) に配置
- LVGL がダブルバッファを使い buf1 描画中に buf2 を転送する並列動作

**DMAC flush の実装**
- DMAC ch4 を使用（USB FIT が ch0-3 を使用するため ch4 を選択）
- 初期化時に `R_DMACA_Open()` を 1 回だけ呼び、以降 Close しない
- flush_cb: `R_DMACA_Create()` + `Enable()` + `SoftReq()` で転送開始し即 return
- 転送完了 ISR (DMAC74I, vec124, ch4-7 共有) で `lv_disp_flush_ready()` を呼ぶ

**DMAC FIT (r_dmaca_rx) の重要な注意点**

| 注意点 | 詳細 |
|---|---|
| `R_DMACA_Close` を呼ぶと USB が壊れる | Close は `r_dmaca_module_disable()` を呼び DMAC 全停止。USB FIT は `g_locking_sw` を使わないため ch4 だけ Close しても全チャンネル停止する。ch4 を forever-open で保持することで回避。 |
| DMIST は読み取り専用 | `DMAC.DMIST.BYTE = 0x00` は効果なし。クリアは `R_DMACA_Control(ch, DMACA_CMD_DTIF_STATUS_CLR, &stat)` を使う。 |
| `DMCSL.DISEL = 1` 必須 | `DISEL=0 (BEGINNING_TRANSFER)` では転送完了後も割り込み信号がアサートされ ISR ループになる。`DMACA_ISSUES_INTERRUPT_TO_CPU_END_OF_TRANSFER (=1)` を指定する。 |
| 初期化時に DTIF クリア | 前回実行の残留 DTIF があると IRQ 有効化直後に誤って `flush_ready` が呼ばれる。`R_DMACA_Int_Enable()` の前に `DTIF_STATUS_CLR` を実行する。 |
| ISR 内で DTIF クリア必須 | FIT の `r_dmaca_intdmac74i_isr` は DMIST を読むが DTIF をクリアしない（FIT の設計）。コールバック内で `CMD_DTIF_STATUS_CLR` を呼ぶ。 |

**RAM2 レイアウト確認**

| 用途 | サイズ |
|---|---|
| draw_buf_1 (800×16行) | 25,600 bytes |
| draw_buf_2 (800×16行) | 25,600 bytes |
| LVGL heap (work_mem_int) | 32,768 bytes |
| GUIConf s_a_memory | 28,672 bytes |
| **合計** | **112,640 bytes ≤ 114,688 bytes (RAM2)** |

### 結果
- 非同期 DMAC flush により描画と転送が並列化
- フレームレートが Phase 1 比で大幅に改善、実用レベルに到達
- 実機動作確認済み

---

## Phase 4: SLS UI + データバインディング層

### 目的
emWin AppWizard で設計されていた UI を SquareLine Studio (SLS) で再設計し、
センサーデータとウィジェットを接続するデータバインディング層を実装する。

### UI 設計方針

**SquareLine Studio の選択理由**
- LVGL v8.3 に対応した公式 GUI デザインツール (SLS 1.6.0)
- C コードとして Export されるため git 管理が容易
- Export されたファイルは `Firmware/src/ui/` に配置し、手動変更は禁止
- UI の変更は SLS プロジェクトを編集して再 Export する運用

**ウィジェット構成 (Screen1)**

| ウィジェット名 | 種別 | 用途 |
|---|---|---|
| `ui_imageRPM` | Image | タコメーター針（回転画像） |
| `ui_ArcRPM` | Arc | タコメーター背景アーク |
| `ui_LblRPM` | Label | RPM 数値表示 |
| `ui_LblWaterTemp` | Label | 水温表示 |
| `ui_LblIAT` | Label | 吸気温表示 |
| `ui_LblOilTemp` | Label | 油温表示 |
| `ui_LblMAP` | Label | MAP 表示 |
| `ui_LblOilPress` | Label | 油圧表示 |
| `ui_LblBattery` | Label | バッテリー電圧表示 |
| `ui_BarWaterTemp` 〜 `ui_BarBattery` | Bar | 各センサーバーグラフ |
| `ui_ImgWarnMaster` 〜 `ui_ImgWarnFuel` | Image | 各種警告アイコン |
| `ui_MAPChart` | Chart | MAP チャートレコーダ |
| `ui_NotifyBox` | Panel | 通知オーバーレイ背景 |
| `ui_NotifyLabel` | Label | 通知オーバーレイテキスト |

### データバインディング層

**ファイル構成**
- `Firmware/src/ui_binding/ui_dashboard.h` : API 定義
- `Firmware/src/ui_binding/ui_dashboard.c` : 実装

**主要 API**

```c
void ui_dashboard_create(void);
// ui_init() 呼び出し + バー範囲設定 + MAPチャート初期化
// main.c の初期化シーケンスから 1 回呼び出す

void ui_dashboard_update(void);
// 毎フレーム (lv_timer_handler() 呼び出し前) に実行
// g_CALC_data / g_CALC_data_sm → 全ウィジェット更新

void ui_dashboard_set_notify(const char *msg, uint32_t bg_color);
// 通知オーバーレイを表示 (msg=NULL/"" で非表示)

void ui_dashboard_clear_notify(void);
// 通知オーバーレイを非表示
```

**タコメーター針の角度計算**
```
angle (0.1°単位) = 950 + rpm × 2650 / 9000
```
- RPM=0 → 95.0°、RPM=9000 → 360.0° (= 0°、アーク終端)
- `lv_img_set_angle()` に渡す（0.1° = 1 単位）

**バー範囲設定**

| バー | 最小 | 最大 | 単位 |
|---|---|---|---|
| WaterTemp | 0 | 130 | °C |
| IAT | 0 | 70 | °C |
| OilTemp | 0 | 160 | °C |
| MAP | 0 | 300 | kPa |
| OilPress | 0 | 500 | - |
| Battery | 100 | 160 | 0.1V (10.0〜16.0V) |

**警告アイコン閾値**

| 項目 | 条件 | アイコン |
|---|---|---|
| 水温 | ≥ 105°C | `ui_ImgWarnWater` |
| 油圧 | ≤ 50 | `ui_ImgWarnOilPress` |
| バッテリー | ≤ 11.5V | `ui_ImgWarnBattery` |
| 排気温 | AD3 ≥ 3000 (raw) | `ui_ImgWarnExhaust` |
| ブレーキ液 | AD2 ≥ 2000 (raw) | `ui_ImgWarnBrake` |
| シートベルト | AD4 ≥ 2000 (raw) | `ui_Image11` |
| 燃料 | fuel_per ≤ 10% | `ui_ImgWarnFuel` |
| マスター | 上記いずれか | `ui_ImgWarnMaster` |

**MAP チャートレコーダ**
- `LV_CHART_UPDATE_MODE_SHIFT` (スクロール方式)
- 200 点 × 50ms = 10 秒分の履歴
- `data_setLCD50ms()` の 50ms タイマーではなく、`ui_dashboard_update()` 内で `lv_tick_get()` により 50ms 経過を自律判定して push

**通知オーバーレイ**
- `ui_NotifyBox` / `ui_NotifyLabel` は SLS で設計・配置
  - 当初はプログラマティック生成（`lv_obj_create()`）で実装
  - SLS 側でデザインを管理するため SLS 配置に移行
- `ui_dashboard_set_notify()` で背景色を動的に変更（警告=赤、HOST ACCESS=緑など）

### emWin コードの LVGL 対応ガード

emWin の API 呼び出しを `#if !LVGL_ENABLE` でガードし、
`LVGL_ENABLE 1` でもコンパイルエラーが出ない構造に変更した。

**変更ファイルと対応内容**

| ファイル | 変更内容 |
|---|---|
| `settings.h` | `#define LVGL_ENABLE 1` を追加。emWin ヘッダのインクルードを `#if !LVGL_ENABLE` でガード |
| `dataregister.c` | `static APPW_PARA_ITEM` 宣言を `#if !LVGL_ENABLE` でガード |
| `dataregister.c` | `data_store()` 内の `APPW_SetVarData/DoJob/SetText` 呼び出しブロックを `#if !LVGL_ENABLE` でガード |
| `dataregister.c` | `data_setLCD10ms/50ms/100ms()` 内の APPW 呼び出しを各々 `#if !LVGL_ENABLE` でガード |
| `dataregister.c` | `master_warning_gui_update()` に `#if LVGL_ENABLE` ブランチを追加し `ui_dashboard_set_notify()` を呼び出す |
| `main.c` | `master_warning_gui_update()` を `#if LVGL_ENABLE / #else` の外（共通処理）に移動 |
| `main.c` | HOST ACCESS 時に `ui_dashboard_set_notify("HOST ACCESS", 0x00AA00u)` を呼び出す |

**`master_warning_gui_update()` の共通化の背景**

当初 `master_warning_gui_update()` は `#else`（emWin 側）にのみ配置されていた。
LVGL モードでは呼ばれないため `s_warning_displayed` フラグが常に 0 のままとなり、
警告音は繰り返し鳴るが通知オーバーレイが表示されないという問題が発生した。
`#if / #else` の外（LVGL/emWin 共通）に移動して解決。

### 警告音スケジューリング

`data_setLCD100ms()` の警告音発砲条件を以下のように変更した。

**変更前:**
```c
if (!s_warning_displayed && s_warning_sound_cooldown == 0)
```

**変更後:**
```c
if (s_warning_sound_cooldown == 0)
```

**変更理由:**
`s_warning_displayed` は「GUI 更新済み」を示すフラグ。
emWin 時代は `master_warning_gui_update()` が LVGL モードで呼ばれなかったため
`s_warning_displayed` が常に 0 のまま → 副作用として警告音が 3 秒ごとに繰り返し鳴っていた。
`master_warning_gui_update()` を共通化したことで `s_warning_displayed = 1` になり
以降の音が止まる問題が顕在化した。
`!s_warning_displayed` 条件を削除し、クールダウンタイマー (30 × 100ms = 3 秒) のみで
繰り返し間隔を制御する設計に統一した。

なお `speaker_play_warning()` は PCM クリップの 1 回再生であり、`s_patterns`
（OVERHEAT=無限、LOW_FUEL=2回等）を用いた警告種別ごとのパターン制御は
emWin 時代から未実装のまま（将来課題）。

### lv_conf.h の追加設定

SLS が生成するコードで使用するウィジェット・フォントを有効化した。

| 設定 | 値 | 理由 |
|---|---|---|
| `LV_USE_BTNMATRIX` | 1 | `LV_USE_KEYBOARD` の依存 |
| `LV_USE_DROPDOWN` | 1 | ui_helpers.c 使用 |
| `LV_USE_ROLLER` | 1 | ui_helpers.c 使用 |
| `LV_USE_SLIDER` | 1 | ui_helpers.c 使用 |
| `LV_USE_TEXTAREA` | 1 | ui_helpers.c 使用 |
| `LV_USE_KEYBOARD` | 1 | ui_helpers.c 使用 |
| `LV_USE_SPINBOX` | 1 | ui_helpers.c 使用 |
| `LV_USE_CHART` | 1 | MAPChart 使用 |
| `LV_FONT_MONTSERRAT_32` | 1 | SLS フォント指定 |

`lvgl/subdir.mk` には `extra/widgets/keyboard`、`extra/widgets/spinbox`、
`extra/widgets/chart` のソースを追加した。

### ビルドシステム追加ファイル

| ファイル | 内容 |
|---|---|
| `HardwareDebug/src/ui/subdir.mk` | SLS 生成 ui/ |
| `HardwareDebug/src/ui/screens/subdir.mk` | SLS 生成 screens/ |
| `HardwareDebug/src/ui/images/subdir.mk` | SLS 生成 images/ |
| `HardwareDebug/src/ui/fonts/subdir.mk` | SLS 生成 fonts/ |
| `HardwareDebug/src/ui/components/subdir.mk` | SLS 生成 components/ |
| `HardwareDebug/src/ui_binding/subdir.mk` | ui_dashboard.c |

---

## ビルドバリアント

`tools/build_variants.ps1` により aw001 / aw002 の 2 バリアントをビルドする。
バリアントはユーザがメータレイアウトを好みで選択するための機能であり、
LVGL 版でも継続する予定（将来的に各バリアントが異なる SLS UI を持つ構成）。

---

## 今後の予定

### Phase 5: emWin 完全撤去（不要リソースのクリーンアップ）

- `aw/` ディレクトリ (AppWizard 生成リソース) の削除
- `smc_gen/r_emwin_rx/` の削除
- `settings.h` / `dataregister.c` 等の `#if !LVGL_ENABLE` ガードと `#else` ブランチの除去
- `qe_emwin_config.h` 等の emWin 設定ファイルの削除
- aw002 バリアントの LVGL 版 SLS UI への移行

**注意:** aw001/aw002 のバリアント機能自体は残す。削除対象は emWin リソースのみ。
