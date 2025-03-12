/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_can_rx_if.h
* Description  : The RX CAN FIT API.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.20.2015 2.01    For 64M, 71M. Originates from RX63N.
*         : 30.10.2015 2.02    FIT update, new R_CAN_Create interface.
*         : 3.3.2016   2.10    65N added. No significant changes.
*         : 1.30.2017  2.11    - const was added to CAN_port_map_t so that there is no need to add const
*                                to instantiations.
*                              - Replaced 0 with NULL for unused pointers to
*                                port config. registers. (p_CAN0_RX_PIN_MPC etc.)
*                              - Test ran with 65N 2MB.
*                              - Some GSCE coding guidelines implemented. Mulitple lines changed. (Plugin was used.)
*         : 8.14.2017  2.12    - RX65N 2MB MP release.
*         : 27.04.2018 2.13WS  - Added RX66T. Fixed faulty CANn RX port configuration #if statements
*         : 26.10.2018 2.13    - Added support for additional RX/TX ports on RX66T
*         : 08.01.2019 2.15    - Added RX72T
*         : 05.04.2019 3.00    - Added support for GCC and IAR compilers
*         : 30.04.2019 3.10    - Added RX72M
*         : 16.09.2019 3.11    - Added message to warn issue of CAN0, CAN1 and CAN2 interrupt sources
*                                are not assigned any interrupt vector number
*         : 30.12.2019 3.20    - Added support RX66N, RX72N.
*         : 13.06.2020 4.00    - Added support CAN FIFO.
*         :                    - Added support Pin-setting.
*         :                    - Removed definitions port pin.
*         :                    - Removed define registers and information to fill the CAN pin map.
*         : 04.01.2021 4.10    - Changed can_tx_callback to can_txf_callback in case txf_cb_func
*                                is NULL in R_CAN_Create().
*                              - Added support demo for CAN v4.10.
*         : 01.04.2021 5.00    - Added support for setting different bitrate for different channels.
*         : 07.04.2021 5.10    - Changed sid to id in R_CAN_RxSet().
*                              - Added support RX671.
*         : 13.09.2021 5.20    - Added the demo for RX671.
*         : 21.02.2022 5.21    - Updated minor version.
*         : 28.06.2022 5.30    - Updated demo projects.
*         : 20.09.2022 5.40    - Updated demo projects.
*         : 08.09.2023 5.50    - Updated according to GSCE Code Checker 6.50.
*                              - Updated demo projects to support FIFO callback.
*                              - Added demo for RX72N.
*                              - Added WAIT_LOOP comments.
***********************************************************************************************************************/
#ifndef CAN_INTERFACE_HEADER_FILE
#define CAN_INTERFACE_HEADER_FILE 

/***********************************************************************************************************************
Includes   <System Includes>, "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "r_can_rx_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

#if R_BSP_VERSION_MAJOR < 5
    #error "This module must use BSP module of Rev.5.00 or higher. Please use the BSP module of Rev.5.00 or higher."
#endif

/* Version Number of API. */
#define RCAN_RX_VERSION_MAJOR           (5)
#define RCAN_RX_VERSION_MINOR           (50)
/* The process of getting the version number is done through the macro below. The version number is encoded where the
   top 2 bytes are the major version number and the bottom 2 bytes are the minor version number. For example,
   Version 4.25 would be returned as 0x00040019. */
#define R_CAN_GetVersion()  ((((uint32_t)RCAN_RX_VERSION_MAJOR) << 16) | (uint32_t)RCAN_RX_VERSION_MINOR)



#if (CAN_USE_CAN1 == 1)
    #if ((BSP_MCU_RX66T == 1) || (BSP_MCU_RX72T == 1))
        #error " - CAN1  does not exist on this device! -"
    #endif
#endif

#if (CAN_USE_CAN2 == 1)
    #if ((BSP_MCU_RX65N == 1) || (BSP_MCU_RX66T == 1) || (BSP_MCU_RX671 == 1) || (BSP_MCU_RX72T == 1))
        #error " - CAN2 does not exist on this device! -"
    #endif
#endif

#if ((CAN_USE_CAN0 == 1)&&((VECT(CAN0,TXM0) == 0)||(VECT(CAN0,RXM0) == 0)))
        #error "Interrupt vector number of CAN0 should be defined in file r_bsp_interrupt_config.h."
#endif

