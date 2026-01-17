/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_DA.c
* Component Version: 1.11.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_DA.
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
#include "Config_DA.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_DA_Create
* Description  : This function initializes the DA converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DA_Create(void)
{
    /* Cancel DA stop state in LPC */
    MSTP(DA) = 0U;
    
    /* Set DA format select register */
    DA.DADPR.BYTE = _00_DA_DPSEL_R;

    /* Set D/A-A/D synchronous control register */
    DA.DAADSCR.BYTE = _00_DA_DAADSYNC_DISABLE;

    /* Set DA1 pin */
    PORT0.PMR.BYTE &= 0xDFU;
    PORT0.PDR.BYTE &= 0xDFU;
    MPC.P05PFS.BYTE = 0x80U;

    R_Config_DA_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_DA1_Start
* Description  : This function enables the DA1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DA1_Start(void)
{
    DA.DACR.BIT.DAE = 0U;
    DA.DACR.BIT.DAOE1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_DA1_Stop
* Description  : This function stops the DA1 converter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_DA1_Stop(void)
{
    DA.DACR.BIT.DAOE1 = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_DA1_Set_ConversionValue
* Description  : This function sets the DA1 converter value
* Arguments    : reg_value -
*                    value of conversion
* Return Value : None
***********************************************************************************************************************/

void R_Config_DA1_Set_ConversionValue(uint16_t reg_value)
{
    DA.DADR1 = (uint16_t)(reg_value & 0x0FFFU);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
