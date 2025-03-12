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
* File Name        : r_cg_s12ad.h
* Version          : 1.0.140
* Device(s)        : R5F572NNDxFP
* Description      : General header file for S12AD peripheral.
***********************************************************************************************************************/

#ifndef S12AD_H
#define S12AD_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    A/D Control Register (ADCSR)
*/
/* Double Trigger Channel Select (DBLANS) */
#define _0000_AD_DBCHANNEL_ANx00             (0x0000U) /* ANx00 is used for double trigger mode */
#define _0001_AD_DBCHANNEL_ANx01             (0x0001U) /* ANx01 is used for double trigger mode */
#define _0002_AD_DBCHANNEL_ANx02             (0x0002U) /* ANx02 is used for double trigger mode */
#define _0003_AD_DBCHANNEL_ANx03             (0x0003U) /* ANx03 is used for double trigger mode */
#define _0004_AD_DBCHANNEL_ANx04             (0x0004U) /* ANx04 is used for double trigger mode */
#define _0005_AD_DBCHANNEL_ANx05             (0x0005U) /* ANx05 is used for double trigger mode */
#define _0006_AD_DBCHANNEL_ANx06             (0x0006U) /* ANx06 is used for double trigger mode */
#define _0007_AD_DBCHANNEL_ANx07             (0x0007U) /* ANx07 is used for double trigger mode */
#define _0008_AD_DBCHANNEL_AN108             (0x0008U) /* AN108 is used for double trigger mode */
#define _0009_AD_DBCHANNEL_AN109             (0x0009U) /* AN109 is used for double trigger mode */
#define _000A_AD_DBCHANNEL_AN110             (0x000AU) /* AN110 is used for double trigger mode */
#define _000B_AD_DBCHANNEL_AN111             (0x000BU) /* AN111 is used for double trigger mode */
#define _000C_AD_DBCHANNEL_AN112             (0x000CU) /* AN112 is used for double trigger mode */
#define _000D_AD_DBCHANNEL_AN113             (0x000DU) /* AN113 is used for double trigger mode */
#define _000E_AD_DBCHANNEL_AN114             (0x000EU) /* AN114 is used for double trigger mode */
#define _000F_AD_DBCHANNEL_AN115             (0x000FU) /* AN115 is used for double trigger mode */
#define _0010_AD_DBCHANNEL_AN116             (0x0010U) /* AN116 is used for double trigger mode */
#define _0011_AD_DBCHANNEL_AN117             (0x0011U) /* AN117 is used for double trigger mode */
#define _0012_AD_DBCHANNEL_AN118             (0x0012U) /* AN118 is used for double trigger mode */
#define _0013_AD_DBCHANNEL_AN119             (0x0013U) /* AN119 is used for double trigger mode */
#define _0014_AD_DBCHANNEL_AN120             (0x0014U) /* AN120 is used for double trigger mode */
/* Group B Scan End Interrupt Enable (GBADIE) */
#define _0000_AD_GBADI_DISABLE               (0x0000U) /* Disable S12GBADI interrupt generation upon group B scan
                                                          completion */
#define _0040_AD_GBADI_ENABLE                (0x0040U) /* Enable S12GBADI interrupt generation upon group B scan
                                                          completion */
/* Double Trigger Mode Select (DBLE) */
#define _0000_AD_DBLTRIGGER_DISABLE          (0x0000U) /* Disable double trigger mode */
#define _0080_AD_DBLTRIGGER_ENABLE           (0x0080U) /* Enable double trigger mode */
/* Trigger Select (EXTRG) */
#define _0000_AD_SYNC_TRIGGER                (0x0000U) /* A/D conversion started by synchronous trigger */
#define _0100_AD_ASYNC_TRIGGER               (0x0100U) /* A/D conversion started by asynchronous trigger */
/* Trigger Start Enable (TRGE) */
#define _0000_AD_SYNCASYNCTRG_DISABLE        (0x0000U) /* A/D conversion synchronous or asynchronous trigger disable */
#define _0200_AD_SYNCASYNCTRG_ENABLE         (0x0200U) /* A/D conversion synchronous or asynchronous trigger enable */
/* Scan End Interrupt Enable (ADIE) */
#define _0000_AD_SCAN_END_INTERRUPT_DISABLE  (0x0000U) /* Disable S12ADI interrupt generation upon scan completion */
#define _1000_AD_SCAN_END_INTERRUPT_ENABLE   (0x1000U) /* Enable S12ADI interrupt generation upon scan completion */
/* Scan Mode Select (ADCS) */
#define _0000_AD_SINGLE_SCAN_MODE            (0x0000U) /* Single scan mode */
#define _2000_AD_GROUP_SCAN_MODE             (0x2000U) /* Group scan mode */
#define _4000_AD_CONTINUOUS_SCAN_MODE        (0x4000U) /* Continuous scan mode */
/* A/D Conversion Start (ADST) */
#define _0000_AD_CONVERSION_STOP             (0x0000U) /* Stop A/D conversion */
#define _8000_AD_CONVERSION_START            (0x8000U) /* Start A/D conversion */

/*
    A/D Channel Select Register A0 (ADANSA0)
*/
#define _0001_AD_ANx00_USED                  (0x0001U) /* ANx00 is subjected to conversion */
#define _0001_AD_ANx00_GROUPA_USED           (0x0001U) /* ANx00 is subjected to group A conversion */
#define _0002_AD_ANx01_USED                  (0x0002U) /* ANx01 is subjected to conversion */
#define _0002_AD_ANx01_GROUPA_USED           (0x0002U) /* ANx01 is subjected to group A conversion */
#define _0004_AD_ANx02_USED                  (0x0004U) /* ANx02 is subjected to conversion */
#define _0004_AD_ANx02_GROUPA_USED           (0x0004U) /* ANx02 is subjected to group A conversion */
#define _0008_AD_ANx03_USED                  (0x0008U) /* ANx03 is subjected to conversion */
#define _0008_AD_ANx03_GROUPA_USED           (0x0008U) /* ANx03 is subjected to group A conversion */
#define _0010_AD_ANx04_USED                  (0x0010U) /* ANx04 is subjected to conversion */
#define _0010_AD_ANx04_GROUPA_USED           (0x0010U) /* ANx04 is subjected to group A conversion */
#define _0020_AD_ANx05_USED                  (0x0020U) /* ANx05 is subjected to conversion */
#define _0020_AD_ANx05_GROUPA_USED           (0x0020U) /* ANx05 is subjected to group A conversion */
#define _0040_AD_ANx06_USED                  (0x0040U) /* ANx06 is subjected to conversion */
#define _0040_AD_ANx06_GROUPA_USED           (0x0040U) /* ANx06 is subjected to group A conversion */
#define _0080_AD_ANx07_USED                  (0x0080U) /* ANx07 is subjected to conversion */
#define _0080_AD_ANx07_GROUPA_USED           (0x0080U) /* ANx07 is subjected to group A conversion */
#define _0100_AD_AN108_USED                  (0x0100U) /* AN108 is subjected to conversion */
#define _0100_AD_AN108_GROUPA_USED           (0x0100U) /* AN108 is subjected to group A conversion */
#define _0200_AD_AN109_USED                  (0x0200U) /* AN109 is subjected to conversion */
#define _0200_AD_AN109_GROUPA_USED           (0x0200U) /* AN109 is subjected to group A conversion */
#define _0400_AD_AN110_USED                  (0x0400U) /* AN110 is subjected to conversion */
#define _0400_AD_AN110_GROUPA_USED           (0x0400U) /* AN110 is subjected to group A conversion */
#define _0800_AD_AN111_USED                  (0x0800U) /* AN111 is subjected to conversion */
#define _0800_AD_AN111_GROUPA_USED           (0x0800U) /* AN111 is subjected to group A conversion */
#define _1000_AD_AN112_USED                  (0x1000U) /* AN112 is subjected to conversion */
#define _1000_AD_AN112_GROUPA_USED           (0x1000U) /* AN112 is subjected to group A conversion */
#define _2000_AD_AN113_USED                  (0x2000U) /* AN113 is subjected to conversion */
#define _2000_AD_AN113_GROUPA_USED           (0x2000U) /* AN113 is subjected to group A conversion */
#define _4000_AD_AN114_USED                  (0x4000U) /* AN114 is subjected to conversion */
#define _4000_AD_AN114_GROUPA_USED           (0x4000U) /* AN114 is subjected to group A conversion */
#define _8000_AD_AN115_USED                  (0x8000U) /* AN115 is subjected to conversion */
#define _8000_AD_AN115_GROUPA_USED           (0x8000U) /* AN115 is subjected to group A conversion */

