/*
 * usb_cdc.c
 *
 * USB CDC Driver for FULLMONI-WIDE Firmware
 *
 * 設計方針:
 * - 通常動作時: USB最小監視（PARAM_ENTERのみ検出）
 * - パラメータモード時: フルCDC通信
 * - USB割り込み優先度: 最低(1)で性能影響を最小化
 */

#include "usb_cdc.h"
#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_pcdc_if.h"
#include "r_usb_basic_pinset.h"  /* USB pin setup */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* USB CDC バッファサイズ */
#define USB_CDC_RX_BUF_SIZE     64
#define USB_CDC_TX_BUF_SIZE     256
#define USB_CDC_RING_SIZE       512

/* USB制御構造体 */
static usb_ctrl_t s_usb_ctrl;
static usb_cfg_t  s_usb_cfg;

/* USB記述子（外部定義） */
extern usb_descriptor_t g_usb_descriptor;

/* CDC Line Coding */
static usb_pcdc_linecoding_t s_line_coding = {
    115200,     /* dwDTERate: 115200 bps */
    0,          /* bCharFormat: 1 stop bit */
    0,          /* bParityType: None */
    8           /* bDataBits: 8 bits */
};

/* USBバッファ */
static uint8_t s_usb_rx_buf[USB_CDC_RX_BUF_SIZE];
static uint8_t s_usb_tx_buf[USB_CDC_TX_BUF_SIZE];

/* 受信リングバッファ（パラメータモード用） */
static uint8_t s_rx_ring[USB_CDC_RING_SIZE];
static volatile uint16_t s_rx_ring_head = 0;
static volatile uint16_t s_rx_ring_tail = 0;

/* 状態フラグ */
static volatile bool s_cdc_configured = false;
static volatile bool s_tx_busy = false;
static volatile bool s_param_request = false;
static volatile usb_mode_t s_usb_mode = USB_MODE_STANDBY;

/* 内部関数 */
static void usb_cdc_handle_rx(const uint8_t *data, uint16_t len);
static void rx_ring_push(uint8_t byte);
static int rx_ring_pop(void);
static uint16_t rx_ring_available(void);

/*
 * USB CDC 初期化
 */
void usb_cdc_init(void)
{
    usb_err_t err;

    /* 状態初期化 */
    s_cdc_configured = false;
    s_tx_busy = false;
    s_param_request = false;
    s_usb_mode = USB_MODE_STANDBY;
    s_rx_ring_head = 0;
    s_rx_ring_tail = 0;

    /* USBピン設定（VBUS検出ピン） */
    R_USB_PinSet_USB0_PERI();

    /* USB構成 */
    s_usb_ctrl.module = USB_IP0;
    s_usb_ctrl.type   = USB_PCDC;

    s_usb_cfg.usb_mode  = USB_PERI;
    s_usb_cfg.usb_speed = USB_FS;
    s_usb_cfg.p_usb_reg = &g_usb_descriptor;

    /* USB開始 */
    err = R_USB_Open(&s_usb_ctrl, &s_usb_cfg);
    if (USB_SUCCESS != err) {
        /* 初期化失敗 - 継続するがUSBは使用不可 */
        return;
    }
}

/*
 * USB CDC メインループ処理
 */
