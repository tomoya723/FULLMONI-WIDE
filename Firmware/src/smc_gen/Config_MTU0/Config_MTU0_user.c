/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_MTU0_user.c
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_MTU0.
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
#include "Config_MTU0.h"
/* Start user code for include. Do not edit comment generated here */
#include "dataregister.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
unsigned int g_fps_time;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU0_Create_UserInit
* Description  : This function adds user code after initializing the MTU0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU0_tgia0_interrupt
* Description  : This function is TGIA0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_MTU0_tgia0_interrupt(void)
{
    
    /* Start user code for r_Config_MTU0_tgia0_interrupt. Do not edit comment generated here */

	// --------------------------------------------------------------------
	// interrupt flag clear
	// --------------------------------------------------------------------
//	TPU0.TSR.BIT.TGFA = 0;

	PORT9.PODR.BIT.B2 = ~PORT9.PODR.BIT.B2;
	// --------------------------------------------------------------------
	// 10msec counter increment
	// --------------------------------------------------------------------
	g_int10mscnt ++;
	g_fps_time ++;
	sch_10ms();
	if(g_fps_time >= 100)
	{
			g_fps_max = g_fps_cnt;
			g_fps_cnt = 0;
			g_fps_time = 0;
	}

	S12AD.ADCSR.BIT.ADST = 1;         // ADC start
	while(S12AD.ADCSR.BIT.ADST==1);   // wait for ADC completion
	g_CALC_data.AD0 = 0.5    * g_CALC_data.AD0 + 0.5    * (float)S12AD.ADDR0;
	g_CALC_data.AD1 = 0.5    * g_CALC_data.AD1 + 0.5    * (float)S12AD.ADDR1;
	g_CALC_data.AD2 = 0.5    * g_CALC_data.AD2 + 0.5    * (float)S12AD.ADDR2;
	g_CALC_data.AD3 = 0.5    * g_CALC_data.AD3 + 0.5    * (float)S12AD.ADDR3;
	g_CALC_data.AD4 = 0.5    * g_CALC_data.AD4 + 0.5    * (float)S12AD.ADDR4;
	g_CALC_data.AD5 = 0.99   * g_CALC_data.AD5 + 0.01   * (float)S12AD.ADDR5;
	g_CALC_data.AD6 = 0.5    * g_CALC_data.AD6 + 0.5    * (float)S12AD.ADDR6;
	g_CALC_data.AD7 = 0.9999 * g_CALC_data.AD7 + 0.0001 * (float)S12AD.ADDR7;
	g_CALC_data.sp  = 0.99   * g_CALC_data.sp  + 0.01   * (float)(704372/sp_TGRA);

	g_sp_int_flg ++;
	if(g_sp_int_flg > 100)
	{
		g_sp_int_flg = 100;
		g_CALC_data.sp = 0;
	}

	// SW Input
//	if(PORT2.PIDR.BIT.B7 == 1)
	if(PORTD.PIDR.BIT.B0 == 0)
	{
		g_sw_cnt ++;
		if(g_sw_cnt > 100)
			g_sw_cnt = 100;
	}
	else
	{
		g_sw_cnt = 0;
	}
	if(g_sw_cnt == 10)
	{
		g_sw_int_flg = 1;	// single click
	}
	else if(g_sw_cnt >= 50)
	{
		g_sw_int_flg = 2;	// long click
	}
	else
	{
		//
	}
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