/*
    A/D Channel Select Register A1 (ADANSA1)
*/
#define _0001_AD_AN116_USED                  (0x0001U) /* AN116 is subjected to conversion */
#define _0001_AD_AN116_GROUPA_USED           (0x0001U) /* AN116 is subjected to group A conversion */
#define _0002_AD_AN117_USED                  (0x0002U) /* AN117 is subjected to conversion */
#define _0002_AD_AN117_GROUPA_USED           (0x0002U) /* AN117 is subjected to group A conversion */
#define _0004_AD_AN118_USED                  (0x0004U) /* AN118 is subjected to conversion */
#define _0004_AD_AN118_GROUPA_USED           (0x0004U) /* AN118 is subjected to group A conversion */
#define _0008_AD_AN119_USED                  (0x0008U) /* AN119 is subjected to conversion */
#define _0008_AD_AN119_GROUPA_USED           (0x0008U) /* AN119 is subjected to group A conversion */
#define _0010_AD_AN120_USED                  (0x0010U) /* AN120 is subjected to conversion */
#define _0010_AD_AN120_GROUPA_USED           (0x0010U) /* AN120 is subjected to group A conversion */

/*
    A/D Channel Select Register B0 (ADANSB0)
*/
#define _0001_AD_ANx00_GROUPB_USED           (0x0001U) /* ANx00 is subjected to group B conversion */
#define _0002_AD_ANx01_GROUPB_USED           (0x0002U) /* ANx01 is subjected to group B conversion */
#define _0004_AD_ANx02_GROUPB_USED           (0x0004U) /* ANx02 is subjected to group B conversion */
#define _0008_AD_ANx03_GROUPB_USED           (0x0008U) /* ANx03 is subjected to group B conversion */
#define _0010_AD_ANx04_GROUPB_USED           (0x0010U) /* ANx04 is subjected to group B conversion */
#define _0020_AD_ANx05_GROUPB_USED           (0x0020U) /* ANx05 is subjected to group B conversion */
#define _0040_AD_ANx06_GROUPB_USED           (0x0040U) /* ANx06 is subjected to group B conversion */
#define _0080_AD_ANx07_GROUPB_USED           (0x0080U) /* ANx07 is subjected to group B conversion */
#define _0100_AD_AN108_GROUPB_USED           (0x0100U) /* AN108 is subjected to group B conversion */
#define _0200_AD_AN109_GROUPB_USED           (0x0200U) /* AN109 is subjected to group B conversion */
#define _0400_AD_AN110_GROUPB_USED           (0x0400U) /* AN110 is subjected to group B conversion */
#define _0800_AD_AN111_GROUPB_USED           (0x0800U) /* AN111 is subjected to group B conversion */
#define _1000_AD_AN112_GROUPB_USED           (0x1000U) /* AN112 is subjected to group B conversion */
#define _2000_AD_AN113_GROUPB_USED           (0x2000U) /* AN113 is subjected to group B conversion */
#define _4000_AD_AN114_GROUPB_USED           (0x4000U) /* AN114 is subjected to group B conversion */
#define _8000_AD_AN115_GROUPB_USED           (0x8000U) /* AN115 is subjected to group B conversion */

/*
    A/D Channel Select Register B1 (ADANSB1)
*/
#define _0001_AD_AN116_GROUPB_USED           (0x0001U) /* AN116 is subjected to group B conversion */
#define _0002_AD_AN117_GROUPB_USED           (0x0002U) /* AN117 is subjected to group B conversion */
#define _0004_AD_AN118_GROUPB_USED           (0x0004U) /* AN118 is subjected to group B conversion */
#define _0008_AD_AN119_GROUPB_USED           (0x0008U) /* AN119 is subjected to group B conversion */
#define _0010_AD_AN120_GROUPB_USED           (0x0010U) /* AN120 is subjected to group B conversion */

/*
    A/D Channel Select Register C0 (ADANSC0)
*/
#define _0001_AD_ANx00_GROUPC_USED           (0x0001U) /* ANx00 is subjected to group C conversion */
#define _0002_AD_ANx01_GROUPC_USED           (0x0002U) /* ANx01 is subjected to group C conversion */
#define _0004_AD_ANx02_GROUPC_USED           (0x0004U) /* ANx02 is subjected to group C conversion */
#define _0008_AD_ANx03_GROUPC_USED           (0x0008U) /* ANx03 is subjected to group C conversion */
#define _0010_AD_ANx04_GROUPC_USED           (0x0010U) /* ANx04 is subjected to group C conversion */
#define _0020_AD_ANx05_GROUPC_USED           (0x0020U) /* ANx05 is subjected to group C conversion */
#define _0040_AD_ANx06_GROUPC_USED           (0x0040U) /* ANx06 is subjected to group C conversion */
#define _0080_AD_ANx07_GROUPC_USED           (0x0080U) /* ANx07 is subjected to group C conversion */
#define _0100_AD_AN108_GROUPC_USED           (0x0100U) /* AN108 is subjected to group C conversion */
#define _0200_AD_AN109_GROUPC_USED           (0x0200U) /* AN109 is subjected to group C conversion */
#define _0400_AD_AN110_GROUPC_USED           (0x0400U) /* AN110 is subjected to group C conversion */
#define _0800_AD_AN111_GROUPC_USED           (0x0800U) /* AN111 is subjected to group C conversion */
#define _1000_AD_AN112_GROUPC_USED           (0x1000U) /* AN112 is subjected to group C conversion */
#define _2000_AD_AN113_GROUPC_USED           (0x2000U) /* AN113 is subjected to group C conversion */
#define _4000_AD_AN114_GROUPC_USED           (0x4000U) /* AN114 is subjected to group C conversion */
#define _8000_AD_AN115_GROUPC_USED           (0x8000U) /* AN115 is subjected to group C conversion */