#ifdef CAN1
    #if ((CAN_USE_CAN1 == 1)&&((VECT(CAN1,TXM1) == 0)||(VECT(CAN1,RXM1) == 0)))
        #error "Interrupt vector number of CAN1 should be defined in file r_bsp_interrupt_config.h."
    #endif
#endif

#ifdef CAN2
    #if ((CAN_USE_CAN2 == 1)&&((VECT(CAN2,TXM2) == 0)||(VECT(CAN2,RXM2) == 0)))
        #error "Interrupt vector number of CAN2 should be defined in file r_bsp_interrupt_config.h."
    #endif
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
/*** CAN API ACTION TYPES ***/
#define DISABLE                         (0)
#define ENABLE                          (1)

/* Periph CAN modes */
#define EXITSLEEP_CANMODE               (2)
#define ENTERSLEEP_CANMODE              (3)
#define RESET_CANMODE                   (4)
#define HALT_CANMODE                    (5)
#define OPERATE_CANMODE                 (6)

/* Port mode actions */
#define CANPORT_TEST_LISTEN_ONLY        (7)
#define CANPORT_TEST_0_EXT_LOOPBACK     (8)
#define CANPORT_TEST_1_INT_LOOPBACK     (9)
#define CANPORT_RETURN_TO_NORMAL        (10)

/* Local sleep mode for CAN module */
#define CAN_NOT_SLEEP   (0)
#define CAN_SLEEP       (1)

/* Tranceiver port pin macros.  */
#define CAN_TRX_DDR(x, y)               (CAN_TRX_DDR_PREPROC(x, y))
#define CAN_TRX_DDR_PREPROC(x, y)       ((PORT ## x .PDR.BIT.B ## y))
#define CAN_TRX_DR(x, y)                (CAN_TRX_DR_PREPROC(x, y))
#define CAN_TRX_DR_PREPROC(x, y)        ((PORT ## x .PODR.BIT.B ## y))

/*** CAN API return values *****************************/
#define     R_CAN_OK                ((uint32_t)0x00000000)
#define     R_CAN_NOT_OK            ((uint32_t)0x00000001)
#define     R_CAN_MODULE_STOP_ERR   ((uint32_t)0x00000002)
#define     R_CAN_MSGLOST           ((uint32_t)0x00000004)
#define     R_CAN_NO_SENTDATA       ((uint32_t)0x00000008)
#define     R_CAN_RXPOLL_TMO        ((uint32_t)0x00000010)    /* 16 */
#define     R_CAN_BAD_CH_NR         ((uint32_t)0x00000020)    /* 32 */
#define     R_CAN_SW_BAD_MBX        ((uint32_t)0x00000040)    /* 64 */
#define     R_CAN_BAD_ACTION_TYPE   ((uint32_t)0x00000080)    /* 128 */
#define     CAN_ERR_NOT_FIFO_MODE   ((uint32_t)0x00010000)
#define     CAN_ERR_BOX_FULL        ((uint32_t)0x00020000)
#define     CAN_ERR_BOX_EMPTY       ((uint32_t)0x00040000)
#define     R_CAN_SW_BAD_MODE       ((uint32_t)0x00080000)


/* CAN peripheral timeout reasons. */
#define     R_CAN_SW_WAKEUP_ERR     ((uint32_t)0x00000100)    /* 256 */
#define     R_CAN_SW_SLEEP_ERR      ((uint32_t)0x00000200)    /* 512 */
#define     R_CAN_SW_HALT_ERR       ((uint32_t)0x00000400)    /* 1024 */
#define     R_CAN_SW_RST_ERR        ((uint32_t)0x00000800)    /* 2048 */
#define     R_CAN_SW_TSRC_ERR       ((uint32_t)0x00001000)    /* 4096 */
#define     R_CAN_SW_SET_TX_TMO     ((uint32_t)0x00002000)    /* 8192 */
#define     R_CAN_SW_SET_RX_TMO     ((uint32_t)0x00004000)    /* 16384 */
#define     R_CAN_SW_ABORT_ERR      ((uint32_t)0x00008000)    /* 32768 */

/* CAN STATE CODES */
#define     R_CAN_STATUS_INIT           ((uint32_t)0x0000000)
#define     R_CAN_STATUS_ERROR_ACTIVE   ((uint32_t)0x0000001)
#define     R_CAN_STATUS_ERROR_PASSIVE  ((uint32_t)0x0000002)
#define     R_CAN_STATUS_BUSOFF         ((uint32_t)0x0000004)
/*** API end *********************************************/

