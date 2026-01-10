/*
 * Copyright (c) 2023-2025 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/***********************************************************************************************************************
 * File Name     : boot_loader.c
 * Description   : Boot loader main function (Based on Renesas r_fwup reference)
 * Device        : RX72N (4MB Flash LINEAR MODE)
 * Modified      : 2026-01-06
 *               : - Hardware: 2-wire UART only (TX/RX, no RTS/CTS)
 *               : - Replaced RTS/CTS flow control with 256KB ring buffer
 *               : - Simplified for direct Flash write without r_fwup library
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 Includes
 *********************************************************************************************************************/
#include "boot_loader.h"
#include "r_flash_rx_if.h"
#include <string.h>

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Log Messages (from reference code) */
#define BL_MSG_SEND_VIA_UART        "Send firmware image via UART.\r\n"
#define BL_MSG_SW_RESET             "Software reset...\r\n"
#define BL_MSG_ERROR                "Error occurred. Please reset your board.\r\n"
#define BL_MSG_BOOTLOADER           "\r\n==== %s : BootLoader (2-wire UART) ====\r\n"
#define BL_MSG_OK                   "OK\r\n"
#define BL_MSG_NG                   "NG\r\n"
#define BL_MSG_EXEC_IMG             "Execute application image...\r\n"

/* FCU Command Area - RX72N uses 0x007E0000 */
#define FCU_CMD_AREA                ((volatile uint8_t *)0x007E0000UL)
#define FCU_CMD_AREA_WORD           ((volatile uint16_t *)0x007E0000UL)

/* FCU Commands */
#define FCU_CMD_PROGRAM             (0xE8)
#define FCU_CMD_BLOCK_ERASE         (0x20)
#define FCU_CMD_FINAL               (0xD0)

/* Wait counts for timeout - use r_flash_rx definition if available */
#ifndef WAIT_MAX_ROM_WRITE
#define WAIT_MAX_ROM_WRITE          (31200)    /* CF write timeout */
#endif

/* Force update flag (shared with Firmware via RAM2) */
/* Address at end of RAM2 to avoid conflict with RAM2_FUNC_AREA */
#define BL_FORCE_UPDATE_ADDR        (0x0087FFF0UL)  /* RAM2 end - 16 bytes */
#define BL_FORCE_UPDATE_MAGIC       (0xDEADBEEFUL)

/**********************************************************************************************************************
 RAM2 Flash Write Function
 The flash_write_from_ram function will be copied to RAM2 and executed from there.
 This avoids the RX72N 4MB Linear Mode restriction of not being able to execute from
 the same Flash bank being written to.
 *********************************************************************************************************************/
/* RAM2 buffer for Flash write function */
#define RAM2_FUNC_AREA              (0x00800000UL)
#define RAM2_FUNC_SIZE              (4096)   /* 4KB for function code */

/* Function type for RAM-executed flash write */
typedef flash_err_t (*ram_flash_write_t)(uint32_t src, uint32_t dest, uint32_t bytes);