/*
    A/D Channel Select Register C1 (ADANSC1)
*/
#define _0001_AD_AN116_GROUPC_USED           (0x0001U) /* AN116 is subjected to group C conversion */
#define _0002_AD_AN117_GROUPC_USED           (0x0002U) /* AN117 is subjected to group C conversion */
#define _0004_AD_AN118_GROUPC_USED           (0x0004U) /* AN118 is subjected to group C conversion */
#define _0008_AD_AN119_GROUPC_USED           (0x0008U) /* AN119 is subjected to group C conversion */
#define _0010_AD_AN120_GROUPC_USED           (0x0010U) /* AN120 is subjected to group C conversion */

/*
    A/D-Converted Value Addition/Average Mode Select Register 0 (ADADS0)
*/
#define _0001_AD_ANx00_ADD_USED              (0x0001U) /* ANx00 is selected to addition/average mode */
#define _0002_AD_ANx01_ADD_USED              (0x0002U) /* ANx01 is selected to addition/average mode */
#define _0004_AD_ANx02_ADD_USED              (0x0004U) /* ANx02 is selected to addition/average mode */
#define _0008_AD_ANx03_ADD_USED              (0x0008U) /* ANx03 is selected to addition/average mode */
#define _0010_AD_ANx04_ADD_USED              (0x0010U) /* ANx04 is selected to addition/average mode */
#define _0020_AD_ANx05_ADD_USED              (0x0020U) /* ANx05 is selected to addition/average mode */
#define _0040_AD_ANx06_ADD_USED              (0x0040U) /* ANx06 is selected to addition/average mode */
#define _0080_AD_ANx07_ADD_USED              (0x0080U) /* ANx07 is selected to addition/average mode */
#define _0100_AD_AN108_ADD_USED              (0x0100U) /* AN108 is selected to addition/average mode */
#define _0200_AD_AN109_ADD_USED              (0x0200U) /* AN109 is selected to addition/average mode */
#define _0400_AD_AN110_ADD_USED              (0x0400U) /* AN110 is selected to addition/average mode */
#define _0800_AD_AN111_ADD_USED              (0x0800U) /* AN111 is selected to addition/average mode */
#define _1000_AD_AN112_ADD_USED              (0x1000U) /* AN112 is selected to addition/average mode */
#define _2000_AD_AN113_ADD_USED              (0x2000U) /* AN113 is selected to addition/average mode */
#define _4000_AD_AN114_ADD_USED              (0x4000U) /* AN114 is selected to addition/average mode */
#define _8000_AD_AN115_ADD_USED              (0x8000U) /* AN115 is selected to addition/average mode */

/*
    A/D-Converted Value Addition/Average Mode Select Register 1 (ADADS1)
*/
#define _0001_AD_AN116_ADD_USED              (0x0001U) /* AN116 is selected to addition/average mode */
#define _0002_AD_AN117_ADD_USED              (0x0002U) /* AN117 is selected to addition/average mode */
#define _0004_AD_AN118_ADD_USED              (0x0004U) /* AN118 is selected to addition/average mode */
#define _0008_AD_AN119_ADD_USED              (0x0008U) /* AN119 is selected to addition/average mode */
#define _0010_AD_AN120_ADD_USED              (0x0010U) /* AN120 is selected to addition/average mode */

/*
    A/D-Converted Value Addition/Average Count Select Register (ADADC)
*/
/* Addition Count Select (ADC) */
#define _00_AD_1_TIME_CONVERSION             (0x00U) /* 1-time conversion */
#define _01_AD_2_TIME_CONVERSION             (0x01U) /* 2-time conversion */
#define _02_AD_3_TIME_CONVERSION             (0x02U) /* 3-time conversion */
#define _03_AD_4_TIME_CONVERSION             (0x03U) /* 4-time conversion */
#define _05_AD_16_TIME_CONVERSION            (0x05U) /* 16-time conversion */
/* Average Mode Enable (AVEE) */
#define _00_AD_ADDITION_MODE                 (0x00U) /* Addition mode */
#define _80_AD_AVERAGE_MODE                  (0x80U) /* Average mode */

/*
    A/D Control Extended Register (ADCER)
*/
/* A/D Conversion Resolution Setting (ADPRC) */
#define _0000_AD_RESOLUTION_12BIT            (0x0000U) /* 12-bit resolution */
#define _0002_AD_RESOLUTION_10BIT            (0x0002U) /* 10-bit resolution */
#define _0004_AD_RESOLUTION_8BIT             (0x0004U) /* 8-bit resolution */
/* A/D Data Register Automatic Clearing Enable (ACE) */
#define _0000_AD_AUTO_CLEARING_DISABLE       (0x0000U) /* Disable auto clearing */
#define _0020_AD_AUTO_CLEARING_ENABLE        (0x0020U) /* Enable auto clearing */
/* Self-Diagnosis Conversion Voltage Select (DIAGVAL) */
#define _0100_AD_SELFTDIAGST_0V              (0x0100U) /* Self-diagnosis using a voltage of 0V */
#define _0200_AD_SELFTDIAGST_VREFH0_HALF     (0x0200U) /* Self-diagnosis using a voltage of VREFH0_1/2 */
#define _0300_AD_SELFTDIAGST_VREFH0          (0x0300U) /* Self-diagnosis using a voltage of VREFH0_1 */
#define _0200_AD_SELFTDIAGST_AVCC1_HALF      (0x0200U) /* Self-diagnosis using a voltage of AVCC1_1/2*/
#define _0300_AD_SELFTDIAGST_AVCC1           (0x0300U) /* Self-diagnosis using a voltage of AVCC1_1*/
/* Self-Diagnosis Mode Select (DIAGLD) */
#define _0000_AD_SELFTDIAGST_ROTATION        (0x0000U) /* Rotation mode for self-diagnosis voltage */
#define _0400_AD_SELFTDIAGST_FIX             (0x0400U) /* Fixed mode for self-diagnosis voltage */
/* Self-Diagnosis Enable (DIAGM) */
#define _0000_AD_SELFTDIAGST_DISABLE         (0x0000U) /* 12-bit self-diagnosis disable */
#define _0800_AD_SELFTDIAGST_ENABLE          (0x0800U) /* 12-bit self-diagnosis enable */
/* A/D Data Register Format Select (ADRFMT) */
#define _0000_AD_RIGHT_ALIGNMENT             (0x0000U) /* Right-alignment for data register format */
#define _8000_AD_LEFT_ALIGNMENT              (0x8000U) /* Left-alignment for data register format */

