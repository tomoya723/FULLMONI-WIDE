# EEZ UI カスタマイズ機能 設計書

## 1. 概要

### 1.1 目的
ユーザーが EEZ Studio で画面デザインを自由にカスタマイズし、FULLMONI-WIDE Terminal（Windows HOST アプリ）から MCU の **UI 専用フラッシュ領域だけ** を書き換えられるようにする。ユーザーはコンパイラやビルド環境を意識する必要がない。

### 1.2 ユーザーから見た操作フロー
```
1. EEZ Studio でデザイン編集
2. EEZ Studio で Build (Ctrl+Shift+B)
3. FULLMONI-WIDE Terminal の「UI書き込み」ボタンを押す
4. EEZ Build 出力フォルダを選択
5. 自動でコンパイル → リンク → UIバイナリ生成 → eezwrite CDC転送
6. MCUリブート → 新しいデザインで起動
```

### 1.3 実現方式の要約
- **配布**: 今まで通りフル `.bin` を配布（Bootloader 経由の通常 FW アップデート）
- **UI カスタマイズ**: EEZ UI コードを **専用フラッシュ領域 (EEZ_UI)** にコンパイル＆リンクし、`eezwrite` コマンドで **その領域だけ** を部分書き換え
- **HOST アプリ**: GCC-RX コンパイラを内蔵。EEZ 生成 `.c` をコンパイル → Firmware.elf のシンボルテーブルを使ってリンク → UI セクションバイナリ抽出 → eezwrite で CDC 転送

---

## 2. 調査記録（2026-03-25）

本セクションは、EEZ Studio の出力構造を実機プロジェクトで検証した結果をまとめたものである。設計方針はこの調査結果に基づいている。

### 2.1 EEZ Studio LVGL 出力の二重構造

EEZ Studio が LVGL 向けに Build すると、**2系統の全く異なる出力**が生成される：

| | `ui.c` の `assets[]` | `screens.c` |
|---|---|---|
| **形式** | バイナリ配列（インタプリタ用データ） | C関数呼び出しのソースコード |
| **内容** | ページ遷移ロジック、変数バインディング、アクション定義、テーマカラー定義 | 全ウィジェットの生成コード（色、サイズ、位置、フォント、フラグ等すべて） |
| **実行方式** | eez-flow.cpp のインタプリタが解釈 | GCC-RX でコンパイル → ネイティブ実行 |
| **差し替え** | バイナリ書き換えで可能 | **コンパイル必須** |

### 2.2 実証: assets[] と screens.c の差分検証

RPM ゲージの色を赤→青に変更した type2 デザインで `git diff` した結果：

- **`assets[]`**: オリジナルと type2 で **完全に同一**（SHA ハッシュ一致）
- **`screens.c`**: 色の変更が `lv_obj_set_style_arc_color(obj, lv_color_hex(0xff0800ff), ...)` として出力（オリジナルは `0xffff0000`）

**結論**: ウィジェットのビジュアル変更は assets[] に一切反映されず、screens.c のコンパイルが不可避。

### 2.3 EEZ Studio の「バイナリ出力」は存在しない

EEZ Studio の設定に「Generate source code for EE...」トグルがあるが、これを OFF にしても `.res` バイナリファイルは生成されなかった。この設定は **EEZ Flow ランタイムの C++ ソース生成** を制御するもので、LVGL ウィジェットのバイナリ出力とは無関係。EEZ Studio の LVGL モードでは、**常に C ソースコード出力のみ**。

### 2.4 テーマカラーの限界

`assets[]` 内の `colorsDefinition` にテーマカラーは含まれるが、`screens.c` で **直接値指定**（`lv_color_hex(0xffXXXXXX)`）された色はテーマシステムを経由しない。

- 現在のプロジェクトは直接値指定 → assets[] だけの差し替えでは**色すら変えられない**
- テーマ参照に変更すれば色だけは assets[] 経由で変更可能
- ただし、**位置・サイズ・フォント・レイアウトは常に screens.c** にあり、テーマの範囲外
- テーマ方式ではデザインの自由度が大幅に制限されるため、本プロジェクトでは不採用

### 2.5 採用しなかった代替案

| 代替案 | 不採用理由 |
|---|---|
| assets[] バイナリ差し替えのみ | ビジュアル変更が反映されない（上記実証） |
| テーマカラーのみ変更 | 色以外（位置、サイズ、フォント）を変えられない。デザインの自由度を求めて EEZ Studio を使うのであって、制約があっては意味がない |
| firmware_base.a 配布 + フルリンク | .o / .a ファイルの配布はユーザーにとって不自然。フル FW 書き換えも不要に重い |
| Bootloader 経由のフル FW 更新 | UI だけ変えたいのに全体を書き換える必要がない。eezwrite による部分書き換えで十分 |

