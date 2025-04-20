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
* File Name        : Config_RTC.c
* Component Version: 1.8.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_RTC.
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
#include "Config_RTC.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Create
* Description  : This function initializes the RTC module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Create(void)
{
    uint16_t w_count;
    uint32_t rw_count;
    volatile uint32_t dummy;

    /* Disable ALM, PRD and CUP interrupts */
    IEN(RTC, ALM) = 0U;
    IEN(RTC, PRD) = 0U;

    if (RTC.RCR1.BIT.AIE == 1U)
    {
        /* Clear IR flag of ICU ALARM interrupt */
        IR(RTC, ALM) = 0U;
    }

    /* RTC cold start with sub-clock source */
    if(0 == SYSTEM.RSTSR1.BIT.CWSF)
    {
        /* Set RTC clock source */
        RTC.RCR4.BYTE = _00_RTC_SOURCE_SELECT_SUB;

        /* Set sub-clock oscillator */
        /* WAIT_LOOP */
        while (1U != RTC.RCR3.BIT.RTCEN)
        {
            RTC.RCR3.BIT.RTCEN = 1U;
        }

        /* Wait for 6 sub-clock cycles */
        /* WAIT_LOOP */
        for (w_count = 0U; w_count < _2256_RTC_SUB_6_CYCLE_WAIT; w_count++)
        {
            nop();
        }
    }

    /* RTC cold start with sub-clock source or RTC start with main clock source */
    if((0 == SYSTEM.RSTSR1.BIT.CWSF) || (_01_RTC_SOURCE_SELECT_MAIN_FORCED == RTC.RCR4.BYTE))
    {

        /* Stop all counters */
        RTC.RCR2.BYTE = 0x00U;
        /* WAIT_LOOP */
        while (RTC.RCR2.BIT.START != 0U)
        {
            /* Wait for the register modification to complete */
        }

        /* Select count mode */
        RTC.RCR2.BIT.CNTMD = 0U;
        /* WAIT_LOOP */
        while (RTC.RCR2.BIT.CNTMD != 0U)
        {
            /* Wait for the register modification to complete */
        }

        /* Execute RTC software reset */
        RTC.RCR2.BIT.RESET = 1U;
        /* WAIT_LOOP */
        while (RTC.RCR2.BIT.RESET != 0U)
        {
            /* Wait for the register modification to complete */
        }

         /* Stop RTC counter */
        RTC.RCR2.BIT.START = 0U;
        /* WAIT_LOOP */
        while (RTC.RCR2.BIT.START != 0U)
        {
            /* Wait for the register modification to complete */
        }

        /* Clear ALM,PRD,CUP IR */
        IR(RTC, ALM) = 0U;
        IR(RTC, PRD) = 0U;
        RTC.RCR2.BYTE |= (_00_RTC_AUTO_ADJUSTMENT_DISABLE | _40_RTC_HOUR_MODE_24);

        /* Perform 4 read operations after writing */
        /* WAIT_LOOP */
        for (rw_count = 0U; rw_count < _04_FOUR_READ_COUNT; rw_count++)
        {
            dummy = RTC.RCR2.BYTE;
        }
        /* WAIT_LOOP */
        while (RTC.RCR2.BIT.HR24 != 1U)
        {
            /* Wait for the register modification to complete */
        }

        /* Set control registers */
        dummy = _00_RTC_ALARM_INT_DISABLE | _00_RTC_PERIOD_INT_DISABLE | _00_RTC_PERIODIC_INT_PERIOD_DISABLE;
        RTC.RCR1.BYTE = dummy;
        /* WAIT_LOOP */
        while (dummy != RTC.RCR1.BYTE)
        {
            /* Wait for this write to complete. */
        }

    }

    /* Set control registers */
    dummy = RTC.RCR1.BYTE | _00_RTC_CARRY_INT_DISABLE;
    RTC.RCR1.BYTE = dummy;
    /* WAIT_LOOP */
    while (dummy != RTC.RCR1.BYTE)
    {
        /* Wait for this write to complete. */
    }
    dummy = RTC.RCR2.BYTE | _00_RTC_RTCOUT_OUTPUT_DISABLE;
    RTC.RCR2.BYTE = dummy;
    /* WAIT_LOOP */
    while (dummy != RTC.RCR2.BYTE)
    {
        /* Wait for this write to complete. */
    }

    /* Set time capture */
    RTC.RTCCR0.BYTE = _00_RTC_TIME_CAPTURE_NO_EVENT | _00_RTC_TIME_CAPTURE_NOISE_FILTER_OFF | 
                      _00_RTC_TIME_CAPTURE_EVENT_DISABLE;
    RTC.RTCCR1.BYTE = _00_RTC_TIME_CAPTURE_NO_EVENT | _00_RTC_TIME_CAPTURE_NOISE_FILTER_OFF | 
                      _00_RTC_TIME_CAPTURE_EVENT_DISABLE;
    RTC.RTCCR2.BYTE = _00_RTC_TIME_CAPTURE_NO_EVENT | _00_RTC_TIME_CAPTURE_NOISE_FILTER_OFF | 
                      _00_RTC_TIME_CAPTURE_EVENT_DISABLE;

    R_Config_RTC_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Start
* Description  : This function starts RTC counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Start(void)
{

    /* Set the START bit to 1 */
    RTC.RCR2.BIT.START = 1U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.START != 1U)
    {
        /* Wait for the register modification to complete */
    }

}

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Stop
* Description  : This function stops RTC counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Stop(void)
{

    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.START != 0U)
    {
        /* Wait for the register modification to complete */
    }

}

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Restart
* Description  : This function restarts RTC counter
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Restart(rtc_calendarcounter_value_t counter_write_val)
{
    uint32_t rw_count;
    volatile uint32_t dummy;

    /* Disable ALM, PRD and CUP interrupts */
    IEN(RTC, ALM) = 0U;
    IEN(RTC, PRD) = 0U;

    if (RTC.RCR1.BIT.AIE == 1U)
    {
        /* Disable RTC ALARM interrupt */
        RTC.RCR1.BIT.AIE = 0U;
        /* WAIT_LOOP */
        while (RTC.RCR1.BIT.AIE != 0U)
        {
            /* Wait for the register modification to complete */
        }

        /* Clear IR flag of ICU ALARM interrupt */
        IR(RTC, ALM) = 0U;
    }

    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.START != 0U)
    {
        /* Wait for the register modification to complete */
    }

    /* Execute RTC software reset */
    RTC.RCR2.BIT.RESET = 1U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.RESET != 0U)
    {
        /* Wait for the register modification to complete */
    }

    /* Set control registers */
    RTC.RCR2.BYTE |= (_00_RTC_RTCOUT_OUTPUT_DISABLE | _00_RTC_AUTO_ADJUSTMENT_DISABLE | _40_RTC_HOUR_MODE_24);

    /* Perform 4 read operations after writing */
    /* WAIT_LOOP */
    for (rw_count = 0U; rw_count < _04_FOUR_READ_COUNT; rw_count++)
    {
        dummy = RTC.RCR2.BYTE;
    }

    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.HR24 != 1U)
    {
        /* Wait for the register modification to complete */
    }

    /* Set counter values */
    RTC.RSECCNT.BYTE = counter_write_val.rseccnt;
    RTC.RMINCNT.BYTE = counter_write_val.rmincnt;
    RTC.RHRCNT.BYTE = counter_write_val.rhrcnt;
    RTC.RWKCNT.BYTE = counter_write_val.rwkcnt;
    RTC.RDAYCNT.BYTE = counter_write_val.rdaycnt;
    RTC.RMONCNT.BYTE = counter_write_val.rmoncnt;
    RTC.RYRCNT.WORD = counter_write_val.ryrcnt;

    /* Set the START bit to 1 */
    RTC.RCR2.BIT.START = 1U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.START != 1U)
    {
        /* Wait for the register modification to complete */
    }

}

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Get_CalendarCounterValue
* Description  : This function get RTC calendar counter value
* Arguments    : counter_read_val -
*                    counter read pointer
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val)
{

    /* Enable RTC CUP interrupt */
    RTC.RCR1.BYTE |= _02_RTC_CARRY_INT_ENABLE;
    /* WAIT_LOOP */
    do
    {
        ICU.PIBR6.BYTE = 0x02U;
        /* Read counter registers */
        counter_read_val->rseccnt = RTC.RSECCNT.BYTE;
        counter_read_val->rmincnt = RTC.RMINCNT.BYTE;
        counter_read_val->rhrcnt = RTC.RHRCNT.BYTE & 0x3FU;
        counter_read_val->rwkcnt = RTC.RWKCNT.BYTE;
        counter_read_val->rdaycnt = RTC.RDAYCNT.BYTE;
        counter_read_val->rmoncnt = RTC.RMONCNT.BYTE;
        counter_read_val->ryrcnt = RTC.RYRCNT.WORD;
    } while ((ICU.PIBR6.BYTE & 0x02U) != 0U);

    /* Disable RTC CUP interrupt */
    RTC.RCR1.BYTE &= (~_02_RTC_CARRY_INT_ENABLE);

}