/*
    A/D Conversion Start Trigger Select Register (ADSTRGR)
*/
/* A/D Conversion Start Trigger Select for Group B (TRSB) */
#define _0001_AD_TRSB_TRGA0N                 (0x0001U) /* Compare match/input capture from MTU0.TGRA */
#define _0002_AD_TRSB_TRGA1N                 (0x0002U) /* Compare match/input capture from MTU1.TGRA */
#define _0003_AD_TRSB_TRGA2N                 (0x0003U) /* Compare match/input capture from MTU2.TGRA */
#define _0004_AD_TRSB_TRGA3N                 (0x0004U) /* Compare match/input capture from MTU3.TGRA */
#define _0005_AD_TRSB_TRGA4N                 (0x0005U) /* Compare match/input capture from MTU4.TGRA,or an
                                                          underflow of MTU4.TCNT (in the trough) in complementary
                                                          PWM mode */
#define _0006_AD_TRSB_TRGA6N                 (0x0006U) /* Compare match/input capture from MTU6.TGRA */
#define _0007_AD_TRSB_TRGA7N                 (0x0007U) /* Compare match/input capture from MTU7.TGRA,or an
                                                          underflow of MTU7.TCNT (in the trough) in complementary
                                                          PWM mode */
#define _0008_AD_TRSB_TRG0N                  (0x0008U) /* Compare match from MTU0.TGRE */
#define _0009_AD_TRSB_TRG4AN                 (0x0009U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _000A_AD_TRSB_TRG4BN                 (0x000AU) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _000B_AD_TRSB_TRG4AN_TRG4BN          (0x000BU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or
                                                          between MTU4.TADCORB and MTU4.TCNT */
#define _000C_AD_TRSB_TRG4ABN                (0x000CU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, and
                                                          between MTU4.TADCORB and MTU4.TCNT (when interrupt skipping
                                                          function 2 is in use) */
#define _000D_AD_TRSB_TRG7AN                 (0x000DU) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _000E_AD_TRSB_TRG7BN                 (0x000EU) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _000F_AD_TRSB_TRG7AN_TRG7BN          (0x000FU) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                          MTU7.TADCORB and MTU7.TCNT */
#define _0010_AD_TRSB_TRG7ABN                (0x0010U) /* Compare match between MTU7.TADCORA and MTU7.TCNT,
                                                          and between MTU7.TADCORB and MTU7.TCNT 
                                                          (when interrupt skipping function 2 is in use) */
#define _001D_AD_TRSB_TMTRG0AN_0             (0x001DU) /* Compare match between TMR0.TCORA0
                                                          and TMR0.TCNT0(unit0,ch0) */
#define _001E_AD_TRSB_TMTRG0AN_1             (0x001EU) /* Compare match between TMR2.TCORA0
                                                          and TMR2.TCNT0(unit1,ch0) */
#define _001F_AD_TRSB_TPTRGAN                (0x001FU) /* Compare match/input capture from TPUn.TGRAn(n = 0to5) */
#define _0020_AD_TRSB_TPTRG0AN               (0x0020U) /* Compare match/input capture from TPU0.TGRA0 */
#define _0011_AD_TRSB_ELCTRGX0N              (0x0011U) /* A/D Startup source 0 from ELC */
#define _0012_AD_TRSB_ELCTRGX1N              (0x0012U) /* A/D Startup source 1 from ELC */
#define _0019_AD_TRSB_ELCTRGX01N             (0x0019U) /* A/D Startup source 0, or
                                                          A/D Startup source 1 from ELC */
/* A/D Conversion Start Trigger Select (TRSA) */
#define _0000_AD_TRSA_ADTRG                  (0x0000U) /* Input pin for the trigger */
#define _0100_AD_TRSA_TRGA0N                 (0x0100U) /* Compare match/input capture from MTU0.TGRA */
#define _0200_AD_TRSA_TRGA1N                 (0x0200U) /* Compare match/input capture from MTU1.TGRA */
#define _0300_AD_TRSA_TRGA2N                 (0x0300U) /* Compare match/input capture from MTU2.TGRA */
#define _0400_AD_TRSA_TRGA3N                 (0x0400U) /* Compare match/input capture from MTU3.TGRA */
#define _0500_AD_TRSA_TRGA4N                 (0x0500U) /* Compare match/input capture from MTU4.TGRA or, in
                                                          complementary PWM mode,an underflow of MTU4.TCNT
                                                          (in the trough)*/
#define _0600_AD_TRSA_TRGA6N                 (0x0600U) /* Compare match/input capture from MTU6.TGRA */
#define _0700_AD_TRSA_TRGA7N                 (0x0700U) /* Compare match/input capture from MTU7.TGRA or, in
                                                          complementary PWM mode,an underflow of MTU7.TCNT
                                                          (in the trough)*/
#define _0800_AD_TRSA_TRG0N                  (0x0800U) /* Compare match from MTU0.TGRE */
#define _0900_AD_TRSA_TRG4AN                 (0x0900U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _0A00_AD_TRSA_TRG4BN                 (0x0A00U) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _0B00_AD_TRSA_TRG4AN_TRG4BN          (0x0B00U) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or between
                                                          MTU4.TADCORB and MTU4.TCNT */
#define _0C00_AD_TRSA_TRG4ABN                (0x0C00U) /* Compare match between MTU4.TADCORA and MTU4.TCNT,
                                                          and between MTU4.TADCORB and MTU4.TCNT
                                                          (when interrupt skipping function 2 is in use) */
#define _0D00_AD_TRSA_TRG7AN                 (0x0D00U) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _0E00_AD_TRSA_TRG7BN                 (0x0E00U) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _0F00_AD_TRSA_TRG7AN_TRG7BN          (0x0F00U) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                          MTU7.TADCORB and MTU7.TCNT */
#define _1000_AD_TRSA_TRG7ABN                (0x1000U) /* Compare match between MTU7.TADCORA and MTU7.TCNT,
                                                          and between MTU7.TADCORB and MTU7.TCNT
                                                          (when interrupt skipping function 2 is in use) */
#define _1D00_AD_TRSA_TMTRG0AN_0             (0x1D00U) /* Compare match between TMR0.TCORA0
                                                          and TMR0.TCNT0(unit0,ch0) */
#define _1E00_AD_TRSA_TMTRG0AN_1             (0x1E00U) /* Compare match between TMR2.TCORA0
                                                          and TMR2.TCNT0(unit1,ch0) */
#define _1F00_AD_TRSA_TPTRGAN                (0x1F00U) /* Compare match/input capture from TPUn.TGRAn(n=0 to 5) */
#define _2000_AD_TRSA_TPTRG0AN               (0x2000U) /* Compare match/input capture from TPU0.TGRAn */
#define _1100_AD_TRSA_ELCTRGX0N              (0x1100U) /* A/D Startup source 0 from ELC */
#define _1200_AD_TRSA_ELCTRGX1N              (0x1200U) /* A/D Startup source 1 from ELC */
#define _1900_AD_TRSA_ELCTRGX01N             (0x1900U) /* A/D Startup source 0, or
                                                          A/D Startup source 1 from ELC */

/*
    A/D Conversion Extended Input Control Register (ADEXICR)
*/
/* Temperature Sensor Output A/D Converted Value Addition/Averaging Mode Select (TSSAD) */
#define _0000_AD_TEMP_ADD_UNUSED             (0x0000U) /* Temperature sensor output A/D converted value addition/average
                                                          mode disabled */
