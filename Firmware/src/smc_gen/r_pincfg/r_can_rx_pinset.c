/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name    : r_can_rx_pinset.c
* Version      : 1.0.2
* Device(s)    : R5F572NNDxFP
* Tool-Chain   : RXC toolchain
* Description  : Setting of port and mpc registers
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_can_rx_pinset.h"
#include "platform.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_CAN_PinSet_CAN0
* Description  : This function initializes pins for r_can_rx module
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_CAN_PinSet_CAN0()
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set CRX0 pin */
    PORT3.PMR.BIT.B3 = 0U;
    PORT3.PDR.BIT.B3 = 0U;
    MPC.P33PFS.BYTE = 0x10U;
    PORT3.PMR.BIT.B3 = 1U;
    PORT3.PDR.BIT.B3 = 0U;

    /* Set CTX0 pin */
    PORT3.PMR.BIT.B2 = 0U;
    PORT3.PDR.BIT.B2 = 0U;
    MPC.P32PFS.BYTE = 0x10U;
    PORT3.PMR.BIT.B2 = 1U;
    PORT3.PDR.BIT.B2 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

