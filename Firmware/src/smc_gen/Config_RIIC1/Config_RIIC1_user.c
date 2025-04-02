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
* File Name        : Config_RIIC1_user.c
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_RIIC1.
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
#include "Config_RIIC1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t    g_riic1_mode_flag;               /* RIIC1 master transmit receive flag */
extern volatile uint8_t    g_riic1_state;                   /* RIIC1 master state */
extern volatile uint16_t   g_riic1_slave_address;           /* RIIC1 slave address */
extern volatile uint8_t   *gp_riic1_tx_address;             /* RIIC1 transmit buffer address */
extern volatile uint16_t   g_riic1_tx_count;                /* RIIC1 transmit data number */
extern volatile uint8_t   *gp_riic1_rx_address;             /* RIIC1 receive buffer address */
extern volatile uint16_t   g_riic1_rx_count;                /* RIIC1 receive data number */
extern volatile uint16_t   g_riic1_rx_length;               /* RIIC1 receive data length */
extern volatile uint8_t    g_riic1_stop_generation;         /* RIIC1 stop condition generation flag */
/* Start user code for global. Do not edit comment generated here */
extern void User_CallBack_transmitend1(void);//extern void User_CallBack_transmitend1();
extern void User_CallBack_receiveend1(void);//extern void User_CallBack_receiveend1();
extern void User_CallBack_receiveerror1(MD_STATUS status);//extern void User_CallBack_error1(MD_STATUS status);
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Create_UserInit
* Description  : This function adds user code after initializing the RIIC1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_transmit_interrupt
* Description  : This function is TXI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_RIIC1_transmit_interrupt(void)
{
    /*Set bit PSW.I = 1 to allow multiple interrupts*/
    R_BSP_SETPSW_I();

    if (_0D_IIC_MASTER_TRANSMIT == g_riic1_mode_flag)
    {
        if (_01_IIC_MASTER_SENDS_ADR_7_W == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(g_riic1_slave_address << 1U);
            g_riic1_state = _05_IIC_MASTER_SENDS_DATA;
        }
        else if (_02_IIC_MASTER_SENDS_ADR_10A_W == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(((g_riic1_slave_address & 0x0300U) >> 7U) | 0x00F0U);
            g_riic1_state = _04_IIC_MASTER_SENDS_ADR_10B;
        }
        else if (_04_IIC_MASTER_SENDS_ADR_10B == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(g_riic1_slave_address & 0x00FFU);
            g_riic1_state = _05_IIC_MASTER_SENDS_DATA;
        }
        else if (_05_IIC_MASTER_SENDS_DATA == g_riic1_state)
        {
            if (0U < g_riic1_tx_count)
            {
                RIIC1.ICDRT = *gp_riic1_tx_address;
                gp_riic1_tx_address++;
                g_riic1_tx_count--;
            } 
            else
            {
                g_riic1_state = _06_IIC_MASTER_SENDS_END;
            }
        }
        else
        {
             /* Do nothing */
        }
    }
    else if (_0C_IIC_MASTER_RECEIVE == g_riic1_mode_flag)
    {
        if (_00_IIC_MASTER_SENDS_ADR_7_R == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)((g_riic1_slave_address << 1U) | 0x0001U);
            g_riic1_state = _08_IIC_MASTER_RECEIVES_START;
        }
        else if (_02_IIC_MASTER_SENDS_ADR_10A_W == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(((g_riic1_slave_address & 0x0300U) >> 7U) | 0x00F0U);
            g_riic1_state = _04_IIC_MASTER_SENDS_ADR_10B;
        }
        else if (_04_IIC_MASTER_SENDS_ADR_10B == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(g_riic1_slave_address & 0x00FFU);
            g_riic1_state = _0E_IIC_MASTER_RECEIVES_RESTART;
        }
        else if (_03_IIC_MASTER_SENDS_ADR_10A_R == g_riic1_state)
        {
            RIIC1.ICDRT = (uint8_t)(((g_riic1_slave_address & 0x0300U) >> 7U) | 0x00F1U);
            g_riic1_state = _08_IIC_MASTER_RECEIVES_START;
        }
        else
        {
             /* Do nothing */
        }
    }
    else
    {
         /* Do nothing */
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_transmitend_interrupt
* Description  : This function is TEI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_RIIC1_transmitend_interrupt(void)
{
    /*Set bit PSW.I = 1 to allow multiple interrupts*/
    R_BSP_SETPSW_I();

    if (_06_IIC_MASTER_SENDS_END == g_riic1_state)
    {
        if (1U == g_riic1_stop_generation)
        {
            RIIC1.ICSR2.BIT.STOP = 0U;
            RIIC1.ICCR2.BIT.SP = 1U;

            g_riic1_state = _07_IIC_MASTER_SENDS_STOP;
        }
        else
        {
            RIIC1.ICSR2.BIT.TEND = 0U;
            r_Config_RIIC1_callback_transmitend();
        }
    }
    else if (_0E_IIC_MASTER_RECEIVES_RESTART == g_riic1_state)
    {
        RIIC1.ICSR2.BIT.START = 0U;
        RIIC1.ICIER.BIT.STIE = 1U;
        RIIC1.ICCR2.BIT.RS = 1U;    /* Set restart condition flag */
        RIIC1.ICSR2.BIT.TEND = 0U;
    }
    else
    {
        /* Do nothing */
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_receive_interrupt
* Description  : This function is RXI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_RIIC1_receive_interrupt(void)
{
    /*Set bit PSW.I = 1 to allow multiple interrupts*/
    R_BSP_SETPSW_I();

    volatile uint8_t dummy;

    if (_08_IIC_MASTER_RECEIVES_START == g_riic1_state)
    {
        if ((2U == g_riic1_rx_length) || (1U == g_riic1_rx_length))
        {
            RIIC1.ICMR3.BIT.WAIT = 1U;
        }

        if (1U == g_riic1_rx_length)
        {
            RIIC1.ICMR3.BIT.ACKWP = 1U;
            RIIC1.ICMR3.BIT.ACKBT = 1U;
        }

        /* Dummy read to release SCL */
        dummy = RIIC1.ICDRR;

        g_riic1_state = _09_IIC_MASTER_RECEIVES_DATA;

        if (1U == g_riic1_rx_length)
        {
            g_riic1_state = _0A_IIC_MASTER_RECEIVES_STOPPING;
        }
    }
    else if (_09_IIC_MASTER_RECEIVES_DATA == g_riic1_state)
    {
        if (g_riic1_rx_count < g_riic1_rx_length)
        {
            if (g_riic1_rx_count == (g_riic1_rx_length - 3))
            {
                RIIC1.ICMR3.BIT.WAIT = 1U;

                *gp_riic1_rx_address = RIIC1.ICDRR;
                gp_riic1_rx_address++;
                g_riic1_rx_count++;
            }
            else if (g_riic1_rx_count == (g_riic1_rx_length - 2))
            {
                RIIC1.ICMR3.BIT.ACKWP = 1U;
                RIIC1.ICMR3.BIT.ACKBT = 1U;

                *gp_riic1_rx_address = RIIC1.ICDRR;
                gp_riic1_rx_address++;
                g_riic1_rx_count++;

                g_riic1_state = _0A_IIC_MASTER_RECEIVES_STOPPING;
            }
            else
            {
                *gp_riic1_rx_address = RIIC1.ICDRR;
                gp_riic1_rx_address++;
                g_riic1_rx_count++;
            }
        }
    }
    else if (_0A_IIC_MASTER_RECEIVES_STOPPING == g_riic1_state)
    {
        RIIC1.ICSR2.BIT.STOP = 0U;
        RIIC1.ICCR2.BIT.SP = 1U;

        *gp_riic1_rx_address = RIIC1.ICDRR;
        gp_riic1_rx_address++;
        g_riic1_rx_count++;

        RIIC1.ICMR3.BIT.WAIT = 0U;
        g_riic1_state = _0B_IIC_MASTER_RECEIVES_STOP;
    }
    else
    {
         /* Do nothing */
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_error_interrupt
* Description  : This function is EEI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_RIIC1_error_interrupt(void)
{
    /*Set bit PSW.I = 1 to allow multiple interrupts*/
    R_BSP_SETPSW_I();

    volatile uint8_t dummy;

    if ((1U == RIIC1.ICIER.BIT.ALIE) && (1U == RIIC1.ICSR2.BIT.AL))
    {
        r_Config_RIIC1_callback_error(MD_ERROR1);
    }
    else if ((1U == RIIC1.ICIER.BIT.TMOIE) && (1U == RIIC1.ICSR2.BIT.TMOF))
    {
        r_Config_RIIC1_callback_error(MD_ERROR2);
    }
    else if ((1U == RIIC1.ICIER.BIT.NAKIE) && (1U == RIIC1.ICSR2.BIT.NACKF))
    {
        if (_0D_IIC_MASTER_TRANSMIT == g_riic1_mode_flag)
        {
            RIIC1.ICSR2.BIT.STOP = 0U;
            RIIC1.ICCR2.BIT.SP = 1U;
            RIIC1.ICSR2.BIT.NACKF = 0U;
            g_riic1_state = _07_IIC_MASTER_SENDS_STOP;
        }
        else if (_0C_IIC_MASTER_RECEIVE == g_riic1_mode_flag)
        {
            RIIC1.ICSR2.BIT.STOP = 0U;
            RIIC1.ICCR2.BIT.SP = 1U;

            /* Dummy read the ICDRR register */
            dummy = RIIC1.ICDRR;
            RIIC1.ICSR2.BIT.NACKF = 0U;
            g_riic1_state = _0B_IIC_MASTER_RECEIVES_STOP;
        }
        else
        {
            /* Do nothing */
        }

        r_Config_RIIC1_callback_error(MD_ERROR3);
    }
    else if (_0D_IIC_MASTER_TRANSMIT == g_riic1_mode_flag)
    {
        if ((_01_IIC_MASTER_SENDS_ADR_7_W == g_riic1_state) || (_02_IIC_MASTER_SENDS_ADR_10A_W == g_riic1_state))
        {
            RIIC1.ICSR2.BIT.START = 0U;
            RIIC1.ICIER.BIT.STIE = 0U;
            RIIC1.ICIER.BIT.SPIE = 1U;
        }
        else if (_07_IIC_MASTER_SENDS_STOP == g_riic1_state)
        {
            RIIC1.ICSR2.BIT.NACKF = 0U;
            RIIC1.ICSR2.BIT.STOP = 0U;
            RIIC1.ICIER.BIT.SPIE = 0U;
            RIIC1.ICIER.BIT.STIE = 1U;

            r_Config_RIIC1_callback_transmitend();
        }
        else
        {
            r_Config_RIIC1_callback_error(MD_ERROR4);
        }
    }
    else if (_0C_IIC_MASTER_RECEIVE == g_riic1_mode_flag)
    {
        if ((_00_IIC_MASTER_SENDS_ADR_7_R == g_riic1_state) || (_02_IIC_MASTER_SENDS_ADR_10A_W == g_riic1_state))
        {
            RIIC1.ICSR2.BIT.START = 0U;
            RIIC1.ICIER.BIT.STIE = 0U;
            RIIC1.ICIER.BIT.SPIE = 1U;
        }
        else if (_0E_IIC_MASTER_RECEIVES_RESTART == g_riic1_state)
        {
            RIIC1.ICSR2.BIT.START = 0U;
            RIIC1.ICIER.BIT.STIE = 0U;
            g_riic1_state = _03_IIC_MASTER_SENDS_ADR_10A_R;
        }
        else if (_0B_IIC_MASTER_RECEIVES_STOP == g_riic1_state)
        {
            RIIC1.ICSR2.BIT.NACKF = 0U;
            RIIC1.ICSR2.BIT.STOP = 0U;
            RIIC1.ICIER.BIT.SPIE = 0U;
            RIIC1.ICIER.BIT.STIE = 1U;

            r_Config_RIIC1_callback_receiveend();
        }
        else
        {
            r_Config_RIIC1_callback_error(MD_ERROR4);
        }
    }
    else
    {
        r_Config_RIIC1_callback_error(MD_ERROR4);
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_callback_transmitend
* Description  : This function is a callback function when RIIC1 finishes transmission
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_RIIC1_callback_transmitend(void)
{
    /* Start user code for r_Config_RIIC1_callback_transmitend. Do not edit comment generated here */
	User_CallBack_transmitend1();
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_callback_receiveend
* Description  : This function is a callback function when RIIC1 finishes reception
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_RIIC1_callback_receiveend(void)
{
    /* Start user code for r_Config_RIIC1_callback_receiveend. Do not edit comment generated here */
	User_CallBack_receiveend1();
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_RIIC1_callback_error
* Description  : This function is a callback function when RIIC1 encounters error
* Arguments    : status -
*                    error status
* Return Value : None
***********************************************************************************************************************/

static void r_Config_RIIC1_callback_error(MD_STATUS status)
{
    switch (status)
    {
        case MD_ERROR1:
        {
            /* Start user code for arbitration-lost error. Do not edit comment generated here */
        	User_CallBack_receiveerror1(status);
            /* End user code. Do not edit comment generated here */

            RIIC1.ICSR2.BIT.AL = 0U;
            break;
        }
        case MD_ERROR2:
        {
            if (1U == RIIC1.ICCR1.BIT.SCLI)
            {
                uint8_t count = 0U;

                /* Try outputting additional clock pulses to release SDA */
                while ((0U == RIIC1.ICCR1.BIT.SDAI) && (count < 0x0AU))
                {
                    RIIC1.ICCR1.BIT.CLO = 1U;
                    count++;
                }
            }

            /* Start user code for timeout error. Do not edit comment generated here */
        	User_CallBack_receiveerror1(status);
           /* End user code. Do not edit comment generated here */

            RIIC1.ICSR2.BIT.TMOF = 0U;
            break;
        }
        case MD_ERROR3:
        {
            /* Start user code for NACK signal. Do not edit comment generated here */
        	User_CallBack_receiveerror1(status);
           /* End user code. Do not edit comment generated here */
            break;
        }
        case MD_ERROR4:
        {
            /* Start user code for communication sequence error. Do not edit comment generated here */
        	User_CallBack_receiveerror1(status);
            /* End user code. Do not edit comment generated here */
            break;
        }
        default:
        {
            break;
        }
    }

    /* Start user code for others. Do not edit comment generated here */
	User_CallBack_receiveerror1(status);
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
