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
* File Name        : Config_RIIC1.c
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
volatile uint8_t  g_riic1_mode_flag;               /* RIIC1 master transmit receive flag */
volatile uint8_t  g_riic1_state;                   /* RIIC1 state */
volatile uint16_t g_riic1_slave_address;           /* RIIC1 slave address */
volatile uint8_t *gp_riic1_tx_address;             /* RIIC1 transmit buffer address */
volatile uint16_t g_riic1_tx_count;                /* RIIC1 transmit data number */
volatile uint8_t *gp_riic1_rx_address;             /* RIIC1 receive buffer address */
volatile uint16_t g_riic1_rx_count;                /* RIIC1 receive data number */
volatile uint16_t g_riic1_rx_length;               /* RIIC1 receive data length */
volatile uint8_t  g_riic1_dummy_read_count;        /* RIIC1 count for dummy read */
volatile uint8_t  g_riic1_stop_generation;         /* RIIC1 stop condition generation flag */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Create
* Description  : This function initializes the RIIC1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_Create(void)
{
    /* Cancel RIIC stop state */
    MSTP(RIIC1) = 0U;
    RIIC1.ICCR1.BIT.ICE = 0U;
    RIIC1.ICCR1.BIT.IICRST = 1U;
    RIIC1.ICCR1.BIT.ICE = 1U;

    /* Set transfer bit rate */
    RIIC1.ICMR1.BYTE |= _10_IIC_PCLK_DIV_2;
    RIIC1.ICBRL.BYTE = _FC_IIC1_SCL_LOW_LEVEL_PERIOD;
    RIIC1.ICBRH.BYTE = _FC_IIC1_SCL_HIGH_LEVEL_PERIOD;
    RIIC1.ICMR2.BYTE = 0x00U;
    RIIC1.ICMR3.BIT.NF = _00_IIC_NOISE_FILTER_1;
    RIIC1.ICMR3.BIT.SMBS = 0U;

    /* Set ICFER */
    RIIC1.ICFER.BYTE = _00_IIC_TIMEOUT_FUNCTION_DISABLE | _02_IIC_MASTER_ARBITRATION_ENABLE | 
                       _00_IIC_NACK_ARBITRATION_DISABLE | _10_IIC_NACK_SUSPENSION_ENABLE | _20_IIC_NOISE_FILTER_USED | 
                       _40_IIC_SCL_SYNCHRONOUS_USED | _00_IIC_FASTPLUS_MODE_DISABLE;

    /* Set ICIER */
    RIIC1.ICIER.BYTE = _02_IIC_ARBITRATION_LOST_INTERRUPT_ENABLE | _04_IIC_START_CONDITION_INTERRUPT_ENABLE | 
                       _08_IIC_STOP_CONDITION_INTERRUPT_ENABLE | _10_IIC_NACK_INTERRUPT_ENABLE | 
                       _20_IIC_RECEIVE_DATA_INTERRUPT_ENABLE | _40_IIC_TRANSMIT_END_INTERRUPT_ENABLE | 
                       _80_IIC_TRANSMIT_EMPTY_INTERRUPT_ENABLE;

    /* Cancel internal reset */
    RIIC1.ICCR1.BIT.IICRST = 0U;

    /* Set interrupt priority */
    IPR(RIIC1, RXI1) = _0E_IIC_PRIORITY_LEVEL14;
    IPR(RIIC1, TXI1) = _0E_IIC_PRIORITY_LEVEL14;

    /* Set SCL1 pin */
    MPC.P21PFS.BYTE = 0x0FU;
    PORT2.PMR.BYTE |= 0x02U;

    /* Set SDA1 pin */
    MPC.P20PFS.BYTE = 0x0FU;
    PORT2.PMR.BYTE |= 0x01U;

    R_Config_RIIC1_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Start
* Description  : This function starts the RIIC1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_Start(void)
{
    /* Clear interrupt flag */
    IR(RIIC1,TXI1) = 0U;
    IR(RIIC1,RXI1) = 0U;

    /* Enable RIIC1 interrupt */
    IEN(RIIC1,TXI1) = 1U;
    IEN(RIIC1,RXI1) = 1U;
    ICU.GENBL1.BIT.EN28 = 1U;
    ICU.GENBL1.BIT.EN29 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Stop
* Description  : This function stops the RIIC1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_Stop(void)
{
    /* Clear interrupt flag */
    IR(RIIC1,TXI1) = 0U;
    IR(RIIC1,RXI1) = 0U;

    /* Disable RIIC1 interrupt */
    IEN(RIIC1,TXI1) = 0U;
    IEN(RIIC1,RXI1) = 0U;
    ICU.GENBL1.BIT.EN28 = 0U;
    ICU.GENBL1.BIT.EN29 = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_IIC_StartCondition
* Description  : This function generates I2C start condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_IIC_StartCondition(void)
{
    RIIC1.ICCR2.BIT.ST = 1U;    /* Set start condition flag */
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_IIC_StopCondition
* Description  : This function generates I2C stop condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_RIIC1_IIC_StopCondition(void)
{
    RIIC1.ICCR2.BIT.SP = 1U;    /* Set stop condition flag */
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Master_Send
* Description  : This function sends RIIC1 data to slave device and generates stop condition when transmission finishes
* Arguments    : adr -
*                    address of slave device
*                tx_buf -
*                    transmit buffer pointer
*                tx_num -
*                    transmit data length
* Return Value : status -
*                    MD_OK: OK
*                    MD_ERROR1: Bus is busy
*                    MD_ERROR2: Slave address is invalid
***********************************************************************************************************************/

MD_STATUS R_Config_RIIC1_Master_Send(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U == RIIC1.ICCR2.BIT.BBSY)
    {
        status = MD_ERROR1;
    }
    else if (0x03FFU < adr)
    {
        status = MD_ERROR2;
    }
    else
    {
        /* Set parameter */
        g_riic1_tx_count = tx_num;
        gp_riic1_tx_address = tx_buf;
        g_riic1_slave_address = adr;
        g_riic1_mode_flag = _0D_IIC_MASTER_TRANSMIT;

        if (0x80U > g_riic1_slave_address)
        {
            g_riic1_state = _01_IIC_MASTER_SENDS_ADR_7_W;
        }
        else
        {
            g_riic1_state = _02_IIC_MASTER_SENDS_ADR_10A_W;
        }

        /* Issue a start condition */
        R_Config_RIIC1_IIC_StartCondition();

        /* Set flag for generating stop condition when transmission finishes */
        g_riic1_stop_generation = 1;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Master_Send_Without_Stop
* Description  : This function sends RIIC1 data to slave device, not generate stop condition when transmission finishes
* Arguments    : adr -
*                    address of slave device
*                tx_buf -
*                    transmit buffer pointer
*                tx_num -
*                    transmit data length
* Return Value : status -
*                    MD_OK: OK
*                    MD_ERROR1: Bus is busy
*                    MD_ERROR2: Slave address is invalid
***********************************************************************************************************************/

MD_STATUS R_Config_RIIC1_Master_Send_Without_Stop(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U == RIIC1.ICCR2.BIT.BBSY)
    {
        status = MD_ERROR1;
    }
    else if (0x03FFU < adr)
    {
        status = MD_ERROR2;
    }
    else
    {
        /* Set parameter */
        g_riic1_tx_count = tx_num;
        gp_riic1_tx_address = tx_buf;
        g_riic1_slave_address = adr;
        g_riic1_mode_flag = _0D_IIC_MASTER_TRANSMIT;

        if (0x80U > g_riic1_slave_address)
        {
            g_riic1_state = _01_IIC_MASTER_SENDS_ADR_7_W;
        }
        else
        {
            g_riic1_state = _02_IIC_MASTER_SENDS_ADR_10A_W;
        }

        /* Issue a start condition */
        R_Config_RIIC1_IIC_StartCondition();

        /* Set flag for not generating stop condition when transmission finishes */
        g_riic1_stop_generation = 0;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_RIIC1_Master_Receive
* Description  : This function receives RIIC1 data from slave device
* Arguments    : adr -
*                    address of slave device
*                rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    receive data length
* Return Value : status -
*                    MD_OK: OK
*                    MD_ERROR2: Slave address is invalid
*                    MD_ERROR4: Timeout or arbitration-lost error
*                    MD_ERROR5: Bus is busy
***********************************************************************************************************************/

MD_STATUS R_Config_RIIC1_Master_Receive(uint16_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (0x03FFU < adr)
    {
        status = MD_ERROR2;
    }
    else
    {
        /* Set parameter */
        g_riic1_rx_length = rx_num;
        g_riic1_rx_count = 0U;
        gp_riic1_rx_address = rx_buf;
        g_riic1_slave_address = adr;
        g_riic1_dummy_read_count = 0U;
        g_riic1_mode_flag = _0C_IIC_MASTER_RECEIVE;
        if (0x80U > g_riic1_slave_address)
        {
            g_riic1_state = _00_IIC_MASTER_SENDS_ADR_7_R;
        }
        else
        {
            g_riic1_state = _02_IIC_MASTER_SENDS_ADR_10A_W;
        }

        if (1U == RIIC1.ICCR2.BIT.BBSY)
        {
            /* Has a stop been issued or detected? */
            if ((1U == RIIC1.ICCR2.BIT.SP) || (1U == RIIC1.ICSR2.BIT.STOP))
            {
                /* Wait for the bus to become idle */
                /* WAIT_LOOP */
                do
                {
                    /* Arbitration lost or timeout? */
                    if ((1U == RIIC1.ICSR2.BIT.TMOF) || (1U == RIIC1.ICSR2.BIT.AL))
                    {
                        return (MD_ERROR4);
                    }
                } while (1U == RIIC1.ICCR2.BIT.BBSY);

                /* Issue a start condition */
                R_Config_RIIC1_IIC_StartCondition();
            }
            /* Bus is busy and it is master mode (MST = 1) */
            else if (1U == RIIC1.ICCR2.BIT.MST)
            {
                /* Issue a restart condition */
                RIIC1.ICSR2.BIT.START = 0U;
                RIIC1.ICIER.BIT.STIE = 1U;
                RIIC1.ICCR2.BIT.RS = 1U;
            }
            else
            {
                /* Another master must have the bus */
                status = MD_ERROR5;
            }
        }
        else
        {
            /* Issue a start condition */
            R_Config_RIIC1_IIC_StartCondition();
        }
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
