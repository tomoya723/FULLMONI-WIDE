/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU8_user.c
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
* Function Name: R_Config_MTU8_Create_UserInit
* Description  : This function adds user code after initializing the MTU8 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU8_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU8_tgia8_interrupt
* Description  : This function is TGIA8 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_MTU8_tgia8_interrupt(void)
{
    
    /* Start user code for r_Config_MTU8_tgia8_interrupt. Do not edit comment generated here */
	if(g_sp_int_flg == 200)
	{
		sp_TGRA = 0;
		g_sp_int_flg = 0;
	}
	else
	{
		g_sp_int_flg = 0;
		if((MTU8.TGRA >= 350) && (MTU8.TGRA <= 83000))
		{
			sp_TGRA = MTU8.TGRA;
			sp_int ++;
		}
	}
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
