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
* File Name        : r_cg_lvd.h
* Version          : 1.0.140
* Device(s)        : R5F572NNDxFP
* Description      : General header file for LVD peripheral.
***********************************************************************************************************************/

#ifndef LVD_H
#define LVD_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Voltage Monitoring 1 Circuit Control Register 1 (LVD1CR1)
*/
/* Voltage Monitoring 1 Interrupt Generation Condition Select (LVD1IDTSEL[1:0]) */
#define _00_LVD_LVD1IDTSEL_RISE      (0x00U) /* When VCC >= Vdet1 (rise) is detected */
#define _01_LVD_LVD1IDTSEL_DROP      (0x01U) /* When VCC < Vdet1 (drop) is detected */
#define _02_LVD_LVD1IDTSEL_BOTH      (0x02U) /* When drop and rise are detected */
/* Voltage Monitoring 1 Interrupt Type Select (LVD1IRQSEL) */
#define _00_LVD_LVD1IRQSEL_NM        (0x00U) /* Non-maskable interrupt */
#define _04_LVD_LVD1IRQSEL_M         (0x04U) /* Maskable interrupt */

/*
    Voltage Monitoring 2 Circuit Control Register 1 (LVD2CR1)
*/
/* Voltage Monitoring 2 Interrupt Generation Condition Select (LVD2IDTSEL[1:0]) */
#define _00_LVD_LVD2IDTSEL_RISE      (0x00U) /* When VCC >= Vdet2 (rise) is detected */
#define _01_LVD_LVD2IDTSEL_DROP      (0x01U) /* When VCC < Vdet2 (drop) is detected */
#define _02_LVD_LVD2IDTSEL_BOTH      (0x02U) /* When drop and rise are detected */
/* Voltage Monitoring 2 Interrupt Type Select (LVD2IRQSEL) */
#define _00_LVD_LVD2IRQSEL_NM        (0x00U) /* Non-maskable interrupt */
#define _04_LVD_LVD2IRQSEL_M         (0x04U) /* Maskable interrupt */

/*
    Voltage Monitoring Circuit Control Register (LVCMPCR)
*/
/* Voltage Detection 1 Enable (LVD1E) */
#define _00_LVD_LVD1E_DISABLE        (0x00U) /* Voltage detection 1 circuit disabled */
#define _20_LVD_LVD1E_ENABLE         (0x20U) /* Voltage detection 1 circuit enabled */
/* Voltage Detection 2 Enable (LVD2E) */
#define _00_LVD_LVD2E_DISABLE        (0x00U) /* Voltage detection 2 circuit disabled */
#define _40_LVD_LVD2E_ENABLE         (0x40U) /* Voltage detection 2 circuit enabled */

/*
    Voltage Detection Level Select Register (LVDLVLR) 
*/
/* Voltage Detection 1 Level Select (LVD1LVL[3:0]) */
#define _09_LVD_LVD1LVL_2_99         (0x09U) /* 2.99 V */
#define _0A_LVD_LVD1LVL_2_92         (0x0AU) /* 2.92 V */
#define _0B_LVD_LVD1LVL_2_85         (0x0BU) /* 2.85 V */
/* Voltage Detection 2 Level Select (LVD2LVL[1:0]) */
#define _90_LVD_LVD2LVL_2_99         (0x90U) /* 2.99 V */
#define _A0_LVD_LVD2LVL_2_92         (0xA0U) /* 2.92 V */
#define _B0_LVD_LVD2LVL_2_85         (0xB0U) /* 2.85 V */