---

## 3. EEZ Flow の特性

EEZ Flow は **ハイブリッドアーキテクチャ** であり、純粋なインタプリタでも純粋なコード生成器でもない。

### 3.1 実行モデル

```
EEZ Flow ランタイム (eez-flow.cpp, 101KB compiled)
   │
   ├─ インタプリタ部: assets[] を解釈
   │   - ページ遷移制御 (replacePageHook)
   │   - 変数のバインディング評価
   │   - アクションのディスパッチ (executeLvglAction)
   │   - テーマカラーの管理 (g_themeColors)
   │
   └─ ネイティブコード呼び出し部: screens.c の関数を直接呼ぶ
       - create_screens()  ← extern "C" で直接リンク
       - tick_screen()     ← extern "C" で直接リンク
```

**ロジック（Flow）はインタプリタ、ビジュアル（Widget）はネイティブコード**という分離。この設計は LVGL の API が C 関数呼び出しベースであることに起因しており、EEZ 側の不完全さではなく、**LVGL の API 特性に忠実な出力**。

### 3.2 assets[] の役割（限定的）

`assets[]` はヘッダタグ `0x5A45457E` を持つバイナリブロブで、以下を含む：
- `colorsDefinition`: テーマカラー配列
- ページ遷移ロジック（Flow グラフ）
- 変数定義・アクション定義の名前テーブル

しかし、ウィジェットの生成（`lv_obj_create`, `lv_arc_create` 等）やプロパティ設定（色、位置、サイズ）は一切含まない。

### 3.3 eez-flow.cpp の内部構造

`eez_flow_init()` が全体の初期化を行い、以下の順序で実行：
1. `loadMainAssets(assets, assetsSize)` — assets[] バイナリを解析
2. `eez::flow::start(g_mainAssets)` — Flow エンジン起動
3. `create_screens()` — **screens.c の関数を直接呼び出し**
4. `replacePageHook(1, 0, 0, 0)` — 初期ページ表示

`eez_flow_tick()` は毎フレーム呼ばれ、Flow エンジンの評価と `tick_screen()` のネイティブ呼び出しを行う。

### 3.4 意味合い

このハイブリッド構造は二つの重要な帰結を持つ：
1. **assets[] だけの差し替えではビジュアルを変更できない** — ウィジェットの見た目は全て screens.c のネイティブコード
2. **screens.c の差し替えにはコンパイルが必須** — C ソースコードであるため、バイナリ配布ではなくツールチェーンが必要

---

## 4. 技術的根拠

### 4.1 なぜ assets[] だけの差し替えでは不十分か
EEZ Studio の LVGL 向け Build 出力は以下の2系統に分かれる：

| 出力ファイル | 内容 | 格納方式 |
|---|---|---|
| `ui.c` の `assets[]` | Flow 制御データ（ページ遷移、変数、アクション、テーマカラー定義） | バイナリ配列 |
| `screens.c` | ウィジェット生成コード（色、サイズ、位置、フラグ、フォント等すべて） | C 関数呼び出し |

ウィジェットの色・サイズ・位置は `screens.c` の C 関数呼び出し（例: `lv_obj_set_style_arc_color()`, `lv_obj_set_pos()`, `lv_obj_set_size()`）として出力されるため、**コンパイルが必須**。

### 4.2 EEZ UI セクションに含めるファイル

| ファイル | text サイズ | UI 変更で変わるか | 備考 |
|---|---|---|---|
| `screens.c` | 11 KB | ほぼ毎回 | ウィジェット生成コード |
| `ui.c` | 8 KB | ほぼ毎回 | assets[], ui_init/ui_tick |
| `images.c` | 0.5 KB | 画像変更時 | 画像リスト |
| `styles.c` | 4.6 KB | スタイル変更時 | スタイル定義 |
| `eez-flow.cpp` | 101 KB | 変わらない※ | EEZ Flow ランタイム |
| **合計** | **~125 KB** | — | — |

※ `eez-flow.cpp` は UI 変更では変わらないが、以下の理由で UI セクションに含める：
- eez-flow.cpp が `create_screens()` / `tick_screen()` を **直接呼び出し** (`extern "C"`) しており、分離するとランタイム改修が必要
- eez-flow と assets[] のフォーマットはバージョン依存で、セットで管理すべき
- 125KB の Flash 書き換えは NOR Flash 上で数秒以内に完了し、実用上問題ない

### 4.3 コンパイル依存関係

EEZ UI ファイルがインクルードするヘッダ：
```
screens.c → screens.h, images.h, fonts.h, actions.h, vars.h, styles.h, ui.h, <string.h>
ui.c      → ui.h, screens.h, images.h, actions.h, vars.h
images.c  → images.h
styles.c  → styles.h, images.h, fonts.h, ui.h, screens.h
eez-flow.cpp → eez-flow.h, lvgl/lvgl.h
```

