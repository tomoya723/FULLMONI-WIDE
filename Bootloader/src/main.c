/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
*  File Name    : main.c
*  Description  : Bootloader entry point
*  Creation Date: 2026-01-04
***********************************************************************************************************************/
#include "r_smc_entry.h"
#include "boot_loader.h"

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
