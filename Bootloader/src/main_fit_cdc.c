/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
*  File Name    : main_fit_cdc.c
*  Description  : Bootloader entry point using FIT USB CDC Driver
*  Creation Date: 2026-01-10
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "r_usb_basic_if.h"
#include "r_usb_pcdc_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"

/* Stub functions for unused debug references in usb_cdc_minimal.c */
void debug_print(const char *s) { (void)s; }
void debug_hex(uint32_t v) { (void)v; }

/* External USB descriptor structure */
extern usb_descriptor_t g_usb_descriptor;

/* USB CDC buffers */
#define USB_CDC_BUF_SIZE    64
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

void main(void)
{
    usb_err_t err;
    usb_status_t event;
    uint32_t msg_counter = 0;
    uint32_t loop_counter = 0;
    bool cdc_configured = false;
    uint32_t event_count = 0;       /* Count total events received */
    uint32_t loop_total = 0;        /* Count total main loops */

    /* Enable global interrupts */
    R_BSP_InterruptsEnable();

    /* Note: VBUS detection pin (P16) is not connected in peripheral mode (R26 N.C.) */
    /* The FIT driver has been modified to bypass VBUS detection */

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

    /* Note: attach_process is now called in R_USB_Open (modified FIT driver) */

    while (1)
    {
        loop_total++;

        /* Get USB event */
        event = R_USB_GetEvent(&g_usb_ctrl);

        /* Count non-NONE events */
        if (event != USB_STS_NONE) {
            event_count++;
        }

        switch (event)
        {
            case USB_STS_CONFIGURED:
                cdc_configured = true;
                break;

            case USB_STS_WRITE_COMPLETE:
                g_tx_busy = false;  /* TX finished */
                break;

            case USB_STS_READ_COMPLETE:
                /* Echo received data back */
                if (g_usb_ctrl.size > 0) {
                    g_usb_ctrl.type = USB_PCDC;
                    g_usb_ctrl.module = USB_IP0;
                    R_USB_Write(&g_usb_ctrl, g_usb_rx_buf, g_usb_ctrl.size);
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
                break;

            default:
                break;
        }

        /* Send periodic message when configured AND not busy */
        if (cdc_configured && !g_tx_busy) {
            loop_counter++;
            if (loop_counter >= 3000000) {
                loop_counter = 0;
                msg_counter++;

                int len = 0;

                /* Format: "M:xxx E:xxx L:xxx\r\n" */
                /* M = message counter, E = event count, L = loop total (in millions) */
                const char *prefix = "M:";
                while (prefix[len]) {
                    g_usb_tx_buf[len] = prefix[len];
                    len++;
                }

                /* Add msg_counter as decimal */
                char num[12];
                uint32_t n = msg_counter;
                int i = 0;
                do {
                    num[i++] = '0' + (n % 10);
                    n /= 10;
                } while (n > 0);
                while (i > 0) {
                    g_usb_tx_buf[len++] = num[--i];
                }

                g_usb_tx_buf[len++] = ' ';
                g_usb_tx_buf[len++] = 'E';
                g_usb_tx_buf[len++] = ':';

                /* Add event_count */
                n = event_count;
                i = 0;
                do {
                    num[i++] = '0' + (n % 10);
                    n /= 10;
                } while (n > 0);
                while (i > 0) {
                    g_usb_tx_buf[len++] = num[--i];
                }

                g_usb_tx_buf[len++] = ' ';
                g_usb_tx_buf[len++] = 'L';
                g_usb_tx_buf[len++] = ':';

                /* Add loop_total in millions */
                n = loop_total / 1000000;
                i = 0;
                do {
                    num[i++] = '0' + (n % 10);
                    n /= 10;
                } while (n > 0);
                while (i > 0) {
                    g_usb_tx_buf[len++] = num[--i];
                }
                g_usb_tx_buf[len++] = 'M';  /* M for millions */

                g_usb_tx_buf[len++] = '\r';
                g_usb_tx_buf[len++] = '\n';

                /* Set type before R_USB_Write */
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
