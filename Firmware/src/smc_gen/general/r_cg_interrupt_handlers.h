/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : r_cg_interrupt_handlers.h
* Version          : 1.0.200
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

/* SCI9 RXI9 */
void r_Config_SCI9_receive_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(SCI9,RXI9))));

/* SCI9 TXI9 */
void r_Config_SCI9_transmit_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(SCI9,TXI9))));

/* S12AD S12ADI */
void r_Config_S12AD0_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIB,INTB186))));

/* MTU0 TGIA0 */
void r_Config_MTU0_tgia0_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIA,INTA209))));

/* MTU8 TGIA8 */
void r_Config_MTU8_tgia8_interrupt(void) __attribute__ ((interrupt(".rvectors",VECT(PERIA,INTA246))));

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
