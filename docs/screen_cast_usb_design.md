# USB経由スマホ画面ミラーリング（連続キャスト）設計

## 概要

スマートフォンやPCの画面（HDMIキャプチャ画像を含む）を、**USB CDC経由で受信し既存の
800×256液晶に連続表示**する機能の設計。本機のUSBはフルスピード(12Mbps)・CDCデバイス
構成であり、ライブ動画のミラーリングは帯域的に不可能であるため、本設計は
**低フレームレートの「連続スナップショット（擬似動画）」**を対象とする。

フラッシュ書き込みを伴う起動画像機能（[`startup_image_write.c`](../Firmware/src/startup_image_write.c)）
とは異なり、**受信ピクセルをGR2フレームバッファへ直接書き込む**ため、フラッシュ操作は不要。

> ⚠️ 本ドキュメントは設計フェーズの成果物。コード実装前に「6. 実装前の確認事項」の
> フレームバッファ構成を実機で検証すること。

---

## 1. 前提と制約

### 1.1 ハードウェア構成（確認済み）

| 項目 | 現状 | 参照 |
|---|---|---|
| USBモジュール | USB0 = **フルスピード(12Mbps)**、`USB_CFG_PERI`（デバイス動作） | [`r_usb_basic_config.h`](../Firmware/src/smc_gen/r_config/r_usb_basic_config.h) |
| USB実効スループット | 約 **268 KB/s**（FW更新実績値） | README |
| 液晶 | 800×256、RGB565（16bit） | [`r_lcd_timing.h`](../Firmware/src/r_lcd_timing.h) |
| GR2フレームバッファ | `0x00800000`（emWinスキャンアウト先） | [`qe_emwin_config.h`](../Firmware/src/qe_emwin_config.h) `EMWIN_GUI_FRAME_BUFFER1` |
| SRAM | 1MB（うちGR1 ARGB8888で約800KB専有） | — |
| CPU | RX72N 240MHz（HWデコーダ無し） | — |

### 1.2 帯域制約（設計の根拠）

- 1フレーム = 800 × 256 × 2 (RGB565) = **400 KB**
- 400 KB ÷ 268 KB/s ≒ **1.5 秒/フレーム ≈ 0.6〜1.5 fps**

この帯域は本機がUSBフルスピードCDCデバイスである以上、ファームウェアだけでは
超えられない。ライブ動画・スクロール表示は本設計の対象外（別途HDMIスケーラ＋
映像muxのハード追加が必要 = 別案A）。

### 1.3 適する用途 / 適さない用途

- **適する**: 地図の静止表示、通知・メッセージ、スロー更新の情報画面、静止画スライド
- **適さない**: 動画再生、ゲーム、スクロールするUI、リアルタイムなミラーリング

---

## 2. 全体アーキテクチャ

```
[スマホ/PC] --USB CDC(FS)--> [RX72N] --直接書込--> [GR2 FB 0x00800000] --GLCDC--> [液晶]
  画面キャプチャ                  screen_cast.c        RGB565 800x256          常時スキャン
  RGB565変換
  400KB/frameを連続送信
```

現状、emWin/AppWizardがGR2フレームバッファ(`0x00800000`)へ描画し、GLCDCが常時
スキャンアウトしている。キャスト中は**emWinの再描画を止め、受信ピクセルを同バッファへ
直書き**するだけで表示できる。GLCDCの再設定は原則不要（例外は6章参照）。

### 2.1 既存資産の再利用

| 再利用する既存機能 | 用途 |
|---|---|
| [`usb_cdc.c`](../Firmware/src/usb_cdc.c) の受信/リングバッファ/ACK | フレームデータ受信・フロー制御 |
| `receive_bytes()`（[`startup_image_write.c`](../Firmware/src/startup_image_write.c)相当） | タイムアウト付きチャンク受信 |
| `send_ack()` / `IMG_CHUNK_SIZE` / `IMG_ACK_CHAR` | チャンク単位フロー制御 |
| `param_console` のコマンドディスパッチ | `imgcast` コマンド起動 |
| `usb_cdc_set_mode(USB_MODE_ACTIVE)` | フルCDC通信モード遷移 |

`startup_image_write.c` のフラッシュ操作（RAM実行コード・FCUコマンド）は**流用しない**。

---

## 3. 通信プロトコル（ホスト → FW）

`imgwrite` と同じ ACK ベースのフロー制御を踏襲する。

### 3.1 シーケンス

```
Host                                   FW (imgcast mode)
 |-- "imgcast\r\n" -------------------->|  コマンド起動、USB_MODE_ACTIVE
 |                                       |  usb_cdc_flush_rx()
 |                                       |
 |== フレームループ ====================|
 |-- Frame Header (4B) ---------------->|  "FRAM" マジック、または "STOP" で終了
 |-- pixel chunk (IMG_CHUNK_SIZE) ----->|  GR2 FB へ直書き
 |<------------------------- ACK (1B) --|  send_ack()
 |         ... 400KB 分繰り返し ...      |
 |                                       |  GLCDCが次スキャンで画面反映
 |== 次フレームへ ======================|
 |                                       |
 |-- "STOP" (4B) --------------------->|  ループ終了 → 通常画面へ復帰
```

### 3.2 フレームヘッダ（4バイト）

| オフセット | 内容 |
|---|---|
| 0..3 | マジック `"FRAM"`（0x46 0x52 0x41 0x4D）= フレーム開始 / `"STOP"` = キャスト終了 |

フレームサイズは固定（800×256×2 = 400KB）とし、可変長は初版では扱わない。
将来的に部分更新（矩形領域）を足す場合はヘッダを拡張する（7章）。