/* CAN Frame ID modes */
#define     STD_ID_MODE     (0)
#define     EXT_ID_MODE     (1)
#define     MIXED_ID_MODE   (2)

/* Mailbox search modes. */
#define     RX_SEARCH       (0)
#define     TX_SEARCH       (1)
#define     MSGLOST_SEARCH  (2)
#define     CHANNEL_SEARCH  (3)

/* CAN1 Control Register (CTLR) b9, b8 CANM[1:0] CAN Operating Mode Select. */
#define CAN_OPERATION       (0)    /* CAN operation mode */
#define CAN_RESET           (1)    /* CAN reset mode */
#define CAN_HALT            (2)    /* CAN halt mode */
#define CAN_RESET_FORCE     (3)    /* CAN reset mode (forcible transition) */

/* Frame types */
#define DATA_FRAME          (0)
#define REMOTE_FRAME        (1)

/* Mailbox mode */
#define NORMAL_MAILBOX_MODE      (0)
#define FIFO_MAILBOX_MODE        (1)

/* Bit set defines */
#define        MBX_0        (0x00000001)
#define        MBX_1        (0x00000002)
#define        MBX_2        (0x00000004)
#define        MBX_3        (0x00000008)
#define        MBX_4        (0x00000010)
#define        MBX_5        (0x00000020)
#define        MBX_6        (0x00000040)
#define        MBX_7        (0x00000080)
#define        MBX_8        (0x00000100)
#define        MBX_9        (0x00000200)
#define        MBX_10       (0x00000400)
#define        MBX_11       (0x00000800)
#define        MBX_12       (0x00001000)
#define        MBX_13       (0x00002000)
#define        MBX_14       (0x00004000)
#define        MBX_15       (0x00008000)
#define        MBX_16       (0x00010000)
#define        MBX_17       (0x00020000)
#define        MBX_18       (0x00040000)
#define        MBX_19       (0x00080000)
#define        MBX_20       (0x00100000)
#define        MBX_21       (0x00200000)
#define        MBX_22       (0x00400000)
#define        MBX_23       (0x00800000)
#define        MBX_24       (0x01000000)
#define        MBX_25       (0x02000000)
#define        MBX_26       (0x04000000)
#define        MBX_27       (0x08000000)
#define        MBX_28       (0x10000000)
#define        MBX_29       (0x20000000)
#define        MBX_30       (0x40000000)
#define        MBX_31       (0x80000000)

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Standard data frame message definition object. */
typedef struct
{
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} can_frame_t;

/* BITRATE configuration object */
typedef struct
{
    uint8_t BRP;
    uint8_t TSEG1;
    uint8_t TSEG2;
    uint8_t SJW;
} can_bitrate_config_t;

/* A pointer to the CAN peripheral registers memory map structure. */
typedef volatile struct st_can R_BSP_EVENACCESS_SFR * can_st_ptr;

/**************************************************************************************************
Exported global variables
***************************************************************************************************/
/**************************************************************************************************
Exported global functions (to be accessed by other files)
***************************************************************************************************/
/*****************************************************************
                R X   C A N   A P I 
******************************************************************/
/* INITIALIZATION */
/******************************************************************************
 * Function Name: R_CAN_Create
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : p_cfg
 *              : (*tx_cb_func)(void)
 *              : (*txf_cb_func)(void)
 *              : (*rx_cb_func)(void)
 *              : (*rxf_cb_func)(void)
 *              : (*err_cb_func)(void)
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_Create (const uint32_t  ch_nr,
                        const uint32_t mb_mode,
                        const can_bitrate_config_t p_cfg,
                        void (*tx_cb_func)(void),
                        void (*txf_cb_func)(void),
                        void (*rx_cb_func)(void),
                        void (*rxf_cb_func)(void),
                        void (*err_cb_func)(void));

/******************************************************************************
 * Function Name: R_CAN_PortSet
 * Description  : .
 * Arguments    : ch_nr
 *              : action_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_PortSet (const uint32_t ch_nr, const uint32_t action_type);

/******************************************************************************
 * Function Name: R_CAN_Control
 * Description  : .
 * Arguments    : ch_nr
 *              : action_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_Control (const uint32_t ch_nr, const uint32_t action_type);

/******************************************************************************
 * Function Name: R_CAN_SetBitrate
 * Description  : .
 * Arguments    : ch_nr
 *              : p_cfg
 * Return Value : .
 *****************************************************************************/