#define _0001_AD_TEMP_ADD_USED               (0x0001U) /* Temperature sensor output A/D converted value addition/average
                                                          mode enabled */
/* Internal Reference Voltage A/D Converted Value Addition/Average Mode Select (OCSAD) */
#define _0000_AD_IREF_ADD_UNUSED             (0x0000U) /* Internal ref. voltage A/D converted value addition/average
                                                          mode disabled */
#define _0002_AD_IREF_ADD_USED               (0x0002U) /* Internal ref. voltage A/D converted value addition/average
                                                          mode enabled */
/* Temperature Sensor Output A/D Conversion Select (TSSA) */
#define _0000_AD_TEMP_UNUSED                 (0x0000U) /* A/D conversion of temperature sensor output is disabled */
#define _0100_AD_TEMP_USED                   (0x0100U) /* A/D conversion of temperature sensor output is enabled */
#define _0100_AD_TEMP_GROUPA_USED            (0x0100U) /* A/D conversion of temperature sensor output is enabled in
                                                          group A */
/* Internal Reference Voltage A/D Conversion Select (OCSA) */
#define _0000_AD_IREF_UNUSED                 (0x0000U) /* A/D conversion of internal reference voltage is disabled */
#define _0200_AD_IREF_USED                   (0x0200U) /* A/D conversion of internal reference voltage is enabled */
#define _0200_AD_IREF_GROUPA_USED            (0x0200U) /* A/D conversion of internal reference voltage is enabled in
                                                          group A */
/* Group B Temperature Sensor Output A/D Conversion Select (TSSB) */
#define _0000_AD_TEMP_GROUPB_UNUSED          (0x0000U) /* A/D conversion of temperature sensor output is disabled in
                                                          group B */
#define _0400_AD_TEMP_GROUPB_USED            (0x0400U) /* A/D conversion of temperature sensor output is enabled in
                                                          group B */
/* Group B Internal Reference Voltage A/D Conversion Select (OCSB) */
#define _0000_AD_IREF_GROUPB_UNUSED          (0x0000U) /* A/D conversion of internal reference voltage is disabled in
                                                          group B */
#define _0800_AD_IREF_GROUPB_USED            (0x0800U) /* A/D conversion of internal reference voltage is enabled in
                                                          group B */
/* Extended Analog Input Select (EXSEL) */
#define _0000_AD_EXTNANEX1_IN_DISABLE        (0x0000U) /* Extended analog input disable */
#define _2000_AD_EXTNANEX1_IN_ENABLE         (0x2000U) /* Extended analog input enable */
/* Extended Analog Output Control (EXOEN) */
#define _0000_AD_EXTNANEX0_OUT_DISABLE       (0x0000U) /* Extended analog output disable */
#define _8000_AD_EXTNANEX0_OUT_ENABLE        (0x8000U) /* Extended analog output enable */

/*
    A/D Group C Extended Input Control Register (ADGCEXCR)
*/
/* Group C Temperature Sensor Output A/D Conversion Select (TSSC) */
#define _00_AD_TEMP_GROUPC_UNUSED            (0x00U)   /* A/D conversion of temperature sensor output is disabled in
                                                          group C */
#define _01_AD_TEMP_GROUPC_USED              (0x01U)   /* A/D conversion of temperature sensor output is enabled in
                                                          group C */
/* Group C Internal Reference Voltage A/D Conversion Select (OCSC) */
#define _00_AD_IREF_GROUPC_UNUSED            (0x00U)   /* A/D conversion of internal reference voltage is disabled in
                                                          group C */
#define _02_AD_IREF_GROUPC_USED              (0x02U)   /* A/D conversion of internal reference voltage is enabled in
                                                          group C */

/*
    A/D Group C Trigger Select Register (ADGCTRGR)
*/
/* Group C A/D Conversion Start Trigger Select (TRSC) */
#define _01_AD_TRSC_TRGA0N                   (0x01U) /* Compare match/input capture from MTU0.TGRA */
#define _02_AD_TRSC_TRGA1N                   (0x02U) /* Compare match/input capture from MTU1.TGRA */
#define _03_AD_TRSC_TRGA2N                   (0x03U) /* Compare match/input capture from MTU2.TGRA */
#define _04_AD_TRSC_TRGA3N                   (0x04U) /* Compare match/input capture from MTU3.TGRA */
#define _05_AD_TRSC_TRGA4N                   (0x05U) /* Compare match/input capture from MTU4.TGRA,or an
                                                        underflow of MTU4.TCNT (in the trough) in complementary
                                                        PWM mode */
#define _06_AD_TRSC_TRGA6N                   (0x06U) /* Compare match/input capture from MTU6.TGRA */
#define _07_AD_TRSC_TRGA7N                   (0x07U) /* Compare match/input capture from MTU7.TGRA,or an
                                                        underflow of MTU7.TCNT (in the trough) in complementary
                                                        PWM mode */
#define _08_AD_TRSC_TRG0N                    (0x08U) /* Compare match from MTU0.TGRE */
#define _09_AD_TRSC_TRG4AN                   (0x09U) /* Compare match between MTU4.TADCORA and MTU4.TCNT */
#define _0A_AD_TRSC_TRG4BN                   (0x0AU) /* Compare match between MTU4.TADCORB and MTU4.TCNT */
#define _0B_AD_TRSC_TRG4AN_TRG4BN            (0x0BU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, or
                                                        between MTU4.TADCORB and MTU4.TCNT */
#define _0C_AD_TRSC_TRG4ABN                  (0x0CU) /* Compare match between MTU4.TADCORA and MTU4.TCNT, and
                                                        between MTU4.TADCORB and MTU4.TCNT (when interrupt skipping
                                                        function 2 is in use) */
#define _0D_AD_TRSC_TRG7AN                   (0x0DU) /* Compare match between MTU7.TADCORA and MTU7.TCNT */
#define _0E_AD_TRSC_TRG7BN                   (0x0EU) /* Compare match between MTU7.TADCORB and MTU7.TCNT */
#define _0F_AD_TRSC_TRG7AN_TRG7BN            (0x0FU) /* Compare match between MTU7.TADCORA and MTU7.TCNT, or between
                                                        MTU7.TADCORB and MTU7.TCNT */
#define _10_AD_TRSC_TRG7ABN                  (0x10U) /* Compare match between MTU7.TADCORA and MTU7.TCNT, and between
                                                        MTU7.TADCORB and MTU7.TCNT (when interrupt skipping function
                                                        2 is in use) */
#define _1D_AD_TRSC_TMTRG0AN_0               (0x1DU) /* Compare match between TMR0.TCORA0 and TMR0.TCNT0(unit0,ch0) */
#define _1E_AD_TRSC_TMTRG0AN_1               (0x1EU) /* Compare match between TMR2.TCORA0 and TMR2.TCNT0(unit1,ch0) */
#define _1F_AD_TRSC_TPTRGAN                  (0x1FU) /* Compare match/input capture from TPUn.TGRAn(n = 0to5) */
#define _20_AD_TRSC_TPTRG0AN                 (0x20U) /* Compare match/input capture from TPU0.TGRA0 */
#define _11_AD_TRSC_ELCTRGX0N                (0x11U) /* A/D Startup source 0 from ELC */
#define _12_AD_TRSC_ELCTRGX1N                (0x12U) /* A/D Startup source 1 from ELC */
#define _19_AD_TRSC_ELCTRGX01N               (0x19U) /* A/D Startup source 0, or
                                                        A/D Startup source 1 from ELC */