void usb_cdc_process(void)
{
    usb_status_t event;
    static bool banner_sent = false;
    static bool rx_started = false;
    static uint32_t init_delay = 0;

    /* 初期化後の遅延処理 */
    if (!s_cdc_configured && init_delay < 100000) {
        init_delay++;
    }

    /* イベント取得 */
    event = R_USB_GetEvent(&s_usb_ctrl);

    switch (event)
    {
        case USB_STS_CONFIGURED:
            s_cdc_configured = true;
            banner_sent = false;
            rx_started = false;
            /* ウェルカムメッセージ送信 */
            {
                const char *msg = "\r\n[FULLMONI-WIDE] USB CDC Ready. Send 'PARAM_ENTER' to enter parameter mode.\r\n";
                s_usb_ctrl.type = USB_PCDC;
                s_usb_ctrl.module = USB_IP0;
                s_tx_busy = true;
                R_USB_Write(&s_usb_ctrl, (uint8_t*)msg, strlen(msg));
            }
            break;

        case USB_STS_WRITE_COMPLETE:
            s_tx_busy = false;
            /* バナー送信後に受信開始 */
            if (!rx_started && s_cdc_configured) {
                rx_started = true;
                banner_sent = true;
                s_usb_ctrl.type = USB_PCDC;
                s_usb_ctrl.module = USB_IP0;
                R_USB_Read(&s_usb_ctrl, s_usb_rx_buf, USB_CDC_RX_BUF_SIZE);
            }
            break;

        case USB_STS_READ_COMPLETE:
            if (s_usb_ctrl.size > 0) {
                usb_cdc_handle_rx(s_usb_rx_buf, s_usb_ctrl.size);
            }
            /* 次の受信を開始 */
            s_usb_ctrl.type = USB_PCDC;
            s_usb_ctrl.module = USB_IP0;
            R_USB_Read(&s_usb_ctrl, s_usb_rx_buf, USB_CDC_RX_BUF_SIZE);
            break;

        case USB_STS_REQUEST:
            {
                uint16_t request_type = s_usb_ctrl.setup.type & USB_BREQUEST;

                if (request_type == USB_PCDC_SET_LINE_CODING) {
                    s_usb_ctrl.type = USB_REQUEST;
                    R_USB_Read(&s_usb_ctrl, (uint8_t*)&s_line_coding, 7);
                }
                else if (request_type == USB_PCDC_GET_LINE_CODING) {
                    s_usb_ctrl.type = USB_REQUEST;
                    R_USB_Write(&s_usb_ctrl, (uint8_t*)&s_line_coding, 7);
                }
                else if (request_type == USB_PCDC_SET_CONTROL_LINE_STATE) {
                    s_usb_ctrl.type = USB_REQUEST;
                    s_usb_ctrl.status = USB_ACK;
                    R_USB_Write(&s_usb_ctrl, (uint8_t*)USB_NULL, 0);
                }
                else {
                    s_usb_ctrl.type = USB_REQUEST;
                    s_usb_ctrl.status = USB_ACK;
                    R_USB_Write(&s_usb_ctrl, (uint8_t*)USB_NULL, 0);
                }
            }
            break;

        case USB_STS_DETACH:
            s_cdc_configured = false;
            s_tx_busy = false;
            break;

        default:
            break;
    }
}

/*
 * 受信データ処理
 */
static void usb_cdc_handle_rx(const uint8_t *data, uint16_t len)
{
    if (s_usb_mode == USB_MODE_STANDBY) {
        /*
         * STANDBY モード: 任意のデータ受信でパラメータモード要求
         *
         * 互換性のため、"PARAM_ENTER" または 任意の入力（改行など）で
         * パラメータモードへの遷移をトリガー
         *
         * アプリ（FULLMONI-WIDE-Terminal）は空コマンド "" を送信して
         * パラメータモードに入るため、この仕様が必要
         */
        if (len > 0) {
            /* 何らかのデータを受信したらパラメータモード要求 */
            s_param_request = true;
        }
        /* 受信データ自体は無視（パラメータモード移行後に処理開始） */
        return;
    }

    /* ACTIVE モード: リングバッファに追加 */
    for (uint16_t i = 0; i < len; i++) {
        rx_ring_push(data[i]);
    }
}

/*
 * USB動作モード設定
 */
void usb_cdc_set_mode(usb_mode_t mode)
{
    s_usb_mode = mode;

    if (mode == USB_MODE_STANDBY) {
        /* STANDBYモードへ移行 - リングバッファクリア */
        s_rx_ring_head = 0;
        s_rx_ring_tail = 0;
    }
}

