/*
 * startup_image_write.h
 *
 * 起動画面書き込み機能
 * USB CDC経由でBMP画像をフラッシュメモリに書き込む
 *
 *  Created on: 2026/01/13
 *      Author: FULLMONI-WIDE Project
 */

#ifndef STARTUP_IMAGE_WRITE_H_
#define STARTUP_IMAGE_WRITE_H_

#include <stdint.h>
#include <stdbool.h>

/* 起動画像の仕様 */
#define STARTUP_IMAGE_WIDTH     765
#define STARTUP_IMAGE_HEIGHT    256
#define STARTUP_IMAGE_MAX_SIZE  (391696UL)  /* acmtc配列の実サイズ */

/* 通信プロトコル */
#define IMG_ACK_CHAR            0x06    /* ASCII ACK制御文字（ログの'.'と衝突防止）*/
#define IMG_CHUNK_SIZE          256     /* RX72Nのプログラミング単位 */
#define IMG_TIMEOUT_MS          5000

/* 終端マーカー (4バイト) - imgread転送完了を明示 */
#define IMG_END_MARKER_0        0xED
#define IMG_END_MARKER_1        0x0F
#define IMG_END_MARKER_2        0xAA
#define IMG_END_MARKER_3        0x55

/*
 * 起動画像書き込みモード
 * imgwriteコマンドから呼び出される
 * USB CDC経由でBMP画像を受信し、フラッシュに書き込む
 *
 * プロトコル:
 * 1. サイズ受信 (4バイト, little-endian)
 * 2. ACK送信 ('.')
 * 3. データチャンク受信 (256バイト単位)
 * 4. 各チャンクごとにACK送信
 * 5. 完了メッセージ送信 ("Done!")
 */
void startup_image_write_mode(void);

/*
 * 起動画像情報表示 (mtcinfoコマンド用)
 * acmtcシンボルのアドレス、サイズ、先頭データを表示
 */
void startup_image_show_info(void);

/*
 * 起動画像読み出しモード (imgreadコマンド用)
 * フラッシュからacmtcデータをUSB CDC経由で送信
 *
 * プロトコル:
 * 1. サイズ送信 (4バイト, little-endian)
 * 2. ACK待ち
 * 3. データチャンク送信 (256バイト単位)
 * 4. 各チャンクごとにACK待ち
 * 5. 完了
 */
void startup_image_read_mode(void);

#endif /* STARTUP_IMAGE_WRITE_H_ */