/* Group C Scan End Interrupt Enable (GCADIE) */
#define _00_AD_GCADI_DISABLE                 (0x00U) /* Group C scan end interrupt is disabled */
#define _40_AD_GCADI_ENABLE                  (0x40U) /* Group C scan end interrupt is enabled */
/* Group C A/D Conversion Operation Enable (GRCE) */
#define _00_AD_GROUPC_DISABLE                (0x00U) /* A/D conversion operation for group C is disabled */
#define _80_AD_GROUPC_ENABLE                 (0x80U) /* A/D conversion operation for group C is enabled */

/*
    A/D Sample-and-Hold Circuit Control Register (ADSHCR)
*/
/*  Channel-Dedicated Sample-and-Hold Circuit Bypass Select (SHANS) */
#define _0100_AD_DSH_AN000_SELECT            (0x0100U) /* AN000 use the channel-dedicated sample-and-hold circuits */
#define _0200_AD_DSH_AN001_SELECT            (0x0200U) /* AN001 use the channel-dedicated sample-and-hold circuits */
#define _0400_AD_DSH_AN002_SELECT            (0x0400U) /* AN002 use the channel-dedicated sample-and-hold circuits */

/*
    A/D Disconnection Detection Control Register (ADDISCR)
*/
#define _00_AD_DISCONECT_UNUSED              (0x00U)   /* Detection detection control unused */
#define _00_AD_DISCONECT_DISCHARGE           (0x00U)   /* Detection detection control discharge */
#define _10_AD_DISCONECT_PRECHARGE           (0x10U)   /* Detection detection control precharge */
#define _02_AD_DISCONECT_2ADCLK              (0x02U)   /* 2 ADCLK of detection detection control precharge/discharge */
#define _03_AD_DISCONECT_3ADCLK              (0x03U)   /* 3 ADCLK of detection detection control precharge/discharge */
#define _04_AD_DISCONECT_4ADCLK              (0x04U)   /* 4 ADCLK of detection detection control precharge/discharge */
#define _05_AD_DISCONECT_5ADCLK              (0x05U)   /* 5 ADCLK of detection detection control precharge/discharge */
#define _06_AD_DISCONECT_6ADCLK              (0x06U)   /* 6 ADCLK of detection detection control precharge/discharge */
#define _07_AD_DISCONECT_7ADCLK              (0x07U)   /* 7 ADCLK of detection detection control precharge/discharge */
#define _08_AD_DISCONECT_8ADCLK              (0x08U)   /* 8 ADCLK of detection detection control precharge/discharge */
#define _09_AD_DISCONECT_9ADCLK              (0x09U)   /* 9 ADCLK of detection detection control precharge/discharge */
#define _0A_AD_DISCONECT_10ADCLK             (0x0AU)   /* 10 ADCLK of detection detection control precharge/discharge */
#define _0B_AD_DISCONECT_11ADCLK             (0x0BU)   /* 11 ADCLK of detection detection control precharge/discharge */
#define _0C_AD_DISCONECT_12ADCLK             (0x0CU)   /* 12 ADCLK of detection detection control precharge/discharge */
#define _0D_AD_DISCONECT_13ADCLK             (0x0DU)   /* 13 ADCLK of detection detection control precharge/discharge */
#define _0E_AD_DISCONECT_14ADCLK             (0x0EU)   /* 14 ADCLK of detection detection control precharge/discharge */
#define _0F_AD_DISCONECT_15ADCLK             (0x0FU)   /* 15 ADCLK of detection detection control precharge/discharge */

/*
    A/D Group Scan Priority Control Register (ADGSPCR)
*/
/* Group Priority Control Setting (PGS) */
#define _0000_AD_GPPRIORITY_DISABLE          (0x0000U) /* Operation is without group priority control */
#define _0001_AD_GPPRIORITY_ENABLE           (0x0001U) /* Operation is with group priority control */
/* Low-Priority Group Restart Setting (GBRSCN) */
#define _0000_AD_GPRESTART_DISABLE           (0x0000U) /* Scanning for the group is not restarted after having been
                                                          interrupted due to group priority control */
#define _0002_AD_GPRESTART_ENABLE            (0x0002U) /* Scanning for the group is restarted after having been
                                                          interrupted due to group priority control */
/* Restart Channel Select (LGRRS) */
#define _0000_AD_RESTART_START_CHAN          (0x0000U) /* Scanning is restarted from the scan start channel */
#define _4000_AD_RESTART_INCOMPLETE          (0x4000U) /* Scanning is restarted from the channel on which A/D
                                                          conversion is not completed. */
/* Single Scan Continuous Start (GBRP) */
#define _0000_AD_SINGLE_START_NOT_CONT       (0x0000U) /* Single cycle scan for group B not continuously activated */
#define _8000_AD_SINGLE_START_CONT           (0x8000U) /* Single cycle scan for group B is continuously activated */

/*
    A/D Comparison Function Control Register (ADCMPCR)
*/
/* Window A/B Complex Conditions Setting (CMPAB) */
#define _0000_AD_COMPLEX_CONDITION_OR        (0x0000U) /* Window A comparison condition matched OR window B
                                                          comparison condition matched */
#define _0001_AD_COMPLEX_CONDITION_EXOR      (0x0001U) /* Window A comparison condition matched EXOR window B
                                                          comparison condition matched */
#define _0002_AD_COMPLEX_CONDITION_AND       (0x0002U) /* Window A comparison condition matched AND window B
                                                          comparison condition matched */
/* Comparison Window B Enable (CMPBE) */
#define _0000_AD_WINDOWB_DISABLE             (0x0000U) /* Comparison window B disabled */
#define _0200_AD_WINDOWB_ENABLE              (0x0200U) /* Comparison window B enabled */
/* Comparison Window A Enable (CMPAE) */
#define _0000_AD_WINDOWA_DISABLE             (0x0000U) /* Comparison window A disabled */
#define _0800_AD_WINDOWA_ENABLE              (0x0800U) /* Comparison window A enabled */
/* Comparison Window B Interrupt Enable (CMPBIE) */
#define _0000_AD_COMPAREB_INTERRUPT_DISABLE  (0x0000U) /* Comparison interrupt by a match with the comparison condition
                                                          (window B) is disabled */
#define _2000_AD_COMPAREB_INTERRUPT_ENABLE   (0x2000U) /* Comparison interrupt by a match with the comparison condition
                                                          (window B) is enable */