void        R_CAN_SetBitrate (const uint32_t ch_nr, const can_bitrate_config_t p_cfg);

/* TRANSMIT */
/******************************************************************************
 * Function Name: R_CAN_TxSet
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 *              : *frame_p
 *              : frame_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_TxSet (const uint32_t ch_nr,
                        const uint32_t mb_mode,
                        const uint32_t mbox_nr,
                        const can_frame_t * frame_p,
                        const uint32_t frame_type);

/******************************************************************************
 * Function Name: R_CAN_TxSetXid
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 *              : *frame_p
 *              : frame_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_TxSetXid (const uint32_t ch_nr,
                        const uint32_t mb_mode,
                        const uint32_t mbox_nr,
                        can_frame_t * frame_p,
                        const uint32_t frame_type);

/******************************************************************************
 * Function Name: R_CAN_Tx
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_Tx (const uint32_t ch_nr, const uint32_t mb_mode, const uint32_t mbox_nr);

/******************************************************************************
 * Function Name: R_CAN_TxCheck
 * Description  : .
 * Arguments    : ch_nr
 *              : mbox_nr
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_TxCheck (const uint32_t ch_nr, const uint32_t mbox_nr);

/******************************************************************************
 * Function Name: R_CAN_TxStopMsg
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_TxStopMsg (const uint32_t ch_nr, const uint32_t  mb_mode, const uint32_t mbox_nr);

/* RECEIVE */
/******************************************************************************
 * Function Name: R_CAN_RxSet
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : id
 *              : frame_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxSet (const uint32_t ch_nr, const uint32_t mbox_nr, const uint32_t id, const uint32_t frame_type);

/******************************************************************************
 * Function Name: R_CAN_RxSetXid
 * Description  : .
 * Arguments    : ch_nr
 *              : mbox_nr
 *              : xid
 *              : frame_type
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxSetXid (const uint32_t ch_nr, const uint32_t mbox_nr, uint32_t xid, const uint32_t frame_type);

/******************************************************************************
 * Function Name: R_CAN_RxPoll
 * Description  : .
 * Arguments    : ch_nr
 *              : mbox_nr
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxPoll (const uint32_t ch_nr, const uint32_t mbox_nr);

/******************************************************************************
 * Function Name: R_CAN_RxRead
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 *              : *frame_p
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxRead (const uint32_t ch_nr,
                        const uint32_t mb_mode,
                        const uint32_t mbox_nr,
                        can_frame_t * const frame_p);

/******************************************************************************
 * Function Name: R_CAN_RxSetMask
 * Description  : .
 * Arguments    : ch_nr
 *              : mbox_nr
 *              : mask_value
 * Return Value : .
 *****************************************************************************/
void        R_CAN_RxSetMask (const uint32_t ch_nr, const uint32_t mbox_nr, const uint32_t mask_value);

/******************************************************************************
 * Function Name: R_CAN_RxSetFIFO
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 *              : fidcr0_value
 *              : fidcr1_value
 *              : frame_type
 *              : mkr6_value
 *              : mkr7_value
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxSetFIFO (const uint32_t  ch_nr,
                            const uint32_t mb_mode,
                            const uint32_t  mbox_nr,
                            const uint32_t  fidcr0_value,
                            const uint32_t  fidcr1_value,
                            const uint32_t  frame_type,
                            const uint32_t  mkr6_value,
                            const uint32_t  mkr7_value);

/******************************************************************************
 * Function Name: R_CAN_RxSetFIFOXid
 * Description  : .
 * Arguments    : ch_nr
 *              : mb_mode
 *              : mbox_nr
 *              : xfidcr0_value
 *              : xfidcr1_value
 *              : frame_type
 *              : mkr6_value
 *              : mkr7_value
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_RxSetFIFOXid (const uint32_t  ch_nr,
                                const uint32_t mb_mode,
                                const uint32_t  mbox_nr,
                                const uint32_t  xfidcr0_value,
                                const uint32_t  xfidcr1_value,
                                const uint32_t  frame_type,
                                const uint32_t  mkr6_value,
                                const uint32_t  mkr7_value);

/* ERRORS */
/******************************************************************************
 * Function Name: R_CAN_CheckErr
 * Description  : .
 * Arguments    : ch_nr
 * Return Value : .
 *****************************************************************************/
uint32_t    R_CAN_CheckErr (const uint32_t ch_nr);

#endif  /* CAN_INTERFACE_HEADER_FILE */

/* file end */