/***********************************************************************************************************************
* Function Name: R_Config_RTC_Set_CalendarCounterValue
* Description  : This function set RTC calendar counter value
* Arguments    : counter_write_val -
*                    counter write value
* Return Value : None
***********************************************************************************************************************/

void R_Config_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val)
{
    uint32_t rw_count;
    volatile uint32_t dummy;

    /* Stop all counters */
    RTC.RCR2.BIT.START = 0U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.START != 0U)
    {
        /* Wait for the register modification to complete */
    }

    /* Execute RTC software reset */
    RTC.RCR2.BIT.RESET = 1U;
    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.RESET != 0U)
    {
        /* Wait for the register modification to complete */
    }

    /* Set control registers */
    RTC.RCR2.BYTE |= (_00_RTC_RTCOUT_OUTPUT_DISABLE | _00_RTC_AUTO_ADJUSTMENT_DISABLE | _40_RTC_HOUR_MODE_24);

    /* Perform 4 read operations after writing */
    /* WAIT_LOOP */
    for (rw_count = 0U; rw_count < _04_FOUR_READ_COUNT; rw_count++)
    {
        dummy = RTC.RCR2.BYTE;
    }

    /* WAIT_LOOP */
    while (RTC.RCR2.BIT.HR24 != 1U)
    {
        /* Wait for the register modification to complete */
    }

    /* Set counter values */
    RTC.RSECCNT.BYTE = counter_write_val.rseccnt;
    RTC.RMINCNT.BYTE = counter_write_val.rmincnt;
    RTC.RHRCNT.BYTE = counter_write_val.rhrcnt;
    RTC.RWKCNT.BYTE = counter_write_val.rwkcnt;
    RTC.RDAYCNT.BYTE = counter_write_val.rdaycnt;
    RTC.RMONCNT.BYTE = counter_write_val.rmoncnt;
    RTC.RYRCNT.WORD = counter_write_val.ryrcnt;

}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