/* Window Function Setting (WCMPE) */
#define _0000_AD_WINDOWFUNCTION_DISABLE      (0x0000U) /* Window function disabled */
#define _4000_AD_WINDOWFUNCTION_ENABLE       (0x4000U) /* Window function enabled */
/* Comparison A Interrupt Enable (CMPAIE) */
#define _0000_AD_COMPAREA_INTERRUPT_DISABLE  (0x0000U) /* Comparison interrupt by a match with the comparison condition
                                                          (window A) is disabled */
#define _8000_AD_COMPAREA_INTERRUPT_ENABLE   (0x8000U) /* Comparison interrupt by a match with the comparison condition
                                                          (window A) is enable */

/*
    A/D Comparison Function Window A Extended Input Select Register (ADCMPANSER)
*/
/* Temperature Sensor Output Comparison Select (CMPSTS) */
#define _00_AD_TEMP_CMPA_UNUSED              (0x00U)   /* Temperature sensor output is not a target for comparison */
#define _01_AD_TEMP_CMPA_USED                (0x01U)   /* Temperature sensor output is a target for comparison */
/* Internal Reference Voltage Compare Select (CMPSOC) */
#define _00_AD_IREF_CMPA_UNUSED              (0x00U)   /* Internal reference voltage is not a target for comparison */
#define _02_AD_IREF_CMPA_USED                (0x02U)   /* Internal reference voltage is a target for comparison */

/*
    A/D Compare Channel Select Register 0 (ADCMPANSR0)
*/
#define _0001_AD_ANx00_CMPA_USED             (0x0001U) /* ANx00 is target for comparison */
#define _0002_AD_ANx01_CMPA_USED             (0x0002U) /* ANx01 is target for comparison */
#define _0004_AD_ANx02_CMPA_USED             (0x0004U) /* ANx02 is target for comparison */
#define _0008_AD_ANx03_CMPA_USED             (0x0008U) /* ANx03 is target for comparison */
#define _0010_AD_ANx04_CMPA_USED             (0x0010U) /* ANx04 is target for comparison */
#define _0020_AD_ANx05_CMPA_USED             (0x0020U) /* ANx05 is target for comparison */
#define _0040_AD_ANx06_CMPA_USED             (0x0040U) /* ANx06 is target for comparison */
#define _0080_AD_ANx07_CMPA_USED             (0x0080U) /* ANx07 is target for comparison */
#define _0100_AD_AN108_CMPA_USED             (0x0100U) /* AN108 is target for comparison */
#define _0200_AD_AN109_CMPA_USED             (0x0200U) /* AN109 is target for comparison */
#define _0400_AD_AN110_CMPA_USED             (0x0400U) /* AN110 is target for comparison */
#define _0800_AD_AN111_CMPA_USED             (0x0800U) /* AN111 is target for comparison */
#define _1000_AD_AN112_CMPA_USED             (0x1000U) /* AN112 is target for comparison */
#define _2000_AD_AN113_CMPA_USED             (0x2000U) /* AN113 is target for comparison */
#define _4000_AD_AN114_CMPA_USED             (0x4000U) /* AN114 is target for comparison */
#define _8000_AD_AN115_CMPA_USED             (0x8000U) /* AN115 is target for comparison */

/*
    A/D Compare Channel Select Register 1 (ADCMPANSR1)
*/
#define _0001_AD_AN116_CMPA_USED             (0x0001U) /* AN116 is target for comparison */
#define _0002_AD_AN117_CMPA_USED             (0x0002U) /* AN117 is target for comparison */
#define _0004_AD_AN118_CMPA_USED             (0x0004U) /* AN118 is target for comparison */
#define _0008_AD_AN119_CMPA_USED             (0x0008U) /* AN119 is target for comparison */
#define _0010_AD_AN120_CMPA_USED             (0x0010U) /* AN120 is target for comparison */

/*
    A/D Compare Level Register 0 (ADCMPLR0)
*/
#define _0000_AD_ANx00_CMPA_LEVEL0           (0x0000U) /* ANx00 converted value < ADCMPDR0 register value or ANx00
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx01_CMPA_LEVEL0           (0x0000U) /* ANx01 converted value < ADCMPDR0 register value or ANx01
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx02_CMPA_LEVEL0           (0x0000U) /* ANx02 converted value < ADCMPDR0 register value or ANx02
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx03_CMPA_LEVEL0           (0x0000U) /* ANx03 converted value < ADCMPDR0 register value or ANx03
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx04_CMPA_LEVEL0           (0x0000U) /* ANx04 converted value < ADCMPDR0 register value or ANx04
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx05_CMPA_LEVEL0           (0x0000U) /* ANx05 converted value < ADCMPDR0 register value or ANx05
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx06_CMPA_LEVEL0           (0x0000U) /* ANx06 converted value < ADCMPDR0 register value or ANx06
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_ANx07_CMPA_LEVEL0           (0x0000U) /* ANx07 converted value < ADCMPDR0 register value or ANx07
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN108_CMPA_LEVEL0           (0x0000U) /* AN108 converted value < ADCMPDR0 register value or AN108
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN109_CMPA_LEVEL0           (0x0000U) /* AN109 converted value < ADCMPDR0 register value or AN109
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN110_CMPA_LEVEL0           (0x0000U) /* AN110 converted value < ADCMPDR0 register value or AN110
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN111_CMPA_LEVEL0           (0x0000U) /* AN111 converted value < ADCMPDR0 register value or AN111
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN112_CMPA_LEVEL0           (0x0000U) /* AN112 converted value < ADCMPDR0 register value or AN112
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN113_CMPA_LEVEL0           (0x0000U) /* AN113 converted value < ADCMPDR0 register value or AN113
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN114_CMPA_LEVEL0           (0x0000U) /* AN114 converted value < ADCMPDR0 register value or AN114
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN115_CMPA_LEVEL0           (0x0000U) /* AN115 converted value < ADCMPDR0 register value or AN115
                                                          converted value > ADCMPDR1 register value */
#define _0001_AD_ANx00_CMPA_LEVEL1           (0x0001U) /* ADCMPDR0 register value < ANx00 converted value < ADCMPDR1
                                                          register value */
#define _0002_AD_ANx01_CMPA_LEVEL1           (0x0002U) /* ADCMPDR0 register value < ANx01 converted value < ADCMPDR1
                                                          register value */
#define _0004_AD_ANx02_CMPA_LEVEL1           (0x0004U) /* ADCMPDR0 register value < ANx02 converted value < ADCMPDR1
                                                          register value */
#define _0008_AD_ANx03_CMPA_LEVEL1           (0x0008U) /* ADCMPDR0 register value < ANx03 converted value < ADCMPDR1
                                                          register value */
#define _0010_AD_ANx04_CMPA_LEVEL1           (0x0010U) /* ADCMPDR0 register value < ANx04 converted value < ADCMPDR1
                                                          register value */
#define _0020_AD_ANx05_CMPA_LEVEL1           (0x0020U) /* ADCMPDR0 register value < ANx05 converted value < ADCMPDR1
                                                          register value */
#define _0040_AD_ANx06_CMPA_LEVEL1           (0x0040U) /* ADCMPDR0 register value < ANx06 converted value < ADCMPDR1
                                                          register value */
