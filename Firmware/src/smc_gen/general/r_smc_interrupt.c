/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : r_smc_interrupt.c
* Version          : 1.2.100
* Device(s)        : R5F572NNDxFP
* Description      : This file implements interrupt setting.
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
#include "r_smc_interrupt.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Interrupt_Create
* Description  : This function Used to set the fast interrupt or group interrupt 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Interrupt_Create(void)
{
    /* Disable group BE0 interrupt*/
    IEN(ICU,GROUPBE0) = 0U;
    
    /* Disable group BL1 interrupt*/
    IEN(ICU,GROUPBL1) = 0U;
    
    /* Disable group AL0 interrupt*/
    IEN(ICU,GROUPAL0) = 0U;
    

    /* Set group BE0 interrupt priority level */
    IPR(ICU,GROUPBE0) = _02_ICU_PRIORITY_LEVEL2;

    /* Set group BL1 interrupt priority level */
    IPR(ICU,GROUPBL1) = _0F_ICU_PRIORITY_LEVEL15;

    /* Set group AL0 interrupt priority level */
    IPR(ICU,GROUPAL0) = _0F_ICU_PRIORITY_LEVEL15;

    /* Enable group BE0 interrupt */
    IEN(ICU,GROUPBE0) = 1U;

    /* Enable group BL1 interrupt */
    IEN(ICU,GROUPBL1) = 1U;

    /* Enable group AL0 interrupt */
    IEN(ICU,GROUPAL0) = 1U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
