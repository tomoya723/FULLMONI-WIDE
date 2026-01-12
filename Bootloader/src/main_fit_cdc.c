/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
*  File Name    : main_fit_cdc.c
*  Description  : Bootloader entry point using FIT USB CDC Driver
*                 Protocol: Size-prefix + ACK (verified working)
*  Creation Date: 2026-01-10
*  Modified     : 2026-01-11 - Reverted to size+ACK protocol
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_pcdc_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_flash_rx_if.h"

/* Bootloader Configuration - same as boot_loader.c */
#define BL_APP_START        (0xFFC20000UL)   /* Application start address */
#define BL_APP_END          (0xFFFFFFFFUL)   /* Application end address */
#define BL_FLASH_BLOCK_SIZE (0x8000UL)       /* 32KB per block */
#define BL_APP_BLOCKS       (124)            /* 124 x 32KB = 3.875MB */
#define BL_FW_HEADER_ADDR   (0xFFC20000UL)   /* Firmware header at app start */
#define BL_FW_MAGIC         (0x52584657UL)   /* "RXFW" */
#define BL_FLASH_WRITE_SIZE (128)            /* Flash write unit */

/* Force update flag (shared with Firmware via RAM2) */
#define BL_FORCE_UPDATE_ADDR    (0x0087FFF0UL)
#define BL_FORCE_UPDATE_MAGIC   (0xDEADBEEFUL)

/* FCU definitions - from boot_loader.c */
#define FCU_CMD_AREA        ((volatile uint8_t *)0x007E0000UL)
#define FCU_CMD_AREA_WORD   ((volatile uint16_t *)0x007E0000UL)
#define FCU_CMD_PROGRAM     (0xE8)
#define FCU_CMD_FINAL       (0xD0)
#define WAIT_MAX_ROM_WRITE  (31200)

/* RAM2 for Flash write function */
#define RAM2_FUNC_AREA      (0x00800000UL)
#define RAM2_FUNC_SIZE      (4096)

/* Function type for RAM-executed flash write */
typedef flash_err_t (*ram_flash_write_t)(uint32_t src, uint32_t dest, uint32_t bytes);

/* Flash write function - copied from boot_loader.c (実績あるコード) */
static flash_err_t __attribute__((noinline)) flash_write_impl(uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes)
{
    volatile uint8_t *cmd_area = FCU_CMD_AREA;
    volatile uint16_t *cmd_area_word = FCU_CMD_AREA_WORD;
    uint32_t total_count = num_bytes >> 1;
    uint32_t write_cnt = 64;  /* 128 bytes / 2 = 64 words */
    uint16_t *src = (uint16_t *)src_addr;
    volatile int32_t wait_cnt;
    flash_err_t err = FLASH_SUCCESS;
    uint32_t psw_save;

    psw_save = __builtin_rx_mvfc(0);
    __builtin_rx_clrpsw('I');

    FLASH.FWEPROR.BYTE = 0x01;

    FLASH.FENTRYR.WORD = 0xAA01;
    wait_cnt = 4;
    while (FLASH.FENTRYR.WORD != 0x0001) {
        if (--wait_cnt <= 0) { err = FLASH_ERR_TIMEOUT; goto exit_pe_mode; }
    }

    wait_cnt = WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) { err = FLASH_ERR_TIMEOUT; goto exit_pe_mode; }
    }

    FLASH.FSADDR.LONG = dest_addr;
    *cmd_area = FCU_CMD_PROGRAM;
    *cmd_area = (uint8_t)write_cnt;

    while (total_count > 0) {
        *cmd_area_word = *src++;
        while (FLASH.FSTATR.BIT.DBFULL == 1);
        total_count--;
    }

    *cmd_area = FCU_CMD_FINAL;

    wait_cnt = WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            *cmd_area = 0xB3;
            while (FLASH.FSTATR.BIT.FRDY != 1);
            err = FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    if (FLASH.FASTAT.BIT.CMDLK != 0) {
        *cmd_area = 0xB3;
        while (FLASH.FSTATR.BIT.FRDY != 1);
        err = FLASH_ERR_CMD_LOCKED;
    }