#define _0080_AD_ANx07_CMPA_LEVEL1           (0x0080U) /* ADCMPDR0 register value < ANx07 converted value < ADCMPDR1
                                                          register value */
#define _0100_AD_AN108_CMPA_LEVEL1           (0x0100U) /* ADCMPDR0 register value < AN108 converted value < ADCMPDR1
                                                          register value */
#define _0200_AD_AN109_CMPA_LEVEL1           (0x0200U) /* ADCMPDR0 register value < AN109 converted value < ADCMPDR1
                                                          register value */
#define _0400_AD_AN110_CMPA_LEVEL1           (0x0400U) /* ADCMPDR0 register value < AN110 converted value < ADCMPDR1
                                                          register value */
#define _0800_AD_AN111_CMPA_LEVEL1           (0x0800U) /* ADCMPDR0 register value < AN111 converted value < ADCMPDR1
                                                          register value */
#define _1000_AD_AN112_CMPA_LEVEL1           (0x1000U) /* ADCMPDR0 register value < AN112 converted value < ADCMPDR1
                                                          register value */
#define _2000_AD_AN113_CMPA_LEVEL1           (0x2000U) /* ADCMPDR0 register value < AN113 converted value < ADCMPDR1
                                                          register value */
#define _4000_AD_AN114_CMPA_LEVEL1           (0x4000U) /* ADCMPDR0 register value < AN114 converted value < ADCMPDR1
                                                          register value */
#define _8000_AD_AN115_CMPA_LEVEL1           (0x8000U) /* ADCMPDR0 register value < AN115 converted value < ADCMPDR1
                                                          register value */

/*
    A/D Compare Level Register 1 (ADCMPLR1)
*/
#define _0000_AD_AN116_CMPA_LEVEL0           (0x0000U) /* AN116 converted value < ADCMPDR0 register value or AN116
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN117_CMPA_LEVEL0           (0x0000U) /* AN117 converted value < ADCMPDR0 register value or AN117
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN118_CMPA_LEVEL0           (0x0000U) /* AN118 converted value < ADCMPDR0 register value or AN118
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN119_CMPA_LEVEL0           (0x0000U) /* AN119 converted value < ADCMPDR0 register value or AN119
                                                          converted value > ADCMPDR1 register value */
#define _0000_AD_AN120_CMPA_LEVEL0           (0x0000U) /* AN120 converted value < ADCMPDR0 register value or AN120
                                                          converted value > ADCMPDR1 register value */
#define _0001_AD_AN116_CMPA_LEVEL1           (0x0001U) /* ADCMPDR0 register value < AN116 converted value < ADCMPDR1
                                                          register value */
#define _0002_AD_AN117_CMPA_LEVEL1           (0x0002U) /* ADCMPDR0 register value < AN117 converted value < ADCMPDR1
                                                          register value */
#define _0004_AD_AN118_CMPA_LEVEL1           (0x0004U) /* ADCMPDR0 register value < AN118 converted value < ADCMPDR1
                                                          register value */
#define _0008_AD_AN119_CMPA_LEVEL1           (0x0008U) /* ADCMPDR0 register value < AN119 converted value < ADCMPDR1
                                                          register value */
#define _0010_AD_AN120_CMPA_LEVEL1           (0x0010U) /* ADCMPDR0 register value < AN120 converted value < ADCMPDR1
                                                          register value */

/*
    A/D Comparison Function Window A Extended Input Comparison Condition Setting Register (ADCMPLER)
*/
/* Comparison Window A Temperature Sensor Output Comparison Condition Select (CMPLTS) */
#define _00_AD_TEMP_CMPA_LEVEL0              (0x00U)   /* Temperature converted value < ADCMPDR0 register value or
                                                          Temperature converted value > ADCMPDR1 register value */
#define _01_AD_TEMP_CMPA_LEVEL1              (0x01U)   /* ADCMPDR0 register value < Temperature converted value < ADCMPDR1
                                                          register value */
/* Comparison Window A Internal Reference Voltage Comparison Condition Select (CMPLOC) */
#define _00_AD_IREF_CMPA_LEVEL0              (0x00U)   /* Internal converted value < ADCMPDR0 register value or
                                                          Internal converted value > ADCMPDR1 register value */
#define _02_AD_IREF_CMPA_LEVEL1              (0x02U)   /* ADCMPDR0 register value < Internal converted value < ADCMPDR1
                                                          register value */

/*
    A/D Comparison Function Window B Channel Select Register (ADCMPBNSR)
*/
/* Comparison Window B Channel Select (CMPLB) */
#define _00_AD_CMPB_LEVEL0                   (0x00U)   /* Comparison B converted value < ADCMPDR0 register value or
                                                          Comparison B converted value > ADCMPDR1 register value */
#define _80_AD_CMPB_LEVEL1                   (0x80U)   /* ADWINLLB register value < Comparison B converted value <
                                                          ADWINULB register value */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_AD_PRIORITY_LEVEL0               (0x00U) /* Level 0 (interrupt disabled) */
#define _01_AD_PRIORITY_LEVEL1               (0x01U) /* Level 1 */
#define _02_AD_PRIORITY_LEVEL2               (0x02U) /* Level 2 */
#define _03_AD_PRIORITY_LEVEL3               (0x03U) /* Level 3 */
#define _04_AD_PRIORITY_LEVEL4               (0x04U) /* Level 4 */
#define _05_AD_PRIORITY_LEVEL5               (0x05U) /* Level 5 */
#define _06_AD_PRIORITY_LEVEL6               (0x06U) /* Level 6 */
#define _07_AD_PRIORITY_LEVEL7               (0x07U) /* Level 7 */
#define _08_AD_PRIORITY_LEVEL8               (0x08U) /* Level 8 */
#define _09_AD_PRIORITY_LEVEL9               (0x09U) /* Level 9 */
#define _0A_AD_PRIORITY_LEVEL10              (0x0AU) /* Level 10 */
#define _0B_AD_PRIORITY_LEVEL11              (0x0BU) /* Level 11 */
#define _0C_AD_PRIORITY_LEVEL12              (0x0CU) /* Level 12 */
#define _0D_AD_PRIORITY_LEVEL13              (0x0DU) /* Level 13 */
#define _0E_AD_PRIORITY_LEVEL14              (0x0EU) /* Level 14 */
#define _0F_AD_PRIORITY_LEVEL15              (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
    ADCHANNEL0, ADCHANNEL1, ADCHANNEL2, ADCHANNEL3, ADCHANNEL4, ADCHANNEL5, ADCHANNEL6, ADCHANNEL7,
    ADCHANNEL8, ADCHANNEL9, ADCHANNEL10, ADCHANNEL11, ADCHANNEL12, ADCHANNEL13, ADCHANNEL14, ADCHANNEL15,
    ADCHANNEL16, ADCHANNEL17, ADCHANNEL18, ADCHANNEL19, ADCHANNEL20,
    ADSELFDIAGNOSIS, ADTEMPSENSOR, ADINTERREFVOLT, ADDATADUPLICATION, ADDATADUPLICATIONA, ADDATADUPLICATIONB
} ad_channel_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
