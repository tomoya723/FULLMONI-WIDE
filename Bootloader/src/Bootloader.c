/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
*  File Name    : Bootloader.c
*  Description  : Main Program (Based on Renesas r_fwup reference)
*  Creation Date: 2026-01-04
*  Modified     : 2026-01-06 - RAM-based Flash write for RX72N 4MB Linear Mode
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "boot_loader.h"
#include <string.h>

void main(void);

void main(void)
{
    /* Initialize SCI9 (Smart Configurator generated code)
     * - R_Config_SCI9_Create(): Configure SCI9 registers and pins
     * - R_Config_SCI9_Start():  Enable SCI9 interrupts in ICU
     */
    R_Config_SCI9_Create();
    R_Config_SCI9_Start();
    
    /* Start bootloader main loop
     * Note: Flash write uses RAM-based function (copied to RAM2)
     * to avoid RX72N 4MB Linear Mode restriction
     */
    boot_loader_main();
    
    /* Should never reach here */
    while (1);
}