EEZ Studio Build が生成するヘッダ: `ui.h`, `screens.h`, `images.h`, `styles.h`, `actions.h`, `vars.h`, `fonts.h`
外部依存: `lvgl/lvgl.h`, `eez-flow.h`

---

## 5. アーキテクチャ

### 5.1 システム全体像

```
┌─────────────────────────────────────────────┐
│               Code Flash (4MB)                │
│                                               │
│  ┌─────────────────────────────────────────┐  │
│  │ Main Firmware (.text, .rodata)           │  │
│  │ - LVGL ライブラリ                        │  │
│  │ - ドライバ、CAN、パラメータ管理         │  │
│  │ - ui_binding/ (action/var コールバック)   │  │
│  │ - フォントデータ、画像データ             │  │
│  │ 0xFFC20040 ~ (約1.2MB)                  │  │
│  └─────────────────────────────────────────┘  │
│                                               │
│  ┌─────────────────────────────────────────┐  │
│  │ EEZ_UI Section (差し替え可能)            │  │
│  │ - eez_ui_entry (関数テーブル)            │  │
│  │ - eez-flow.cpp (ランタイム)              │  │
│  │ - screens.c, ui.c, images.c, styles.c    │  │
│  │ 0xFFE00000 ~ (128KB)                    │  │
│  └─────────────────────────────────────────┘  │
│                                               │
│  ┌─────────────────────────────────────────┐  │
│  │ STARTUP_IMAGE (起動画像)                 │  │
│  │ 0xFFE20000 ~ (384KB)                    │  │
│  └─────────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
```

**分離原則**: Main Firmware は EEZ_UI Section の関数を **関数テーブル（間接呼び出し）** 経由で呼ぶ。EEZ_UI Section は Main Firmware の関数を **リンク時アドレス解決** (`--just-symbols`) で呼ぶ。

### 5.2 フラッシュメモリ配置

```
0xFFC20000 ┌────────────────────┐
           │ FW_HEADER (64B)    │
0xFFC20040 ├────────────────────┤
           │ Main Firmware      │ ← .text, .rvectors, .rodata
           │ (LVGL, drivers,    │    EEZ UI 以外の全コード
           │  ui_binding, fonts)│
           │ ~1.2MB             │
           ├ ─ ─ ─ ─ ─ ─ ─ ─ ─┤
           │ (未使用領域)       │ ← ~700KB 空き
0xFFE00000 ├════════════════════┤ ★ EEZ_UI Section (128KB)
           │ eez_ui_entry       │ ← 関数テーブル (magic, ui_init, ui_tick)
           │ eez-flow.cpp code  │ ← Flow ランタイム
           │ screens.c code     │ ← ウィジェット生成
           │ ui.c code + assets │ ← Flow データ
           │ images.c, styles.c │
0xFFE20000 ├────────────────────┤
           │ STARTUP_IMAGE      │ (384KB, 起動画像)
0xFFE80000 ├────────────────────┤
           │ (未使用)           │
0xFFFFFF80 ├────────────────────┤
           │ .exvectors         │
0xFFFFFFFF └────────────────────┘
```

### 5.3 EEZ_UI エントリテーブル

EEZ_UI セクション先頭に配置する関数テーブル。Main Firmware はこのテーブル経由で UI を呼び出す。

```c
/* eez_ui_entry.h — Firmware と UI Section の共通定義 */
#define EEZ_UI_BASE_ADDR  0xFFE00000
#define EEZ_UI_MAGIC      0x45455A55  /* "EEZU" */
#define EEZ_UI_API_VER    1

typedef struct {
    uint32_t magic;           /* EEZ_UI_MAGIC */
    uint32_t api_version;     /* API バージョン */
    uint32_t section_size;    /* セクション全体バイト数 */
    uint32_t reserved;
    void (*ui_init)(void);    /* ui_init() エントリポイント */
    void (*ui_tick)(void);    /* ui_tick() エントリポイント */
} eez_ui_entry_t;
```

**UI Section 側** (`eez_ui_entry.c` — EEZ UI と一緒にコンパイル):
```c
#include "eez_ui_entry.h"
#include "ui.h"

__attribute__((section(".eez_ui_entry")))
const eez_ui_entry_t eez_ui_entry = {
    .magic       = EEZ_UI_MAGIC,
    .api_version = EEZ_UI_API_VER,
    .section_size = 0,  /* リンカスクリプトで埋めるか、書き込み時に設定 */
    .reserved    = 0,
    .ui_init     = ui_init,
    .ui_tick     = ui_tick,
};
```