exit_pe_mode:
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000);
    FLASH.FWEPROR.BYTE = 0x00;
    __builtin_rx_mvtc(0, psw_save);

    return err;
}
static void flash_write_impl_end(void) { }

/* RAM function pointer */
static ram_flash_write_t s_ram_flash_write = NULL;

/* Initialize RAM flash write function */
static void init_ram_flash_write(void)
{
    uint32_t func_size = (uint32_t)flash_write_impl_end - (uint32_t)flash_write_impl;
    if (func_size > RAM2_FUNC_SIZE) func_size = RAM2_FUNC_SIZE;
    if (func_size < 512) func_size = 512;  /* Minimum safe size */

    uint8_t *src = (uint8_t *)flash_write_impl;
    uint8_t *dst = (uint8_t *)RAM2_FUNC_AREA;
    for (uint32_t i = 0; i < func_size; i++) dst[i] = src[i];

    s_ram_flash_write = (ram_flash_write_t)RAM2_FUNC_AREA;
}

/* RAM-based flash write wrapper */
static flash_err_t ram_flash_write(uint32_t src, uint32_t dest, uint32_t bytes)
{
    if (s_ram_flash_write == NULL) init_ram_flash_write();
    return s_ram_flash_write(src, dest, bytes);
}

/* Flash write buffer */
static uint8_t s_flash_buf[BL_FLASH_WRITE_SIZE];
static uint32_t s_flash_buf_cnt = 0;
static uint32_t s_write_addr = 0;
static uint32_t s_total_received = 0;
static bool s_update_mode = false;
static uint32_t s_last_error = 0;   /* Last flash error code */
static uint32_t s_error_addr = 0;   /* Address where error occurred */
static uint32_t s_fw_size = 0;      /* Expected firmware size */
static bool s_size_received = false; /* Size header received flag */
/* Streaming mode - no intermediate ACKs for maximum speed */

/* Stub functions for unused debug references in usb_cdc_minimal.c */
void debug_print(const char *s) { (void)s; }
void debug_hex(uint32_t v) { (void)v; }

/* External USB descriptor structure */
extern usb_descriptor_t g_usb_descriptor;

/* USB CDC buffers - 512 bytes for high-speed transfer */
#define USB_CDC_BUF_SIZE    512
static uint8_t g_usb_rx_buf[USB_CDC_BUF_SIZE];
static uint8_t g_usb_tx_buf[USB_CDC_BUF_SIZE];

/* USB control structure */
static usb_ctrl_t g_usb_ctrl;
static usb_cfg_t  g_usb_cfg;

/* CDC Line Coding - initialize with default values */
static usb_pcdc_linecoding_t g_line_coding = {
    115200,     /* dwDTERate: 115200 bps */
    0,          /* bCharFormat: 1 stop bit */
    0,          /* bParityType: None */
    8           /* bDataBits: 8 bits */
};
static bool g_tx_busy = false;       /* TX in progress flag */

static bool is_valid_image(void)
{
    uint32_t *fw_header = (uint32_t *)BL_FW_HEADER_ADDR;
    return (fw_header[0] == BL_FW_MAGIC);
}

