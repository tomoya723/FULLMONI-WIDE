/*
 * usb_cdc.h
 *
 * USB CDC Driver for FULLMONI-WIDE Firmware
 *
 * 設計方針:
 * - 通常動作時: USB最小監視（PARAM_ENTERのみ検出）
 * - パラメータモード時: フルCDC通信
 * - USB割り込み優先度: 最低(1)で性能影響を最小化
 */

#ifndef USB_CDC_H
#define USB_CDC_H

#include <stdint.h>
#include <stdbool.h>

/* USB動作モード */
typedef enum {
    USB_MODE_STANDBY,   /* 通常動作: 最小限の監視のみ */
    USB_MODE_ACTIVE     /* パラメータモード: フル通信 */
} usb_mode_t;

/* パラメータモード開始コマンド */
#define USB_PARAM_ENTER_CMD     "PARAM_ENTER"
#define USB_PARAM_ENTER_LEN     (11)

/*
 * USB CDC 初期化
 * USBモジュールを初期化し、STANDBY モードで待機開始
 */
void usb_cdc_init(void);

/*
 * USB CDC メインループ処理
 * 非ブロッキングでイベントをポーリング
 * 通常動作時はPARAM_ENTERのみ検出
 */
void usb_cdc_process(void);

/*
 * USB動作モード設定
 * USB_MODE_STANDBY: 最小監視モード（通常動作時）
 * USB_MODE_ACTIVE:  フル通信モード（パラメータモード時）
 */
void usb_cdc_set_mode(usb_mode_t mode);

/*
 * 現在のUSB動作モード取得
 */
usb_mode_t usb_cdc_get_mode(void);

/*
 * パラメータモード要求チェック
 * "PARAM_ENTER"を受信したらtrueを返す（1回のみ）
 */
bool usb_cdc_check_param_request(void);

/*
 * USB CDC 接続状態確認
 */
bool usb_cdc_is_connected(void);

/*
 * USB CDC データ送信（パラメータモード用）
 * 非ブロッキング - 送信バッファにコピー
 */
void usb_cdc_send(const uint8_t *data, uint16_t len);

/*
 * USB CDC 文字列送信
 */
void usb_cdc_print(const char *str);

/*
 * USB CDC printf風送信
 */
void usb_cdc_printf(const char *fmt, ...);

/*
 * USB CDC 受信データ取得（パラメータモード用）
 * 戻り値: 受信バイト数（0 = データなし）
 */
uint16_t usb_cdc_receive(uint8_t *buf, uint16_t max_len);

/*
 * USB CDC 1バイト受信
 * 戻り値: 受信した文字、データなしの場合 -1
 */
int usb_cdc_getchar(void);

/*
 * USB CDC クリーンシャットダウン
 * リセット前にUSBペリフェラルを正常終了させる
 */
void usb_cdc_shutdown(void);

#endif /* USB_CDC_H */