**Firmware 側** (既存 `ui_dashboard.c` 等を修正):
```c
#include "eez_ui_entry.h"

#define EEZ_UI_ENTRY  ((const eez_ui_entry_t *)EEZ_UI_BASE_ADDR)

static bool eez_ui_valid(void) {
    return (EEZ_UI_ENTRY->magic == EEZ_UI_MAGIC &&
            EEZ_UI_ENTRY->api_version == EEZ_UI_API_VER);
}

void ui_start(void) {
    if (eez_ui_valid()) {
        EEZ_UI_ENTRY->ui_init();
    }
}

void ui_update(void) {
    if (eez_ui_valid()) {
        EEZ_UI_ENTRY->ui_tick();
    }
}
```

### 5.4 シンボル解決方式 (`--just-symbols`)

UI Section のコードは Main Firmware 内の関数を呼ぶ必要がある（LVGL 関数、action/var コールバック等）。
GCC-RX の `--just-symbols` オプションを使い、**リンク時に** Firmware.elf からシンボルアドレスを取得する。

```bash
# UI Section のリンク（HOST アプリ内部で実行）
rx-elf-gcc \
  -o eez_ui.elf \
  eez_ui_entry.o screens.o ui.o images.o styles.o eez-flow.o \
  -T eez_ui_linker.ld \
  -Wl,--just-symbols=Firmware.elf \
  -nostartfiles -nodefaultlibs -nostdlib
```

**シンボル解決の流れ:**
- `lv_arc_create()`, `lv_obj_set_pos()` 等 → Firmware.elf のアドレスをそのまま使用
- `action_*()`, `var_*()` → Firmware.elf の ui_binding/ コールバックアドレス
- `malloc()`, `memcpy()` 等の C 標準ライブラリ → Firmware.elf に含まれるもので解決

### 5.5 `objects` 変数の扱い

`objects_t objects;` は `screens.c` で定義され、Firmware 側 (`ui_binding/`) からも直接参照される。

**問題**: `objects` のアドレスは UI Section リンク時に決まるが、Firmware 側は自身のビルド時のアドレスしか知らない。

**解決策**: Firmware フルビルド時に `objects` は EEZ_UI 用の BSS 領域に配置し、そのアドレスを `.elf` シンボルとして固定する。UI Section ビルド時は `--just-symbols` で同じアドレスに配置。

具体的には：
1. Firmware リンカスクリプトで `objects` をEEZ_UI 関連 BSS として固定アドレスに配置
2. UI Section リンク時、`screens.c` の `objects_t objects;` 定義を `extern` に変換（ビルドパイプラインで自動処理）
3. `--just-symbols=Firmware.elf` により Firmware 側と同じアドレスで解決

---

## 6. ビルドフロー

### 6.1 開発者: フルビルド（リリース用）

通常通り GCC-RX でフルビルド。**追加手順**:

```
1. make -j8 all                    → Firmware.elf, Firmware.bin
2. rx-elf-objcopy でシンボルファイル生成:
   rx-elf-objcopy --strip-debug Firmware.elf Firmware_symbols.elf
   （または rx-elf-nm で .sym テキスト生成）
3. UI カスタマイズキットのパッケージング（後述）
```

フルビルドでは EEZ UI コードも EEZ_UI セクションに自動配置される（リンカスクリプトに EXCLUDE_FILE で誘導）。

### 6.2 ユーザー: UI カスタマイズ（HOST アプリ内部で自動実行）

```
Step 1: EEZ Build 出力フォルダの .c/.h を収集
        screens.c, ui.c, images.c, styles.c + 生成ヘッダ

Step 2: screens.c を前処理
        "objects_t objects;" → "extern objects_t objects;" に変換

Step 3: GCC-RX でコンパイル（5ファイル）
        rx-elf-gcc -Os -ffunction-sections -fdata-sections \
          -mcpu=rx72t -misa=v3 -mlittle-endian-data \
          -mtfu=intrinsic,mathlib -mtfu-version=v2 \
          -std=gnu99 \
          -I<kit>/include/ \
          -c screens.c -o screens.o
        
        rx-elf-g++ -Os -ffunction-sections -fdata-sections \
          -mcpu=rx72t -misa=v3 -mlittle-endian-data \
          -std=gnu++14 -fno-exceptions -fno-rtti \
          -I<kit>/include/ \
          -c eez-flow.cpp -o eez-flow.o

Step 4: リンク（UI Section のみ、Firmware シンボル参照）
        rx-elf-gcc -mcpu=rx72t -misa=v3 -mlittle-endian-data \
          -o eez_ui.elf \
          eez_ui_entry.o screens.o ui.o images.o styles.o eez-flow.o \
          -T eez_ui_linker.ld \
          -Wl,--just-symbols=<kit>/Firmware_symbols.elf \
          -nostartfiles -nodefaultlibs -nostdlib

Step 5: UI セクションバイナリ抽出
        rx-elf-objcopy -O binary \
          --only-section=.eez_ui_entry \
          --only-section=.eez_ui_text \
          --only-section=.eez_ui_rodata \
          eez_ui.elf eez_ui.bin

Step 6: eezwrite で CDC 転送（UI セクションのみ書き換え）
        eezwrite コマンド → Flash Erase (128KB) → バイナリ転送 → MCU リブート
```