void main(void)
{
    usb_err_t err;
    usb_status_t event;
    bool cdc_configured = false;
    bool banner_sent = false;      /* Send banner once */
    bool rx_started = false;       /* RX started after banner */
    bool counting = false;         /* Counting delay */
    uint32_t delay_counter = 0;    /* Delay after configured */
    bool force_update = false;     /* Force update flag */

    /* Enable global interrupts */
    R_BSP_InterruptsEnable();

    /* Initialize Flash API */
    if (R_FLASH_Open() != FLASH_SUCCESS) {
        while (1);  /* Halt on Flash init error */
    }

    /* Check force update flag from Firmware (RAM2) */
    volatile uint32_t *force_flag = (volatile uint32_t *)BL_FORCE_UPDATE_ADDR;
    if (*force_flag == BL_FORCE_UPDATE_MAGIC) {
        *force_flag = 0;  /* Clear flag */
        force_update = true;
    }

    /*
     * ★ 起動直後にアプリチェック（USB接続不要）
     * 有効なファームウェアがあり、かつforce_updateフラグが無い場合のみジャンプ
     */
    if (!force_update && is_valid_image()) {
        uint32_t *fw_header = (uint32_t *)BL_FW_HEADER_ADDR;
        uint32_t entry_point = fw_header[5];  /* entry_point at offset 20 */

        R_FLASH_Close();
        __builtin_rx_clrpsw('I');

        void (*app_entry)(void) = (void (*)(void))entry_point;
        app_entry();
        while (1);
    }

    /* No valid firmware OR force update - initialize USB for update */

    /* Initialize USB configuration */
    g_usb_ctrl.module = USB_IP0;        /* Use USB0 */
    g_usb_ctrl.type   = USB_PCDC;       /* Peripheral CDC */

    g_usb_cfg.usb_mode  = USB_PERI;     /* Peripheral mode */
    g_usb_cfg.usb_speed = USB_FS;       /* Full-speed */
    g_usb_cfg.p_usb_reg = &g_usb_descriptor;

    /* Open USB */
    err = R_USB_Open(&g_usb_ctrl, &g_usb_cfg);
    if (USB_SUCCESS != err) {
        while (1);  /* Halt on error */
    }

    while (1)
    {
        /* Get USB event */
        event = R_USB_GetEvent(&g_usb_ctrl);

        switch (event)
        {
            case USB_STS_CONFIGURED:
                cdc_configured = true;
                banner_sent = false;
                rx_started = false;
                counting = true;       /* Start counting from USB connection */
                delay_counter = 0;
                break;

            case USB_STS_WRITE_COMPLETE:
                g_tx_busy = false;  /* TX finished */
                /* In update mode, after ACK sent, request next data */
                if (s_update_mode) {
                    g_usb_ctrl.type = USB_PCDC;
                    g_usb_ctrl.module = USB_IP0;
                    R_USB_Read(&g_usb_ctrl, g_usb_rx_buf, USB_CDC_BUF_SIZE);
                }
                /* Start receiving after banner is sent */
                else if (banner_sent && !rx_started) {
                    rx_started = true;
                    g_usb_ctrl.type = USB_PCDC;
                    g_usb_ctrl.module = USB_IP0;
                    R_USB_Read(&g_usb_ctrl, g_usb_rx_buf, USB_CDC_BUF_SIZE);
                }
                break;

            case USB_STS_READ_COMPLETE:
                if (s_update_mode) {
                    /* Firmware receive mode - streaming (no intermediate ACKs) */
                    uint32_t rx_size = g_usb_ctrl.size;
                    bool write_error = false;
                    bool done = false;

                    /* First 4 bytes = firmware size (little-endian) */
                    if (!s_size_received) {
                        if (rx_size >= 4) {
                            s_fw_size = g_usb_rx_buf[0] | (g_usb_rx_buf[1] << 8) |
                                       (g_usb_rx_buf[2] << 16) | (g_usb_rx_buf[3] << 24);
                            s_size_received = true;
                            /* Send ACK for size, then host starts streaming */
                            g_usb_tx_buf[0] = '.';
                            g_usb_ctrl.type = USB_PCDC;
                            g_usb_ctrl.module = USB_IP0;
                            g_tx_busy = true;
                            R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, 1);
                            break;
                        }
                    }

                    for (uint32_t i = 0; i < rx_size && !write_error && !done; i++) {
                        uint8_t byte = g_usb_rx_buf[i];

                        /* Store byte in flash buffer */
                        s_flash_buf[s_flash_buf_cnt++] = byte;
                        s_total_received++;

                        /* Check if all data received */
                        if (s_total_received >= s_fw_size) {
                            /* Write remaining data (pad with 0xFF) */
                            if (s_flash_buf_cnt > 0) {
                                while (s_flash_buf_cnt < BL_FLASH_WRITE_SIZE) {
                                    s_flash_buf[s_flash_buf_cnt++] = 0xFF;
                                }
                                if (ram_flash_write((uint32_t)s_flash_buf, s_write_addr, BL_FLASH_WRITE_SIZE) != FLASH_SUCCESS) {
                                    write_error = true;
                                }
                            }

                            s_update_mode = false;
                            done = true;

                            /* Send completion message */
                            const char *done_msg = "\r\nDone! Received:        \r\n";
                            int mlen = 0;
                            while (done_msg[mlen]) { g_usb_tx_buf[mlen] = done_msg[mlen]; mlen++; }
                            /* Insert received count (7 digits) */
                            uint32_t cnt = s_total_received;
                            g_usb_tx_buf[16] = '0' + (cnt / 1000000) % 10;
                            g_usb_tx_buf[17] = '0' + (cnt / 100000) % 10;
                            g_usb_tx_buf[18] = '0' + (cnt / 10000) % 10;
                            g_usb_tx_buf[19] = '0' + (cnt / 1000) % 10;
                            g_usb_tx_buf[20] = '0' + (cnt / 100) % 10;
                            g_usb_tx_buf[21] = '0' + (cnt / 10) % 10;
                            g_usb_tx_buf[22] = '0' + cnt % 10;
                            g_usb_ctrl.type = USB_PCDC;
                            g_usb_ctrl.module = USB_IP0;
                            g_tx_busy = true;
                            R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, mlen);

                            /* Wait and verify */
                            for (volatile uint32_t d = 0; d < 1000000; d++);
                            if (is_valid_image()) {
                                /* Success - reset */
                                for (volatile uint32_t d = 0; d < 2000000; d++);
                                R_FLASH_Close();
                                SYSTEM.PRCR.WORD = 0xA502;
                                SYSTEM.SWRR = 0xA501;
                                while (1);
                            }
                            break;
                        }

                        /* Buffer full - write to Flash */
                        if (s_flash_buf_cnt >= BL_FLASH_WRITE_SIZE) {
                            if (s_write_addr > BL_APP_END - BL_FLASH_WRITE_SIZE + 1) {
                                write_error = true;
                                s_last_error = 99;  /* Address overflow */
                            } else {
                                flash_err_t ret = ram_flash_write((uint32_t)s_flash_buf, s_write_addr, BL_FLASH_WRITE_SIZE);
                                if (ret != FLASH_SUCCESS) {
                                    write_error = true;
                                    s_last_error = ret;
                                    s_error_addr = s_write_addr;
                                }
                                s_write_addr += BL_FLASH_WRITE_SIZE;
                                s_flash_buf_cnt = 0;
                                for (int j = 0; j < BL_FLASH_WRITE_SIZE; j++) s_flash_buf[j] = 0xFF;
                                /* No intermediate ACK - streaming mode for maximum speed */
                            }
                        }
                    }

                    if (write_error) {
                        s_update_mode = false;
                        /* Show error with address: "\r\nWrite ERR A=XXXXXXXX E=X\r\n> " */
                        char *p = (char *)g_usb_tx_buf;
                        *p++ = '\r'; *p++ = '\n';
                        *p++ = 'W'; *p++ = 'r'; *p++ = 'i'; *p++ = 't'; *p++ = 'e'; *p++ = ' ';
                        *p++ = 'E'; *p++ = 'R'; *p++ = 'R'; *p++ = ' ';
                        *p++ = 'A'; *p++ = '=';
                        /* Hex address */
                        for (int sh = 28; sh >= 0; sh -= 4) {
                            int nib = (s_error_addr >> sh) & 0xF;
                            *p++ = (nib < 10) ? ('0' + nib) : ('A' + nib - 10);
                        }
                        *p++ = ' '; *p++ = 'E'; *p++ = '=';
                        *p++ = '0' + s_last_error;
                        *p++ = '\r'; *p++ = '\n'; *p++ = '>'; *p++ = ' ';
                        g_usb_ctrl.type = USB_PCDC;
                        g_usb_ctrl.module = USB_IP0;
                        R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, p - (char *)g_usb_tx_buf);
                    } else if (s_update_mode) {
                        /* Streaming mode - always request next data immediately (no ACK wait) */
                        g_usb_ctrl.type = USB_PCDC;
                        g_usb_ctrl.module = USB_IP0;
                        R_USB_Read(&g_usb_ctrl, g_usb_rx_buf, USB_CDC_BUF_SIZE);
                    }
                }
                /* Command mode */
                else if (g_usb_ctrl.size > 0) {
                    char resp[64];
                    int len = 0;
                    uint8_t cmd = g_usb_rx_buf[0];
                    const char *msg;

                    if (cmd == 'S' || cmd == 's') {
                        msg = "\r\nStatus: USB OK, Flash OK\r\n> ";
                        while (msg[len]) { resp[len] = msg[len]; len++; }
                    } else if (cmd == 'U' || cmd == 'u') {
                        /* Erase all application blocks in one call for faster operation */
                        flash_err_t ret;
                        bool erase_ok;

                        ret = R_FLASH_Erase((flash_block_address_t)BL_APP_START, BL_APP_BLOCKS);
                        erase_ok = (ret == FLASH_SUCCESS);

                        if (erase_ok) {
                            /* Enter update mode */
                            s_update_mode = true;
                            s_write_addr = BL_APP_START;
                            s_flash_buf_cnt = 0;
                            s_total_received = 0;
                            s_fw_size = 0;
                            s_size_received = false;
                            for (int i = 0; i < BL_FLASH_WRITE_SIZE; i++) s_flash_buf[i] = 0xFF;

                            msg = "\r\nErase OK! Send size+firmware\r\n";
                            while (msg[len]) { resp[len] = msg[len]; len++; }
                        } else {
                            /* Show erase failure */
                            msg = "\r\nFail E=";
                            while (msg[len]) { resp[len] = msg[len]; len++; }
                            resp[len++] = '0' + ret;
                            resp[len++] = '\r';
                            resp[len++] = '\n';
                            resp[len++] = '>';
                            resp[len++] = ' ';
                        }
                    } else if (cmd == 'B' || cmd == 'b') {
                        /* Boot application - check firmware header first */
                        uint32_t *fw_header = (uint32_t *)BL_FW_HEADER_ADDR;

                        if (fw_header[0] != BL_FW_MAGIC) {
                            msg = "\r\nNo valid firmware!\r\n> ";
                            while (msg[len]) { resp[len] = msg[len]; len++; }
                        } else {
                            uint32_t entry_point = fw_header[5];  /* entry_point at offset 20 */
                            msg = "\r\nBooting...\r\n";
                            while (msg[len]) { resp[len] = msg[len]; len++; }

                            /* Send message */
                            for (int j = 0; j < len; j++) g_usb_tx_buf[j] = resp[j];
                            g_usb_ctrl.type = USB_PCDC;
                            g_usb_ctrl.module = USB_IP0;
                            R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, len);
                            for (volatile uint32_t d = 0; d < 3000000; d++);

                            /* Close Flash */
                            R_FLASH_Close();

                            /* Close USB */
                            R_USB_Close(&g_usb_ctrl);

                            /* Disable all interrupts */
                            __builtin_rx_clrpsw('I');

                            /* Jump to application */
                            void (*app_entry)(void) = (void (*)(void))entry_point;
                            app_entry();
                            while (1);
                        }
                    } else if (cmd == 'R' || cmd == 'r') {
                        msg = "\r\nResetting...\r\n";
                        while (msg[len]) { resp[len] = msg[len]; len++; }
                        /* Send message, then reset */
                        for (int j = 0; j < len; j++) g_usb_tx_buf[j] = resp[j];
                        g_usb_ctrl.type = USB_PCDC;
                        g_usb_ctrl.module = USB_IP0;
                        R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, len);
                        /* Delay for message to be sent */
                        for (volatile uint32_t d = 0; d < 3000000; d++);
                        /* Software reset via SWRR register */
                        SYSTEM.PRCR.WORD = 0xA502;  /* Unlock register protection */
                        SYSTEM.SWRR = 0xA501;       /* Execute software reset */
                        while(1);
                    } else if (cmd == '\r' || cmd == '\n') {
                        resp[len++] = '>';
                        resp[len++] = ' ';
                    } else {
                        resp[len++] = '?';
                        resp[len++] = '\r';
                        resp[len++] = '\n';
                        resp[len++] = '>';
                        resp[len++] = ' ';
                    }

                    /* Copy to TX buffer and send */
                    for (int j = 0; j < len; j++) g_usb_tx_buf[j] = resp[j];
                    g_usb_ctrl.type = USB_PCDC;
                    g_usb_ctrl.module = USB_IP0;
                    R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, len);
                }
                /* Continue receiving */
                g_usb_ctrl.type = USB_PCDC;
                g_usb_ctrl.module = USB_IP0;
                R_USB_Read(&g_usb_ctrl, g_usb_rx_buf, USB_CDC_BUF_SIZE);
                break;

            case USB_STS_REQUEST:
                /* CDC class requests (based on Renesas rm_comms_usb_pcdc sample) */
                {
                    uint16_t request_type = g_usb_ctrl.setup.type & USB_BREQUEST;

                    if (request_type == USB_PCDC_SET_LINE_CODING) {
                        /* SET_LINE_CODING - receive 7 bytes from host */
                        g_usb_ctrl.type = USB_REQUEST;
                        R_USB_Read(&g_usb_ctrl, (uint8_t*)&g_line_coding, 7);
                    }
                    else if (request_type == USB_PCDC_GET_LINE_CODING) {
                        /* GET_LINE_CODING - send 7 bytes to host */
                        g_usb_ctrl.type = USB_REQUEST;
                        R_USB_Write(&g_usb_ctrl, (uint8_t*)&g_line_coding, 7);
                    }
                    else if (request_type == USB_PCDC_SET_CONTROL_LINE_STATE) {
                        /* SET_CONTROL_LINE_STATE - no data, just ACK */
                        g_usb_ctrl.type = USB_REQUEST;
                        g_usb_ctrl.status = USB_ACK;
                        R_USB_Write(&g_usb_ctrl, (uint8_t*)USB_NULL, 0);
                    }
                    else {
                        /* Unknown request - ACK anyway */
                        g_usb_ctrl.type = USB_REQUEST;
                        g_usb_ctrl.status = USB_ACK;
                        R_USB_Write(&g_usb_ctrl, (uint8_t*)USB_NULL, 0);
                    }
                }
                break;

            case USB_STS_DETACH:
                cdc_configured = false;
                g_tx_busy = false;
                banner_sent = false;
                rx_started = false;
                counting = false;
                delay_counter = 0;
                break;

            default:
                break;
        }

        /* Send banner ONCE after delay */
        if (cdc_configured && !g_tx_busy && counting) {
            delay_counter++;
            if (delay_counter >= 3000000) {
                counting = false;
                banner_sent = true;

                const char *banner = force_update ?
                    "\r\n=== FULLMONI Bootloader (FORCE UPDATE) ===\r\nU=Update B=Boot R=Reset S=Status\r\n> " :
                    "\r\n=== FULLMONI Bootloader ===\r\nU=Update B=Boot R=Reset S=Status\r\n> ";
                int len = 0;
                while (banner[len]) {
                    g_usb_tx_buf[len] = banner[len];
                    len++;
                }

                g_usb_ctrl.type = USB_PCDC;
                g_usb_ctrl.module = USB_IP0;

                g_tx_busy = true;
                if (R_USB_Write(&g_usb_ctrl, g_usb_tx_buf, len) != USB_SUCCESS) {
                    g_tx_busy = false;
                }
            }
        }
    }
}

