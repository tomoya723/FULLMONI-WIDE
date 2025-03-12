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
* File Name        : r_cg_interrupt_handlers.h
* Version          : 1.0.140
* Device(s)        : R5F572NNDxFP
* Description      : This file declares interrupt handlers.
***********************************************************************************************************************/

#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* RIIC1 RXI1 */
void r_Config_RIIC1_receive_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(RIIC1,RXI1))));

/* RIIC1 TXI1 */
void r_Config_RIIC1_transmit_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(RIIC1,TXI1))));

/* RIIC0 RXI0 */
void r_Config_RIIC0_receive_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(RIIC0,RXI0))));

/* RIIC0 TXI0 */
void r_Config_RIIC0_transmit_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(RIIC0,TXI0))));

/* S12AD S12ADI */
void r_Config_S12AD0_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIB,INTB186))));

/* MTU0 TGIA0 */
void r_Config_MTU0_tgia0_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIA,INTA209))));

/* MTU8 TGIA8 */
void r_Config_MTU8_tgia8_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIA,INTA246))));

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