---

## 7. 配布物

### 7.1 通常リリース（従来通り）

| 配布物 | 内容 |
|---|---|
| `FULLMONI-1.x.x-awXXX.bin` | フルファームウェア（公式バリアント） |
| `release-manifest.json` | バージョン情報、チェックサム |

### 7.2 UI カスタマイズキット（リリースに追加）

```
eez_ui_kit_v1.x.x/
├── Firmware_symbols.elf     # シンボルテーブル（関数アドレス解決用）
├── eez_ui_linker.ld         # UI Section 用リンカスクリプト
├── eez_ui_entry.c           # エントリテーブルソース
├── eez_ui_entry.h           # エントリテーブル共通ヘッダ
├── include/                 # コンパイル用ヘッダ（.h のみ）
│   ├── lvgl/                # LVGL 8.3 ヘッダ
│   │   ├── lvgl.h
│   │   ├── lv_conf.h
│   │   └── ...
│   ├── eez-flow.h
│   └── fonts.h, actions.h, vars.h  # Firmware 側定義
├── eez-flow.cpp             # EEZ Flow ランタイムソース
├── template.eez-project     # ベーステンプレート EEZ プロジェクト
└── manifest.json            # キットメタデータ
```

```json
// manifest.json
{
    "kit_version": "1.0.0",
    "firmware_version": "1.2.0",
    "eez_studio_version_min": "0.11.0",
    "gcc_rx_version": "8.3.0",
    "eez_ui_base_addr": "0xFFE00000",
    "eez_ui_max_size": 131072,
    "api_version": 1
}
```

**重要**: `Firmware_symbols.elf` と `.bin` は必ず同一ビルドからの対。バージョン不一致のシンボルを使うと正しく動作しない。

---

## 8. FULLMONI-WIDE Terminal への統合

### 8.1 新規ページ: 「UI カスタマイズ」

既存ページに加えて「UI Customize」ページを追加。

**UI 要素：**
- EEZ Build 出力フォルダ選択（ファイルピッカー）
- プレビュー（EEZ 出力のスクリーンショットがあれば表示）
- 「ビルド＆書き込み」ボタン
- 進捗バー + ログ表示
- 現在の Kit バージョン / Firmware バージョン表示

### 8.2 GCC-RX 組み込み方式

**方式A（推奨）: GCC-RX バイナリを Terminal アプリにバンドル**

```
FULLMONI-WIDE-Terminal/
├── FULLMONI-WIDE-Terminal.exe
├── gcc-rx/                       # GCC-RX ツールチェーン（最小構成）
│   ├── bin/
│   │   ├── rx-elf-gcc.exe
│   │   ├── rx-elf-g++.exe
│   │   ├── rx-elf-ld.exe
│   │   ├── rx-elf-objcopy.exe
│   │   └── cc1.exe, cc1plus.exe 等
│   ├── lib/gcc/rx-elf/<ver>/
│   └── rx-elf/lib/
├── eez_ui_kit/                   # UI カスタマイズキット
│   ├── Firmware_symbols.elf
│   ├── eez_ui_linker.ld
│   ├── include/
│   └── ...
└── ...
```

推定サイズ: GCC-RX 最小構成 ~50-70MB（ZIP 圧縮で 20-30MB）

**方式B: GCC-RX を別途インストール要求**

Terminal 初回起動時に「Renesas GCC-RX が必要です」と案内。

### 8.3 新規サービス: `EezUiBuildService.cs`

```csharp
public class EezUiBuildService
{
    // EEZ Build 出力フォルダのバリデーション
    public bool ValidateEezOutput(string folderPath);
    
    // コンパイル + リンク + UI バイナリ生成
    public async Task<BuildResult> BuildUiSectionAsync(
        string eezOutputFolder,
        string kitPath,
        IProgress<BuildProgress> progress,
        CancellationToken ct);
}

public record BuildResult(bool Success, string BinPath, int BinSize, string[] Errors);
public record BuildProgress(int Percent, string Stage, string Detail);
```

**ビルドステージ:**
1. `Validating EEZ output...` (5%)
2. `Preprocessing screens.c...` (10%)
3. `Compiling screens.c...` (20%)
4. `Compiling ui.c...` (30%)
5. `Compiling eez-flow.cpp...` (50%)
6. `Compiling images.c, styles.c...` (60%)
7. `Compiling eez_ui_entry.c...` (65%)
8. `Linking UI section...` (80%)
9. `Extracting binary...` (90%)
10. `Validating size...` (95%)
11. `Build complete` (100%)

