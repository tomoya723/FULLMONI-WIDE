/*
 * Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/***********************************************************************************************************************
 * File Name    : boot_loader.h
 * Description  : Boot loader header (Based on Renesas r_fwup reference)
 * Device       : RX72N (4MB Flash LINEAR MODE)
 * Modified     : 2026-01-06
 *              : - Hardware: 2-wire UART only (TX/RX, no RTS/CTS)
 *              : - Buffer: 256KB ring buffer instead of RTS/CTS flow control
 *              : - Memory: Bootloader 256KB, Application 3.75MB
 ***********************************************************************************************************************/

#ifndef BOOT_LOADER_H_
#define BOOT_LOADER_H_

/**********************************************************************************************************************
 Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "platform.h"
#include "r_smc_entry.h"

/**********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* MCU Identification */
#define BL_MCU_NAME                 "RX72N"

/* UART Configuration (SCI9 @ 115200bps) */
/* Note: SCI9 configured by Smart Configurator (Config_SCI9) */
#define BL_SCI_CH                   (9)
#define BL_SCI_RATE                 (115200)

/* User Switch (optional - for forced update mode entry) */
#define BL_USER_SW_PORT             (PORT0.PIDR.BIT.B3)
#define BL_USER_SW_ON               (0)

/* Flash Configuration */
#define BL_FLASH_WRITE_SIZE         (128)            /* R_FLASH_Write unit size */
#define BL_FLASH_BLOCK_SIZE         (0x8000UL)       /* 32KB per block */

/* Ring Buffer Size (2-wire UART workaround) */
/* Needs to hold data during Flash write (~30ms at 115200bps = ~345 bytes)
 * Using 256KB for safety margin with large transfers */
#define BL_RING_BUFFER_SIZE         (256 * 1024)     /* 256KB */

/* XON/XOFF Flow Control (Software flow control for 2-wire UART) */
#define BL_XOFF_CHAR                (0x13)           /* XOFF = Ctrl+S (DC3) - Stop */
#define BL_XON_CHAR                 (0x11)           /* XON  = Ctrl+Q (DC1) - Resume */
#define BL_XOFF_THRESHOLD           (4 * 1024)       /* 4KB full → XOFF (low threshold for testing) */
#define BL_XON_THRESHOLD            (1 * 1024)       /* 1KB full → XON */

/* Memory Map Configuration (RX72N 4MB Flash, LINEAR MODE) */
/*
 * Block Map:
 *   Block 0-7:     0xFFC00000 - 0xFFC1FFFF  Bootloader  (128KB = 4 x 32KB)
 *   Block 8-127:   0xFFC20000 - 0xFFFFFFFF  Application (3.875MB = 124 x 32KB)
 *
 * Application Layout:
 *   Firmware Header: 0xFFC20000 (64 bytes at app area start)
 *   App .text start: 0xFFC20040 (after header)
 */
#define BL_BOOTLOADER_START         (0xFFC00000UL)
#define BL_BOOTLOADER_END           (0xFFC1FFFFUL)
#define BL_BOOTLOADER_SIZE          (0x20000UL)      /* 128KB */

#define BL_APP_START                (0xFFC20000UL)
#define BL_APP_END                  (0xFFDFFFFFUL)  /* startup_image領域(0xFFE00000～)を除外 */
#define BL_APP_SIZE                 (0x3E0000UL)     /* 3.875MB = 4,063,232 bytes */
#define BL_APP_BLOCKS               (124)            /* 124 x 32KB blocks */

/* Firmware Header (for application validity check) */
#define BL_FW_HEADER_ADDR           (0xFFC20000UL)   /* Firmware header at app start */
#define BL_FW_HEADER_MAGIC          (0x52584657UL)   /* "RXFW" in little-endian */

/* Log Control */
#define BL_LOG_ENABLE               (1)
#if (BL_LOG_ENABLE == 0)
#define BL_LOG(...)
#else
#define BL_LOG                      printf
#endif

/**********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/* Boot Loader Error Codes */
typedef enum e_bl_err
{
    BL_SUCCESS = 0,
    BL_ERR_FAILURE,
    BL_ERR_FLASH,
    BL_ERR_VERIFY,
    BL_ERR_TIMEOUT
} e_bl_err_t;

/**********************************************************************************************************************
 Public Functions
 **********************************************************************************************************************/
/* Main boot loader entry point */
void boot_loader_main(void);

/* UART RX callback (called from Config_SCI9_user.c r_Config_SCI9_callback_receiveend) */
void boot_loader_rx_callback(uint8_t rx_data);

/* Printf character output hook (referenced by BSP_CFG_USER_CHARPUT_FUNCTION) */
void my_sw_charput_function(char data);

#endif /* BOOT_LOADER_H_ */
