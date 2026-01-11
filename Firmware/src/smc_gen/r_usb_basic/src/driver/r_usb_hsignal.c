/*
* Copyright (c) 2011 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : r_usb_hsignal.c
 * Version      : 1.44
 * Description  : Host USB signalling
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 *         : 16.11.2018 1.24 Supporting RTOS Thread safe
 *         : 01.03.2020 1.30 RX72N/RX66N is added and uITRON is supported.
 *         : 01.03.2025 1.44 Change Disclaimer.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/******************************************************************************
 Renesas Abstracted Host Signal functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_vbus_control
 Description     : USB VBUS ON/OFF setting.
 Arguments       : usb_utr_t    *ptr    : Pointer to usb_utr_t structure.
                 : uint16_t     command : ON / OFF.
 Return value    : none
 ******************************************************************************/
void usb_hstd_vbus_control (usb_utr_t *ptr, uint16_t command)
{
    if (USB_VBON == command)
    {
        hw_usb_set_vbout(ptr);
#if USB_CFG_BC == USB_CFG_ENABLE
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
        if (USB_IP1 == ptr->ip)
        {
            usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_VB](ptr);
        }

#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */
    }
    else
    {
        hw_usb_clear_vbout(ptr);
    }
}
/******************************************************************************
 End of function usb_hstd_vbus_control
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_suspend_process
 Description     : Set USB registers as required when USB Device status is moved
                 : to "Suspend".  
 Arguments       : usb_utr_t    *ptr    : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void usb_hstd_suspend_process (usb_utr_t *ptr)
{
    /* SUSPENDED check */
    if (USB_SUSPENDED == g_usb_hstd_remort_port[ptr->ip])
    {
        /* SOF OFF */
        hw_usb_hclear_uact(ptr);

        /* Wait */
        usb_cpu_delay_xms((uint16_t) 1);
        usb_hstd_chk_sof(ptr);

        /* RWUPE=1, UACT=0 */
        hw_usb_hset_rwupe(ptr);

        /* Enable port BCHG interrupt */
        usb_hstd_bchg_enable(ptr);

        /* Wait */
        usb_cpu_delay_xms((uint16_t) 5);
    }
    else
    {
        /* SOF OFF */
        hw_usb_hclear_uact(ptr);

        /* Wait */
        usb_cpu_delay_xms((uint16_t) 5);
    }
}
/******************************************************************************
 End of function usb_hstd_suspend_process
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_attach
 Description     : Set USB registers as required when USB device is attached, 
                 : and notify MGR (manager) task that attach event occurred.
 Arguments       : usb_utr_t    *ptr    : Pointer to usb_utr_t structure.
                 : uint16_t     result  : Result.
                 : uint16_t     port    : Port number.
 Return value    : none
 ******************************************************************************/
void usb_hstd_attach (usb_utr_t *ptr, uint16_t result)
{
    /* DTCH  interrupt enable */
    usb_hstd_dtch_enable(ptr);

    /* Interrupt Enable */
    usb_hstd_berne_enable(ptr);

    /* USB Mng API */
    usb_hstd_notif_ator_detach(ptr, result);
#if USB_CFG_BC == USB_CFG_ENABLE
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    if (USB_IP1 == ptr->ip)
    {
        usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_AT](ptr);
    }

#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
#endif /* USB_CFG_BC == USB_CFG_ENABLE */
}
/******************************************************************************
 End of function usb_hstd_attach
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_hstd_detach
 Description     : Set USB register as required when USB device is detached, and 
                 : notify MGR (manager) task that detach occurred.
 Arguments       : usb_utr_t    *ptr    : Pointer to usb_utr_t structure.
                 : uint16_t     port    : Port number.
 Return value    : none
 ******************************************************************************/
void usb_hstd_detach (usb_utr_t *ptr)
{
#if USB_CFG_BC == USB_CFG_ENABLE
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    if (USB_IP1 == ptr->ip)
    {
        usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_DT](ptr);
    }

#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
#endif /* USB_CFG_BC == USB_CFG_ENABLE */

    /* DVSTCTR clear */
    hw_usb_clear_dvstctr(ptr, (uint16_t) (USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT));

    /* ATTCH interrupt enable */
    usb_hstd_attch_enable(ptr);

    /* USB Mng API */
    usb_hstd_notif_ator_detach(ptr, (uint16_t) USB_DETACH);
}
/******************************************************************************
 End of function usb_hstd_detach
 ******************************************************************************/
#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 End  Of File
 ******************************************************************************/
