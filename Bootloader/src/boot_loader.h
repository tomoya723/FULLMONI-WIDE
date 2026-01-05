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

/* Memory Map Configuration (RX72N 4MB Flash, LINEAR MODE) */
/*
 * Block Map:
 *   Block 0-119:   0xFFC40000 - 0xFFFFFFFF  Application (3.75MB = 120 x 32KB)
 *   Block 120-127: 0xFFC00000 - 0xFFC3FFFF  Bootloader  (256KB = 8 x 32KB)
 */
#define BL_BOOTLOADER_START         (0xFFC00000UL)
#define BL_BOOTLOADER_END           (0xFFC3FFFFUL)
#define BL_BOOTLOADER_SIZE          (0x40000UL)      /* 256KB */

#define BL_APP_START                (0xFFC40000UL)
#define BL_APP_END                  (0xFFFFFFFFUL)
#define BL_APP_SIZE                 (0x3C0000UL)     /* 3.75MB = 3,932,160 bytes */
#define BL_APP_BLOCKS               (120)            /* 120 x 32KB blocks */

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
