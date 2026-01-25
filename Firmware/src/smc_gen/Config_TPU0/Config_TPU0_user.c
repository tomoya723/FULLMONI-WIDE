/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_TPU0_user.c
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
#include "speaker.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_TPU0_Create_UserInit
* Description  : This function adds user code after initializing the TPU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_TPU0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_TPU0_tgi0a_interrupt
* Description  : This function is TGI0A interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_TPU0_tgi0a_interrupt(void)
{
    
    /* Start user code for r_Config_TPU0_tgi0a_interrupt. Do not edit comment generated here */
    speaker_tpu0_isr();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