### 8.4 統合フロー

```
[ビルド＆書き込み] ボタン押下
  ↓
EezUiBuildService.BuildUiSectionAsync()
  ├── screens.c 前処理 (objects extern 変換)
  ├── rx-elf-gcc / rx-elf-g++ でコンパイル（6ファイル）
  ├── rx-elf-gcc でリンク (--just-symbols)
  ├── rx-elf-objcopy で UI バイナリ抽出
  └── サイズ検証 (128KB 以内)
  ↓
EezWriteService.WriteUiSectionAsync()   ← eezwrite プロトコル
  ├── eezwrite コマンド送信
  ├── バイナリサイズ送信
  ├── Flash Erase 待機
  └── バイナリストリーム転送
  ↓
MCU リブート → 新しい UI デザインで起動
```

---

## 9. eezwrite プロトコル（再設計）

既存の `eezwrite` 実装を拡張し、コンパイル済み UI セクション全体を転送する。

### 9.1 プロトコル仕様

```
HOST → MCU: "eezwrite\r\n"
MCU  → HOST: "EEZWRITE_READY\r\n"
HOST → MCU: 4 bytes (size, little-endian)  ※最大 128KB = 0x20000
MCU  → HOST: '.' (ACK - Flash Erase 完了)
HOST → MCU: data (16KB チャンク × N)
MCU  → HOST: '.' (各チャンク ACK)
...
MCU  → HOST: "Done\r\n"
MCU  : 自動リブート
```

### 9.2 Firmware 側実装変更

`startup_image_write.c` の `eez_resource_write_mode()` を更新:
- 書き込み先アドレス: `0xFFE00000` (EEZ_UI_BASE_ADDR)
- 最大サイズ: `0x20000` (128KB)
- Erase: 128KB 分の Flash ブロックを消去（32KB × 4 ブロック）
- Write: 128B 単位で書き込み（既存ロジック流用）
- 書き込み後: magic チェック → API version チェック → リブート

### 9.3 書き込み後の検証

MCU リブート時に `eez_ui_valid()` が検証:
- magic == `0x45455A55`
- api_version == `EEZ_UI_API_VER`
- 不正データの場合: 既定 UI なし（安全側）or エラー表示

---

## 10. Firmware 側変更

### 10.1 リンカスクリプト変更

`linker_script.ld` に EEZ_UI メモリ領域を追加:

```
MEMORY
{
    ...
    ROM : ORIGIN = 0xFFC20040, LENGTH = 0x1DFFC0  /* 0xFFC20040 ~ 0xFFDFFFFF */
    EEZ_UI : ORIGIN = 0xFFE00000, LENGTH = 0x20000  /* 128KB */
    STARTUP_IMAGE : ORIGIN = 0xFFE20000, LENGTH = 0x60000
    ...
}
```

EEZ UI オブジェクトを EEZ_UI セクションに誘導:
```
.text 0xFFC20040 : AT(0xFFC20040)
{
    /* EEZ UI ファイルを除外 */
    EXCLUDE_FILE(*screens.o *ui.o *images.o *styles.o *eez-flow.o *eez_ui_entry.o)
    *(.text .text.*)
    *(P)
    etext = .;
} >ROM

.eez_ui 0xFFE00000 : AT(0xFFE00000)
{
    _eez_ui_start = .;
    KEEP(*eez_ui_entry.o(.eez_ui_entry))
    *eez_ui_entry.o(.text .text.* .rodata .rodata.*)
    *eez-flow.o(.text .text.* .rodata .rodata.*)
    *screens.o(.text .text.* .rodata .rodata.*)
    *ui.o(.text .text.* .rodata .rodata.*)
    *images.o(.text .text.* .rodata .rodata.*)
    *styles.o(.text .text.* .rodata .rodata.*)
    _eez_ui_end = .;
} >EEZ_UI
```

同様に `.rodata` セクションも EXCLUDE_FILE で分離。

### 10.2 新規ファイル

| ファイル | 内容 |
|---|---|
| `src/eez_ui_entry.h` | エントリテーブル構造体定義（Firmware/UI共通） |
| `src/eez_ui_entry.c` | エントリテーブル実体（UI Section に配置） |

### 10.3 既存ファイル変更

| ファイル | 変更内容 |
|---|---|
| `src/ui_binding/ui_dashboard.c` | `ui_init()` / `ui_tick()` 直接呼び出し → `eez_ui_entry` 経由の間接呼び出しに変更 |
| `src/startup_image_write.c` | `eez_resource_write_mode()` を 128KB 対応に拡張 |
| `src/linker_script.ld` | EEZ_UI メモリ領域追加、EXCLUDE_FILE による誘導 |