/* This function will be copied to RAM2 - it's a simplified flash_write */
static flash_err_t __attribute__((noinline)) flash_write_impl(uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes)
{
    volatile uint8_t *cmd_area = FCU_CMD_AREA;
    volatile uint16_t *cmd_area_word = FCU_CMD_AREA_WORD;
    uint32_t total_count = num_bytes >> 1;  /* 2 bytes at a time */
    uint32_t write_cnt = 64;  /* 128 bytes / 2 = 64 words */
    uint16_t *src = (uint16_t *)src_addr;
    volatile int32_t wait_cnt;
    flash_err_t err = FLASH_SUCCESS;
    uint32_t psw_save;

    /* Disable interrupts - critical for PE mode (ROM unreadable during PE) */
    psw_save = __builtin_rx_mvfc(0);  /* Get PSW */
    __builtin_rx_clrpsw('I');          /* Disable interrupts */

    /* Enable Flash write (FWEPROR) */
    FLASH.FWEPROR.BYTE = 0x01;

    /* Enter Code Flash PE mode */
    FLASH.FENTRYR.WORD = 0xAA01;  /* Key + CF PE mode */
    wait_cnt = 4;
    while (FLASH.FENTRYR.WORD != 0x0001) {
        if (--wait_cnt <= 0) {
            err = FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* Wait for FRDY before issuing command */
    wait_cnt = WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            err = FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* Set destination address for programming */
    FLASH.FSADDR.LONG = dest_addr;

    /* Issue program command */
    *cmd_area = FCU_CMD_PROGRAM;
    *cmd_area = (uint8_t)write_cnt;

    /* Write data 2 bytes at a time */
    while (total_count > 0) {
        /* Write 2 bytes to FCU buffer */
        *cmd_area_word = *src++;

        /* Wait for buffer not full */
        while (FLASH.FSTATR.BIT.DBFULL == 1)
            ;

        total_count--;
    }

    /* Issue write end command */
    *cmd_area = FCU_CMD_FINAL;

    /* Wait for operation complete */
    wait_cnt = WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            /* Timeout - issue stop command */
            *cmd_area = 0xB3;
            while (FLASH.FSTATR.BIT.FRDY != 1)
                ;
            err = FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* Check for errors */
    if (FLASH.FASTAT.BIT.CMDLK != 0) {
        /* Error occurred - issue stop command */
        *cmd_area = 0xB3;
        while (FLASH.FSTATR.BIT.FRDY != 1)
            ;
        err = FLASH_ERR_CMD_LOCKED;
        goto exit_pe_mode;
    }

exit_pe_mode:
    /* Exit PE mode - return to read mode */
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000)
        ;

    /* Disable Flash write */
    FLASH.FWEPROR.BYTE = 0x00;

    /* Restore interrupts */
    __builtin_rx_mvtc(0, psw_save);

    return err;
}
/* End marker for function size calculation */
static void flash_write_impl_end(void) { }

/* Pointer to RAM-executed flash write function */
static ram_flash_write_t s_ram_flash_write = NULL;

/* Initialize RAM flash write function */
static void init_ram_flash_write(void)
{
    uint32_t func_size;
    uint8_t *src;
    uint8_t *dst;
    uint32_t i;

    /* Calculate function size */
    func_size = (uint32_t)flash_write_impl_end - (uint32_t)flash_write_impl;
    if (func_size > RAM2_FUNC_SIZE) {
        func_size = RAM2_FUNC_SIZE;
    }

    /* Copy function to RAM2 */
    src = (uint8_t *)flash_write_impl;
    dst = (uint8_t *)RAM2_FUNC_AREA;
    for (i = 0; i < func_size; i++) {
        dst[i] = src[i];
    }

    /* Set function pointer to RAM copy */
    s_ram_flash_write = (ram_flash_write_t)RAM2_FUNC_AREA;
}

/* Wrapper to call RAM-based flash write */
static flash_err_t ram_flash_write(uint32_t src, uint32_t dest, uint32_t bytes)
{
    if (s_ram_flash_write == NULL) {
        init_ram_flash_write();
    }
    return s_ram_flash_write(src, dest, bytes);
}

/**********************************************************************************************************************
 Private variables
 *********************************************************************************************************************/
/* Ring buffer for UART reception (256KB) - placed in BSS */
static uint8_t s_ring_buffer[BL_RING_BUFFER_SIZE] __attribute__((section(".bss")));
static volatile uint32_t s_ring_write_idx = 0;
static volatile uint32_t s_ring_read_idx = 0;

/* XON/XOFF flow control state */
static volatile bool s_xoff_sent = false;   /* true = XOFF sent (transmission paused) */

/* Flash write buffer (128 bytes) */
static uint8_t s_flash_buf[BL_FLASH_WRITE_SIZE];
static uint32_t s_flash_buf_cnt = 0;
static uint32_t s_total_received = 0;

/* Error flag */
static uint8_t s_err_flg = 0;

/**********************************************************************************************************************
 Private function prototypes
 *********************************************************************************************************************/
static void buf_init(void);
static uint32_t ring_buffer_count(void);
static bool ring_buffer_get(uint8_t *data);
static void send_byte(uint8_t data);
static void flow_control_check(void);
static e_bl_err_t erase_app_area(void);
static e_bl_err_t write_image(void);
static bool is_valid_image(void);
static void exec_image(void);
static void sw_reset(void);

/**********************************************************************************************************************
 * Function Name: boot_loader_rx_callback
 * Description  : UART RX interrupt callback
 *              : Called from Config_SCI9_user.c when a byte is received
 *              : Sends XOFF immediately when buffer reaches threshold (interrupt context)
 * Arguments    : rx_data - Received byte
 * Return Value : none
 *********************************************************************************************************************/
void boot_loader_rx_callback(uint8_t rx_data)
{
    uint32_t next_idx = (s_ring_write_idx + 1) % BL_RING_BUFFER_SIZE;
    uint32_t count;

    /* Store data if buffer not full */
    if (next_idx != s_ring_read_idx) {
        s_ring_buffer[s_ring_write_idx] = rx_data;
        s_ring_write_idx = next_idx;

        /* Check if XOFF needs to be sent (in interrupt context for immediate response) */
        if (!s_xoff_sent) {
            /* Calculate buffer count */
            if (s_ring_write_idx >= s_ring_read_idx) {
                count = s_ring_write_idx - s_ring_read_idx;
            } else {
                count = BL_RING_BUFFER_SIZE - s_ring_read_idx + s_ring_write_idx;
            }

            if (count >= BL_XOFF_THRESHOLD) {
                /* Send XOFF immediately - polling in interrupt is OK for single byte */
                while (0 == SCI9.SSR.BIT.TDRE) { /* Wait */ }
                SCI9.TDR = BL_XOFF_CHAR;
                s_xoff_sent = true;
            }
        }
    }
    /* If buffer full, data is lost (overflow) */
}

/**********************************************************************************************************************
 * Function Name: ring_buffer_count
 * Description  : Get number of bytes available in ring buffer
 *********************************************************************************************************************/
static uint32_t ring_buffer_count(void)
{
    uint32_t write_idx = s_ring_write_idx;  /* Snapshot */
    uint32_t read_idx = s_ring_read_idx;

    if (write_idx >= read_idx) {
        return write_idx - read_idx;
    } else {
        return BL_RING_BUFFER_SIZE - read_idx + write_idx;
    }
}

/**********************************************************************************************************************
 * Function Name: ring_buffer_get
 * Description  : Read one byte from ring buffer
 * Return Value : true if data available, false if empty
 *********************************************************************************************************************/
static bool ring_buffer_get(uint8_t *data)
{
    if (s_ring_read_idx == s_ring_write_idx) {
        return false;
    }

    *data = s_ring_buffer[s_ring_read_idx];
    s_ring_read_idx = (s_ring_read_idx + 1) % BL_RING_BUFFER_SIZE;
    return true;
}

/**********************************************************************************************************************
 * Function Name: send_byte
 * Description  : Send one byte via SCI9 (blocking)
 *********************************************************************************************************************/
static void send_byte(uint8_t data)
{
    /* Wait for transmit buffer empty */
    while (0 == SCI9.SSR.BIT.TDRE) {
        /* Wait */
    }
    SCI9.TDR = data;
}

/**********************************************************************************************************************
 * Function Name: flow_control_check
 * Description  : Check if XON needs to be sent (XOFF is sent in interrupt handler)
 *              : Called periodically during receive loop after processing data
 *********************************************************************************************************************/
static void flow_control_check(void)
{
    uint32_t count = ring_buffer_count();

    /* XOFF is sent in interrupt handler (boot_loader_rx_callback) for immediate response */

    if (s_xoff_sent && count <= BL_XON_THRESHOLD) {
        /* Buffer has space - send XON to resume transmission */
        send_byte(BL_XON_CHAR);
        s_xoff_sent = false;
    }
}

/**********************************************************************************************************************
 * Function Name: buf_init
 * Description  : Initialize buffers
 *********************************************************************************************************************/
static void buf_init(void)
{
    s_ring_write_idx = 0;
    s_ring_read_idx = 0;
    s_xoff_sent = false;
    s_flash_buf_cnt = 0;
    s_total_received = 0;
    memset(s_flash_buf, 0xFF, BL_FLASH_WRITE_SIZE);
}

/**********************************************************************************************************************
 * Function Name: erase_app_area
 * Description  : Erase application area (120 blocks x 32KB = 3.75MB)
 * Return Value : BL_SUCCESS or BL_ERR_FLASH
 *********************************************************************************************************************/
static e_bl_err_t erase_app_area(void)
{
    flash_err_t ret;
    uint32_t block_addr;

    BL_LOG("Erasing application area...\r\n");
    BL_LOG("  Address: 0x%08lX - 0x%08lX\r\n", BL_APP_START, BL_APP_END);
    BL_LOG("  Blocks: %d x 32KB\r\n", BL_APP_BLOCKS);

    /* Erase each block */
    for (uint32_t i = 0; i < BL_APP_BLOCKS; i++) {
        block_addr = BL_APP_START + (i * BL_FLASH_BLOCK_SIZE);

        ret = R_FLASH_Erase((flash_block_address_t)block_addr, 1);
        if (ret != FLASH_SUCCESS) {
            BL_LOG("\r\nERROR: Erase failed at 0x%08lX (err=%d)\r\n", block_addr, ret);
            return BL_ERR_FLASH;
        }

        /* Progress indicator */
        if ((i % 10) == 0) {
            BL_LOG(".");
        }
    }

    BL_LOG("\r\nErase complete!\r\n");
    return BL_SUCCESS;
}

/**********************************************************************************************************************
 * Function Name: write_image
 * Description  : Receive firmware via UART and write to Flash
 *              : Uses ring buffer to handle incoming data during Flash writes
 * Return Value : BL_SUCCESS or BL_ERR_FLASH
 *********************************************************************************************************************/
static e_bl_err_t write_image(void)
{
    flash_err_t flash_ret;
    uint32_t write_addr = BL_APP_START;
    uint32_t timeout_cnt = 0;
    uint8_t rx_byte;
    bool first_byte_received = false;

    BL_LOG(BL_MSG_SEND_VIA_UART);
    BL_LOG("  Target: 0x%08lX\r\n", BL_APP_START);
    BL_LOG("  Max size: %lu bytes\r\n", BL_APP_SIZE);
    BL_LOG("  Flow control: XON/XOFF enabled\r\n");
    BL_LOG("Waiting for data...\r\n");

    buf_init();

    /* Send initial XON to ensure PC is ready to send */
    send_byte(BL_XON_CHAR);

    /* Main receive loop */
    while (1) {
        /* Check flow control - send XON/XOFF based on buffer level */
        flow_control_check();

        /* Try to get data from ring buffer */
        if (ring_buffer_get(&rx_byte)) {
            /* Data received */
            if (!first_byte_received) {
                first_byte_received = true;
                BL_LOG("Transfer started...\r\n");
            }
            timeout_cnt = 0;

            /* Store in flash buffer */
            s_flash_buf[s_flash_buf_cnt++] = rx_byte;
            s_total_received++;

            /* Buffer full - write to Flash */
            if (s_flash_buf_cnt >= BL_FLASH_WRITE_SIZE) {
                /* Check address range (avoid overflow with BL_APP_END=0xFFFFFFFF) */
                if (write_addr > BL_APP_END - BL_FLASH_WRITE_SIZE + 1) {
                    BL_LOG("\r\nERROR: Image too large!\r\n");
                    return BL_ERR_FLASH;
                }

                /* Debug: Show write address on first write */
                if (s_total_received == BL_FLASH_WRITE_SIZE) {
                    BL_LOG("Writing to 0x%08lX...\r\n", write_addr);
                }

                /* Send XOFF before Flash write (interrupts will be disabled) */
                send_byte(BL_XOFF_CHAR);

                /* Wait for PC to stop sending + receive any in-flight data */
                /* At 115200bps, 1 byte = ~87us, wait 2ms = ~23 bytes margin */
                R_BSP_SoftwareDelay(2, BSP_DELAY_MILLISECS);

                /* Drain any remaining data in ring buffer won't help here,
                 * but the delay allows in-flight bytes to be received */

                /* Write to Flash using RAM-based function */
                flash_ret = ram_flash_write((uint32_t)s_flash_buf, write_addr, BL_FLASH_WRITE_SIZE);

                /* Send XON after Flash write to resume */
                send_byte(BL_XON_CHAR);

                if (flash_ret != FLASH_SUCCESS) {
                    BL_LOG("\r\nERROR: Flash write failed at 0x%08lX (err=%d)\r\n",
                           write_addr, flash_ret);
                    return BL_ERR_FLASH;
                }

                write_addr += BL_FLASH_WRITE_SIZE;
                s_flash_buf_cnt = 0;
                memset(s_flash_buf, 0xFF, BL_FLASH_WRITE_SIZE);

                /* Progress indicator every 32KB */
                if ((s_total_received % 32768) == 0) {
                    BL_LOG("\r\n%luKB", s_total_received / 1024);
                }
            }
        } else {
            /* No data - check timeout */
            if (first_byte_received) {
                R_BSP_SoftwareDelay(1, BSP_DELAY_MILLISECS);
                timeout_cnt++;

                /* 2 second timeout = transfer complete */
                if (timeout_cnt >= 2000) {
                    BL_LOG("\r\nTransfer complete!\r\n");
                    break;
                }
            } else {
                /* Still waiting for first byte - no timeout */
                R_BSP_SoftwareDelay(10, BSP_DELAY_MILLISECS);
            }
        }
    }

    /* Write remaining data (pad with 0xFF to 128-byte boundary) */
    if (s_flash_buf_cnt > 0) {
        /* Buffer already padded with 0xFF from memset */
        flash_ret = ram_flash_write((uint32_t)s_flash_buf, write_addr, BL_FLASH_WRITE_SIZE);
        if (flash_ret != FLASH_SUCCESS) {
            BL_LOG("\r\nERROR: Final write failed\r\n");
            return BL_ERR_FLASH;
        }
    }

    BL_LOG("  Received: %lu bytes\r\n", s_total_received);

    return BL_SUCCESS;
}

/**********************************************************************************************************************
 * Function Name: is_valid_image
 * Description  : Check if valid application image exists by verifying firmware header
 * Return Value : true if valid image found (firmware header magic = "RXFW")
 *********************************************************************************************************************/
static bool is_valid_image(void)
{
    /* Read firmware header magic number */
    uint32_t *fw_header = (uint32_t *)BL_FW_HEADER_ADDR;
    uint32_t magic = fw_header[0];

    /* Debug: Show firmware header info */
    BL_LOG("FW Header @ 0x%08lX:\r\n", BL_FW_HEADER_ADDR);
    BL_LOG("  Magic: 0x%08lX (expected: 0x%08lX)\r\n", magic, BL_FW_HEADER_MAGIC);

    /* Also show app start for debugging */
    uint32_t *app_start = (uint32_t *)BL_APP_START;
    BL_LOG("App data @ 0x%08lX: ", BL_APP_START);
    for (int i = 0; i < 4; i++) {
        BL_LOG("%08lX ", app_start[i]);
    }
    BL_LOG("\r\n");

    /* Validate firmware header magic number */
    if (magic != BL_FW_HEADER_MAGIC) {
        BL_LOG("Invalid firmware header - no valid application\r\n");
        return false;
    }

    BL_LOG("Valid firmware header found\r\n");
    return true;
}

/**********************************************************************************************************************
 * Function Name: exec_image
 * Description  : Jump to application with clean MCU state
 *              : Application entry point is stored in firmware header at BL_FW_HEADER_ADDR + 20
 *********************************************************************************************************************/
static void exec_image(void)
{
    /* Get entry point from firmware header */
    uint32_t *fw_header = (uint32_t *)BL_FW_HEADER_ADDR;
    uint32_t entry_point = fw_header[5];  /* entry_point is 6th field (offset 20) */

    BL_LOG(BL_MSG_EXEC_IMG);
    BL_LOG("Entry point: 0x%08lX\r\n", entry_point);
    R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);  /* Allow printf to complete */

    /* Close Flash */
    R_FLASH_Close();

    /* Disable all interrupts */
    __builtin_rx_clrpsw('I');

    /* Disable and clear SCI9 */
    SCI9.SCR.BYTE = 0x00U;
    ICU.IR[90].BIT.IR = 0;  /* Clear SCI9 RXI */
    ICU.IR[91].BIT.IR = 0;  /* Clear SCI9 TXI */
    ICU.IR[92].BIT.IR = 0;  /* Clear SCI9 TEI */
    ICU.IR[93].BIT.IR = 0;  /* Clear SCI9 ERI */
    ICU.IER[11].BIT.IEN2 = 0;  /* Disable SCI9 RXI */
    ICU.IER[11].BIT.IEN3 = 0;  /* Disable SCI9 TXI */
    ICU.IER[11].BIT.IEN4 = 0;  /* Disable SCI9 TEI */
    ICU.IER[11].BIT.IEN5 = 0;  /* Disable SCI9 ERI */

    /* Stop SCI9 clock */
    MSTP(SCI9) = 1U;

    /* Jump to application entry point from firmware header */
    void (*app_entry)(void) = (void (*)(void))entry_point;
    app_entry();

    /* Should never reach here */
    while (1);
}