/*
    Voltage Monitoring 1 Circuit Control Register 0 (LVD1CR0)
*/
/* Voltage Monitoring 1 Interrupt/Reset Enable (LVD1RIE) */
#define _00_LVD_LVD1RIE_DISABLE      (0x00U) /* Disabled */
#define _01_LVD_LVD1RIE_ENABLE       (0x01U) /* Enabled */
/* Voltage Monitoring 1 Digital Filter Disable Mode Select (LVD1DFDIS) */
#define _00_LVD_LVD1DFDIS_ENABLE     (0x00U) /* Digital filter enabled */
#define _02_LVD_LVD1DFDIS_DISABLE    (0x02U) /* Digital filter disabled */
/* Voltage Monitoring 1 Circuit Comparison Result Output Enable (LVD1CMPE) */
#define _00_LVD_LVD1CMPE_DISABLE     (0x00U) /* Voltage monitoring 1 circuit comparison results output disabled */
#define _04_LVD_LVD1CMPE_ENABLE      (0x04U) /* Voltage monitoring 1 circuit comparison results output enabled */
/* Sampling Clock Select (LVD1FSAMP) */
#define _00_LVD_LOCO_2               (0x00U) /* 1/2 LOCO frequency */
#define _10_LVD_LOCO_4               (0x10U) /* 1/4 LOCO frequency */
#define _20_LVD_LOCO_8               (0x20U) /* 1/8 LOCO frequency */
#define _30_LVD_LOCO_16              (0x30U) /* 1/16 LOCO frequency */
/* Voltage Monitoring 1 Circuit Mode Select (LVD1RI) */
#define _00_LVD_LVD1RI_0             (0x00U) /* Voltage monitoring 1 interrupt enabled when Vdet1 is crossed */
#define _40_LVD_LVD1RI_1             (0x40U) /* Voltage monitoring 1 reset enabled when the voltage <= Vdet1 */
/* Voltage Monitoring 1 Circuit Reset Negation Select (LVD1RN) */
#define _00_LVD_LVD1RN_0             (0x00U) /* Negation after VCC > Vdet1 is detected */
#define _80_LVD_LVD1RN_1             (0x80U) /* Negation after assertion of the voltage monitoring 1 reset */

/*
    Voltage Monitoring 2 Circuit Control Register 0 (LVD2CR0)
*/
/* Voltage Monitoring 2 Interrupt/Reset Enable (LVD2RIE) */
#define _00_LVD_LVD2RIE_DISABLE      (0x00U) /* Disabled */
#define _01_LVD_LVD2RIE_ENABLE       (0x01U) /* Enabled */
/* Voltage Monitoring 2 Digital Filter Disable Mode Select (LVD2DFDIS) */
#define _00_LVD_LVD2DFDIS_ENABLE     (0x00U) /* Digital filter enabled */
#define _02_LVD_LVD2DFDIS_DISABLE    (0x02U) /* Digital filter disabled */
/* Voltage Monitoring 2 Circuit Comparison Result Output Enable (LVD2CMPE) */
#define _00_LVD_LVD2CMPE_DISABLE     (0x00U) /* Voltage monitoring 2 circuit comparison results output disabled */
#define _04_LVD_LVD2CMPE_ENABLE      (0x04U) /* Voltage monitoring 2 circuit comparison results output enabled */
/* Voltage Monitoring 2 Circuit Mode Select (LVD2RI) */
#define _00_LVD_LVD2RI_0             (0x00U) /* Voltage monitoring 2 interrupt enabled when Vdet2 is crossed */
#define _40_LVD_LVD2RI_1             (0x40U) /* Voltage monitoring 2 reset enabled when the voltage <= Vdet2 */
/* Voltage Monitoring 2 Circuit Reset Negation Select (LVD2RN) */
#define _00_LVD_LVD2RN_0             (0x00U) /* Negation after VCC > Vdet2 is detected */
#define _80_LVD_LVD2RN_1             (0x80U) /* Negation after assertion of the voltage monitoring 2 reset */

#define _F7_LVD_CR_BIT_MASK          (0xF7U) /* The write value should be 0 for bit3 */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_LVD_PRIORITY_LEVEL0      (0x00U) /* Level 0 (interrupt disabled) */
#define _01_LVD_PRIORITY_LEVEL1      (0x01U) /* Level 1 */
#define _02_LVD_PRIORITY_LEVEL2      (0x02U) /* Level 2 */
#define _03_LVD_PRIORITY_LEVEL3      (0x03U) /* Level 3 */
#define _04_LVD_PRIORITY_LEVEL4      (0x04U) /* Level 4 */
#define _05_LVD_PRIORITY_LEVEL5      (0x05U) /* Level 5 */
#define _06_LVD_PRIORITY_LEVEL6      (0x06U) /* Level 6 */
#define _07_LVD_PRIORITY_LEVEL7      (0x07U) /* Level 7 */
#define _08_LVD_PRIORITY_LEVEL8      (0x08U) /* Level 8 */
#define _09_LVD_PRIORITY_LEVEL9      (0x09U) /* Level 9 */
#define _0A_LVD_PRIORITY_LEVEL10     (0x0AU) /* Level 10 */
#define _0B_LVD_PRIORITY_LEVEL11     (0x0BU) /* Level 11 */
#define _0C_LVD_PRIORITY_LEVEL12     (0x0CU) /* Level 12 */
#define _0D_LVD_PRIORITY_LEVEL13     (0x0DU) /* Level 13 */
#define _0E_LVD_PRIORITY_LEVEL14     (0x0EU) /* Level 14 */
#define _0F_LVD_PRIORITY_LEVEL15     (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