---

## 11. 分離における技術的課題

### 11.1 eez-flow.cpp と screens.c の密結合
- `eez_flow_init()` が `create_screens()` を `extern "C"` で**直接呼び出し**、`tick_screen()` も同様
- これらの呼び出しは eez-flow.cpp のソースに**ハードコード**されており、関数ポインタ化されていない
- **選択肢**:
  - A) eez-flow.cpp を改造して関数ポインタ経由にする → EEZ Studio 更新時に再改造が必要
  - B) eez-flow.cpp ごと UI セクションに含める（本設計の方針） → UI セクションが ~125KB に増大
- 方式B を採用した場合、「UI だけ差し替え」の粒度は粗くなるが、ランタイム改修が不要

### 11.2 `objects` 変数の二重所有
- `objects_t objects;` は `screens.c` で**定義**（実体を持つ）が、Firmware 側の `ui_binding/` から**直接参照**される
- 分離では：
  - Firmware 側が知っている `objects` のアドレスと、UI セクション側の `objects` のアドレスが一致する必要がある
  - screens.c の定義を `extern` に変換してリンク時にアドレスを合わせる前処理が必要
  - `objects_t` の構造体定義自体がユーザーのデザイン変更で変わりうる（ウィジェット追加/削除）→ **Firmware 側と不整合が起きる危険**
- *** 検証状況***: 方針のみ策定済み。実際のリンクテスト未実施

### 11.3 フォント・画像の依存
- screens.c は `ui_font_ui_font_font1` 等のフォントシンボルを参照
- これらは Firmware 側でビルド済みのフォントデータ（`.c` → `.o`）
- ユーザーが EEZ Studio で新しいフォントを使うと**未解決シンボル**になりリンクエラー
- 画像も同様（`images[]` 配列の要素は Firmware 側の画像データを指す）
- **対策**: テンプレートプロジェクトで利用可能フォント・画像の一覧を明示

### 11.4 BSS / data セグメントの配置
- eez-flow.cpp は多数の `static` 変数を持つ（`g_objects`, `g_currentScreen`, `g_screenStack[]`, `g_fonts` 等）
- UI セクションのコード (text) は Flash に置けるが、**書き換え可能な変数は RAM に置く必要がある**
- UI セクション専用の RAM 領域確保と、リンカスクリプトでの `.data` / `.bss` 配置が必要
- **検証状況**: 未検証。eez-flow.cpp の BSS/data 使用量の正確な計測が必要

### 11.5 シンボルテーブルのバージョン固定
- `--just-symbols=Firmware.elf` で解決するアドレスは**そのビルド固有**
- Firmware をアップデートすると全関数アドレスが変わるため、シンボルテーブルと MCU 上の Firmware が**厳密に一致**しなければならない
- **対策**: `manifest.json` にバージョンを含め、Terminal が MCU のバージョンと照合

### 11.6 C++ ランタイムの問題
- eez-flow.cpp は C++ であり、`new` / `delete` や `__cxa_pure_virtual` 等の C++ ランタイムシンボルが必要になる可能性がある
- `-fno-exceptions -fno-rtti` でビルドしても一部残る場合がある
- `-nostdlib` でリンクした場合にこれらが解決できるか**未検証**
- `memcpy`, `memset` 等の C 標準関数も `--just-symbols` で Firmware 側から解決できるはずだが確認が必要

### 11.7 128KB のサイズ余裕
| 項目 | サイズ |
|---|---|
| eez-flow.o text | 101 KB |
| screens/ui/images/styles text | 24 KB |
| 合計 text | **125 KB** |
| `.rodata` (assets[] 3KB + 文字列等) | ~5-10 KB |
| **推定合計** | **130-135 KB** |

- 128KB (0x20000) だと**はみ出す可能性がある**
- STARTUP_IMAGE (0xFFE20000) との隙間を広げるか、STARTUP_IMAGE の開始アドレスを後ろにずらす必要があるかもしれない
- **検証状況**: 概算のみ。フルビルドして `.map` で正確な配置を確認すべき

### 11.8 アクション/変数名の固定
- `actions.h` / `vars.h` のシンボル名は Firmware 側で定義済み
- ユーザーが EEZ Studio でアクション/変数を削除・追加するとリンクエラー
- **対策**: テンプレートプロジェクトで必須シンボルを固定し、削除不可とする説明を添付

---

## 12. その他の課題

### 12.1 GCC-RX ライセンス
- Renesas GCC-RX は GCC ベース（GPL/LGPL）。再配布ライセンス条項の確認が必要
- 対策: ライセンスファイルを同梱