/**********************************************************************************************************************
 * Function Name: sw_reset
 * Description  : Perform software reset
 *********************************************************************************************************************/
static void sw_reset(void)
{
    BL_LOG(BL_MSG_SW_RESET);
    R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);  /* Allow printf to complete */

    R_FLASH_Close();

    /* Trigger software reset */
    SYSTEM.PRCR.WORD = 0xA502;  /* Enable writing to SWRR */
    SYSTEM.SWRR = 0xA501;       /* Software reset */

    while (1);  /* Wait for reset */
}

/**********************************************************************************************************************
 * Function Name: boot_loader_main
 * Description  : Boot loader main entry point
 *              : Called from Bootloader.c after SCI9 initialization
 *              : Modified 2026-01-06: Removed 2-second 'U' key wait
 *              :   - Check force update flag from Firmware (RAM2)
 *              :   - If application is valid and no force flag, boot immediately
 *              :   - If application is erased/invalid or force flag set, enter update mode
 *********************************************************************************************************************/
void boot_loader_main(void)
{
    flash_err_t flash_ret;
    bool force_update = false;
    volatile uint32_t *force_flag = (volatile uint32_t *)BL_FORCE_UPDATE_ADDR;

    /* Enable SCI9 TX and RX for polling mode first */
    SCI9.SCR.BIT.RIE = 0U;  /* Disable RX Interrupt for polling */
    SCI9.SCR.BIT.TE = 1U;   /* Transmit Enable */
    SCI9.SCR.BIT.RE = 1U;   /* Receive Enable */

    /* Short delay for UART to stabilize */
    R_BSP_SoftwareDelay(10, BSP_DELAY_MILLISECS);

    /* Clear any pending data */
    if (SCI9.SSR.BIT.RDRF) {
        volatile uint8_t dummy = SCI9.RDR;
        (void)dummy;
    }

    /* Banner */
    BL_LOG(BL_MSG_BOOTLOADER, BL_MCU_NAME);
    BL_LOG("  Memory: BL=128KB, App=3.875MB\r\n");
    BL_LOG("  Flash write unit: 128 bytes\r\n");

    /* Check force update flag from Firmware */
    if (*force_flag == BL_FORCE_UPDATE_MAGIC) {
        BL_LOG("** FORCE UPDATE FLAG DETECTED **\r\n");
        *force_flag = 0;  /* Clear flag */
        force_update = true;
    }

    /* Enable RX interrupt for ring buffer mode */
    SCI9.SCR.BIT.RIE = 1U;

    s_err_flg = 0;

    /* Open Flash API */
    flash_ret = R_FLASH_Open();
    if (flash_ret != FLASH_SUCCESS) {
        BL_LOG("ERROR: Flash open failed (err=%d)\r\n", flash_ret);
        goto ERROR_END;
    }

    /* Check if valid application exists - boot immediately if valid and no force flag */
    BL_LOG("Checking application...");
    if (!force_update && is_valid_image()) {
        BL_LOG(BL_MSG_OK);
        exec_image();  /* Jump to application (does not return) */
    }
    BL_LOG(BL_MSG_NG);

    /* No valid application - enter update mode */
    BL_LOG("\r\n*** FIRMWARE UPDATE MODE ***\r\n");

    /* Erase application area */
    if (erase_app_area() != BL_SUCCESS) {
        goto ERROR_END;
    }

    /* Receive and write firmware image */
    if (write_image() != BL_SUCCESS) {
        goto ERROR_END;
    }

    /* Verify written image */
    BL_LOG("Verifying...");
    if (!is_valid_image()) {
        BL_LOG(BL_MSG_NG);
        goto ERROR_END;
    }
    BL_LOG(BL_MSG_OK);

    /* Success - reset to run new firmware */
    BL_LOG("\r\n*** UPDATE SUCCESS ***\r\n");
    sw_reset();

    return;

ERROR_END:
    BL_LOG(BL_MSG_ERROR);
    s_err_flg = 1;
    R_FLASH_Close();
    while (1);  /* Halt */
}

/**********************************************************************************************************************
 * Function Name: my_sw_charput_function
 * Description  : Printf character output function
 *              : Referenced by BSP_CFG_USER_CHARPUT_FUNCTION in r_bsp_config.h
 *              : Sends one character via SCI9 (polling mode)
 * Arguments    : data - Character to send
 *********************************************************************************************************************/
void my_sw_charput_function(char data)
{
    /* Wait for transmit buffer empty */
    while (0 == SCI9.SSR.BIT.TDRE) {
        /* Wait */
    }

    /* Write data to transmit register */
    SCI9.TDR = (uint8_t)data;
}
