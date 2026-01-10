/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name    : r_usb_basic_pinset.c
* Version      : 1.0.2
* Device(s)    : R5F572NNDxFP
* Tool-Chain   : RXC toolchain
* Description  : Setting of port and mpc registers
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_usb_basic_pinset.h"
#include "platform.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_USB_PinSet_USB0_PERI
* Description  : This function initializes pins for r_usb_basic module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_USB_PinSet_USB0_PERI()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set USB0_VBUS pin */
    MPC.P16PFS.BYTE = 0x11U;
    PORT1.PMR.BIT.B6 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

