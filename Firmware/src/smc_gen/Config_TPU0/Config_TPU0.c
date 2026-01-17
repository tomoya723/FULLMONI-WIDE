/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_TPU0.c
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_TPU0.
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
#include "Config_TPU0.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_TPU0_Create
* Description  : This function initializes the TPU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_TPU0_Create(void)
{
    /* Release TPU channel 0 from stop state */
    MSTP(TPU0) = 0U;

    /* Stop TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 0U;

    /* Set TGI0A interrupt priority level */
    ICU.SLIBXR130.BYTE = 0x0FU;
    IPR(PERIB, INTB130) = _0F_TPU_PRIORITY_LEVEL15;

    /* TPU channel 0 is used as normal mode */
    TPUA.TSYR.BIT.SYNC0 = 0U;
    TPU0.TCR.BYTE = _00_TPU_PCLK_1 | _20_TPU_CKCL_A;
    TPU0.TIER.BYTE = _01_TPU_TGIEA_ENABLE | _00_TPU_TGIEB_DISABLE | _00_TPU_TGIEC_DISABLE | _00_TPU_TGIED_DISABLE | 
                     _00_TPU_TCIEV_DISABLE | _00_TPU_TTGE_DISABLE | _40_TPU_TIER_DEFAULT;
    TPU0.TIORH.BYTE = _00_TPU_IOA_DISABLE | _00_TPU_IOB_DISABLE;
    TPU0.TIORL.BYTE = _00_TPU_IOC_DISABLE | _00_TPU_IOD_DISABLE;
    TPU0.TGRA = _176F_TGRA0_VALUE;
    TPU0.TGRB = _176F_TGRB0_VALUE;
    TPU0.TGRC = _176F_TGRC0_VALUE;
    TPU0.TGRD = _176F_TGRD0_VALUE;

    R_Config_TPU0_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_TPU0_Start
* Description  : This function starts the TPU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_TPU0_Start(void)
{
    /* Enable TGI0A interrupt in ICU */
    IEN(PERIB, INTB130) = 1U;
    
    /* Start TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_TPU0_Stop
* Description  : This function stops the TPU0 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_TPU0_Stop(void)
{
    /* Disable TGI0A interrupt in ICU */
    IEN(PERIB, INTB130) = 0U;
    
    /* Stop TPU channel 0 counter */
    TPUA.TSTR.BIT.CST0 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
