/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU8.c
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_MTU8.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_MTU8.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU8_Create
* Description  : This function initializes the MTU8 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU8_Create(void)
{
    /* Release MTU channel 8 from stop state */
    MSTP(MTU3) = 0U;

    /* Stop MTU channel 8 counter */
    MTU.TSTRA.BIT.CST8 = 0U;

    /* Set TGIA8 interrupt priority level */
    ICU.SLIAR246.BYTE = 0x29U;
    IPR(PERIA, INTA246) = _0F_MTU_PRIORITY_LEVEL15;

    /* MTU channel 8 is used as normal mode */
    MTU8.TCR.BYTE = _00_MTU_CKEG_RISE | _20_MTU_CKCL_A;
    MTU8.TCR2.BYTE = _05_MTU_PCLK_1024;
    MTU8.TIER.BYTE = _01_MTU_TGIEA_ENABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEV_DISABLE;
    MTU8.NFCR8.BYTE = _01_MTU_NFAEN_ENABLE | _20_MTU_NFCS_PCLK_32;
    MTU8.TIORH.BYTE = _08_MTU_IOA_IR | _00_MTU_IOB_DISABLE;
    MTU8.TIORL.BYTE = _00_MTU_IOC_DISABLE | _00_MTU_IOD_DISABLE;
    MTU8.TGRB = _0000000B_TGRB8_VALUE;
    MTU8.TGRC = _0000000B_TGRC8_VALUE;
    MTU8.TGRD = _0000000B_TGRD8_VALUE;

    /* Set MTIOC8A pin */
    MPC.PD6PFS.BYTE = 0x08U;
    PORTD.PMR.BYTE |= 0x40U;

    R_Config_MTU8_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU8_Start
* Description  : This function starts the MTU8 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU8_Start(void)
{
    /* Enable TGIA8 interrupt in ICU */
    IEN(PERIA, INTA246) = 1U;

    /* Start MTU channel 8 counter */
    MTU.TSTRA.BIT.CST8 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU8_Stop
* Description  : This function stops the MTU8 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU8_Stop(void)
{
    /* Disable TGIA8 interrupt in ICU */
    IEN(PERIA, INTA246) = 0U;

    /* Stop MTU channel 8 counter */
    MTU.TSTRA.BIT.CST8 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