### 3.3 ピクセルフォーマット

- **RGB565 リトルエンディアン**、行順（top-to-bottom）、800px/行、256行。
- 液晶と同一フォーマットのため**FW側の色変換は不要**（ホスト側で変換する）。

---

## 4. ファームウェア実装方針

### 4.1 新規ファイル `Firmware/src/screen_cast.c`

```c
#define CAST_FB_ADDR     (0x00800000UL)      /* GR2 scanout (RGB565) */
#define CAST_FRAME_BYTES (800u * 256u * 2u)  /* 400 KB / frame       */

void startup_image_cast_mode(void)
{
    usb_cdc_set_mode(USB_MODE_ACTIVE);
    usb_cdc_flush_rx();
    /* imgwriteと異なりバックライトは点灯のまま（LCD_FadeOUTしない） */

    for (;;) {
        uint8_t hdr[4];
        if (!receive_bytes(hdr, 4, IMG_TIMEOUT_MS)) break;   /* タイムアウトで終了 */
        if (memcmp(hdr, "STOP", 4) == 0) break;              /* 終了センチネル   */
        if (memcmp(hdr, "FRAM", 4) != 0) continue;           /* 同期外れは読み飛ばし */

        uint32_t off = 0;
        while (off < CAST_FRAME_BYTES) {
            uint32_t n = (CAST_FRAME_BYTES - off < IMG_CHUNK_SIZE)
                       ? (CAST_FRAME_BYTES - off) : IMG_CHUNK_SIZE;
            /* GR2バッファへ直接受信（tearingは低fpsで許容） */
            if (!receive_bytes((uint8_t *)(CAST_FB_ADDR + off), n, IMG_TIMEOUT_MS))
                return;
            off += n;
            send_ack();      /* チャンク単位フロー制御 */
        }
        /* 次フレームへ。GLCDCが自動でスキャンアウト */
    }
    /* 復帰時、param_console_process()に戻ると APPW_Exec() が通常画面を再描画 */
}
```

### 4.2 `param_console.c` へのコマンド追加

`imgread`/`imgwrite` の分岐（611行付近）に追加：

```c
} else if (strcmp(cmd, "imgcast") == 0) {
    extern void startup_image_cast_mode(void);
    startup_image_cast_mode();
}
```

`cmd_help()` にも 1 行追記する。

### 4.3 復帰時の再描画

キャスト終了後は `param_console_process()` → メインループの `APPW_Exec()`
（[`main.c:277`](../Firmware/src/main.c)）が通常画面を再描画するため、
GR2バッファ上の画像は自動的に上書きされる。特別な後処理は不要。

---

## 5. ホストアプリ側方針（別スコープ）

初版FWでは扱わないが、送信側は以下を実装する:

1. `imgcast` コマンド送信（パラメータモード進入後）
2. 画面キャプチャ（例: Windows=`Graphics.CopyFromScreen`、Android=`MediaProjection`）
3. **800×256 へダウンスケール + RGB565 変換**（アスペクト比は必要ならレターボックス）
4. `"FRAM"` + 400KB を `IMG_CHUNK_SIZE` 単位で送信、各チャンクの ACK 待ち
5. 停止操作で `"STOP"` 送信

`HostApp` の WPF / Android いずれも、既存の imgwrite 送信ロジックを流用可能。

---

## 6. 実装前の確認事項（重要）

### 6.1 フレームバッファのダブルバッファ有無 ★要実機確認

`qe_emwin_config.h` に `EMWIN_GUI_FRAME_BUFFER2 = 0x00000040` が定義されている。
emWin が `GUI_MULTIBUF` でダブルバッファ切替をしている場合、GLCDCが特定タイミングで
`0x00000040` 側をスキャンしている可能性がある。

- **シングルバッファ（`0x00800000`固定スキャン）の場合** → 4章の直書きでそのまま動作。
- **ダブルバッファの場合** → キャスト開始時に `R_GLCDC_LayerChange()` で
  GR2 スキャンポインタを `CAST_FB_ADDR` に固定し、キャスト中はバッファ切替を止める。

QE生成の `LCDConf.c` は本リポジトリに含まれないため、実機の GLCDC 設定
（GR2 の `p_base` 更新箇所）を確認して確定する。

### 6.2 メモリ競合

`0x00800000` は `startup_image_write.c` が**フラッシュ操作中のみ**RAM実行領域として
流用している（`RAM2_FUNC_AREA`）。キャストとフラッシュ書き込みは排他動作
（どちらもパラメータモードのブロッキング関数）のため競合しない。

### 6.3 ウォッチドッグ / 割り込み

キャストループは長時間ブロックするため、`imgwrite` と同様に WDT リフレッシュや
CAN 受信の扱いを既存パラメータモードに合わせる（`param_console` 内は通常
GUI/CAN を止めている点を踏襲）。

---

## 7. 将来拡張（対象外）

- **矩形部分更新**: 変化領域のみ送信し実効fpsを向上（ヘッダに x,y,w,h を追加）。
- **簡易圧縮**: RLE 等でベタ塗り領域を圧縮し帯域を節約。
- **ハイスピードUSB化 / HDMIスケーラ+mux（別案A）**: ライブ動画が必要な場合のハード改修。

---

## 8. 実装フェーズ

### Phase 1: フレームバッファ構成の実機確認（6.1）
### Phase 2: FW実装（`screen_cast.c` + `imgcast` コマンド）
### Phase 3: ホストアプリ送信機能（WPF / Android）
### Phase 4: テスト・検証（同期外れ・タイムアウト・復帰動作・実効fps測定）