### 12.2 コンパイルエラー時の UX
- ユーザーが EEZ プロジェクトを不正に変更した場合、コンパイル/リンクエラーが発生しうる
- 対策: エラーメッセージを簡略化して Terminal に表示
- 対策: EEZ テンプレートプロジェクトを配布し、変更範囲を案内

### 12.3 HOST アプリ配布サイズ
- GCC-RX バンドルにより +20-30MB（圧縮時）
- 対策: UI カスタマイズ機能を使うときのみ GCC-RX をダウンロードする方式も検討

### 12.4 EEZ Studio バージョン互換性
- EEZ Studio のバージョンアップで出力形式が変わる可能性
- 対策: `manifest.json` に対応 EEZ Studio バージョン範囲を記載

---

## 13. 調査完了度

| 調査項目 | 状態 | 根拠 |
|---|---|---|
| assets[] と screens.c の役割分担 | ✅ 確定 | type2 diff で実証済み |
| コンパイル対象ファイル一覧 | ✅ 確定 | screens.c / ui.c / images.c / styles.c / eez-flow.cpp |
| 各ファイルのコンパイル済みサイズ | ✅ 計測済み | screens:11KB, ui:8KB, images:0.5KB, styles:4.6KB, eez-flow:101KB |
| EEZ Flow ランタイムの内部呼び出し構造 | ✅ 確定 | `eez_flow_init()` → `create_screens()` → `replacePageHook()` |
| `--just-symbols` リンク方式の可否 | ✅ 確認済み | `rx-elf-ld --help` で対応確認 |
| EEZ Studio バイナリ出力の有無 | ✅ 確定 | LVGL モードでは C ソース出力のみ |
| テーマカラーの適用範囲 | ✅ 確定 | 直接値指定の色はテーマ非経由、位置/サイズは常に screens.c |
| `objects` 変数のリンク整合 | ⚠️ 方針のみ | 実際のリンクテスト未実施 |
| UI Section からの C/C++ 標準ライブラリ呼び出し | ⚠️ 未検証 | `-nostdlib` 時に `memcpy` 等が解決されるか |
| eez-flow.cpp の BSS/data 配置 | ⚠️ 未検証 | 101KB のコードに対する BSS/data 使用量 |
| 128KB で足りるか | ⚠️ 概算のみ | text 125KB + rodata で際どい可能性 |

---

## 14. 実装計画

### Phase 1: Firmware 基盤整備
- [ ] `eez_ui_entry.h` / `eez_ui_entry.c` 作成
- [ ] リンカスクリプト変更（EEZ_UI 領域追加、EXCLUDE_FILE）
- [ ] `ui_dashboard.c` を間接呼び出しに変更
- [ ] `eez_resource_write_mode()` を 128KB 対応に拡張
- [ ] フルビルド確認（EEZ UI コードが 0xFFE00000 に配置されること）
- [ ] `rx-elf-size` / `.map` で配置検証（128KB 収まるか確認）

### Phase 2: スタンドアロン UI ビルド検証
- [ ] `eez_ui_linker.ld`（UI Section 専用リンカスクリプト）作成
- [ ] 手動で UI Section スタンドアロンビルドテスト
  - コンパイル → `--just-symbols` リンク → バイナリ抽出
- [ ] BSS/data の RAM 配置検証
- [ ] C++ ランタイムシンボルの解決確認
- [ ] eezwrite で転送テスト
- [ ] MCU リブート後に正常動作確認

### Phase 3: キットパッケージング
- [ ] `tools/generate_eez_ui_kit.ps1` 作成（シンボル抽出、ヘッダ収集、ZIP 生成）
- [ ] manifest.json 生成

### Phase 4: HOST アプリ統合
- [ ] GCC-RX 最小構成の特定・パッケージング
- [ ] `EezUiBuildService.cs` 実装
- [ ] `EezWriteService.cs` 実装（eezwrite CDC プロトコル）
- [ ] 「UI カスタマイズ」ページ UI 実装
- [ ] ビルド → 転送のワンクリックフロー実装

### Phase 5: テスト・改善
- [ ] EEZ Studio の複数デザインでビルドテスト
- [ ] エラーケースの UX 改善
- [ ] テンプレート EEZ プロジェクト整備
- [ ] ドキュメント・ユーザーガイド作成

---

## 15. 前提条件・依存関係

| 項目 | 状態 |
|---|---|
| eezwrite CDC コマンド | ✅ 実装済み（拡張必要） |
| `--just-symbols` オプション | ✅ GCC-RX 8.3.0 で対応済み |
| GCC-RX ツールチェーン | ✅ 利用可能（無償、再配布条件確認要） |
| EEZ Studio LVGL 出力 | ✅ 動作確認済み |
| FULLMONI-WIDE Terminal (WPF) | ✅ 既存アプリ |
| Bootloader CDC FW Upload | ✅ 実装済み（フル FW 用、UI カスタマイズでは不使用） |