/*
 * 現在のUSB動作モード取得
 */
usb_mode_t usb_cdc_get_mode(void)
{
    return s_usb_mode;
}

/*
 * パラメータモード要求チェック
 */
bool usb_cdc_check_param_request(void)
{
    if (s_param_request) {
        s_param_request = false;
        return true;
    }
    return false;
}

/*
 * USB CDC 接続状態確認
 */
bool usb_cdc_is_connected(void)
{
    return s_cdc_configured;
}

/*
 * USB CDC データ送信
 */
void usb_cdc_send(const uint8_t *data, uint16_t len)
{
    if (!s_cdc_configured || s_usb_mode != USB_MODE_ACTIVE) {
        return;
    }

    /* 前回の送信完了を待つ（タイムアウト付き） */
    uint32_t timeout = 100000;
    while (s_tx_busy && --timeout > 0) {
        usb_cdc_process();
    }

    if (len > USB_CDC_TX_BUF_SIZE) {
        len = USB_CDC_TX_BUF_SIZE;
    }

    memcpy(s_usb_tx_buf, data, len);

    s_usb_ctrl.type = USB_PCDC;
    s_usb_ctrl.module = USB_IP0;
    s_tx_busy = true;

    if (R_USB_Write(&s_usb_ctrl, s_usb_tx_buf, len) != USB_SUCCESS) {
        s_tx_busy = false;
    }
}

/*
 * USB CDC 文字列送信
 */
void usb_cdc_print(const char *str)
{
    usb_cdc_send((const uint8_t *)str, strlen(str));
}

/*
 * USB CDC printf風送信
 */
void usb_cdc_printf(const char *fmt, ...)
{
    static char buf[256];
    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (len > 0) {
        usb_cdc_send((const uint8_t *)buf, len);
    }
}

/*
 * USB CDC 受信データ取得
 */
uint16_t usb_cdc_receive(uint8_t *buf, uint16_t max_len)
{
    uint16_t count = 0;

    while (count < max_len) {
        int c = rx_ring_pop();
        if (c < 0) break;
        buf[count++] = (uint8_t)c;
    }

    return count;
}

/*
 * USB CDC 1バイト受信
 */
int usb_cdc_getchar(void)
{
    return rx_ring_pop();
}

/* ============================================================
 * リングバッファ操作
 * ============================================================ */

static void rx_ring_push(uint8_t byte)
{
    uint16_t next = (s_rx_ring_head + 1) % USB_CDC_RING_SIZE;
    if (next != s_rx_ring_tail) {
        s_rx_ring[s_rx_ring_head] = byte;
        s_rx_ring_head = next;
    }
    /* バッファフルの場合は破棄 */
}

static int rx_ring_pop(void)
{
    if (s_rx_ring_head == s_rx_ring_tail) {
        return -1;  /* 空 */
    }
    uint8_t byte = s_rx_ring[s_rx_ring_tail];
    s_rx_ring_tail = (s_rx_ring_tail + 1) % USB_CDC_RING_SIZE;
    return byte;
}

static uint16_t rx_ring_available(void)
{
    if (s_rx_ring_head >= s_rx_ring_tail) {
        return s_rx_ring_head - s_rx_ring_tail;
    } else {
        return USB_CDC_RING_SIZE - s_rx_ring_tail + s_rx_ring_head;
    }
}

/* ============================================================
 * USB CDC クリーンシャットダウン
 * ============================================================ */
void usb_cdc_shutdown(void)
{
    /* USB割り込みを無効化のみ（モジュール停止はしない） */
    /* リセット後にBootloaderがUSBを再初期化するため */
    IEN(USB0, D0FIFO0) = 0;
    IEN(USB0, D1FIFO0) = 0;
    IEN(USB0, USBI0) = 0;

    /* 短い待機でバッファフラッシュ */
    R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);
}
