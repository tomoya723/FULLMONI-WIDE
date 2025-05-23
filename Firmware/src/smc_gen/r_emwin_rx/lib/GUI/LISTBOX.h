/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.32 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              Renesas Electronics Europe GmbH, Arcadiastrasse 10, 40472 Duesseldorf, Germany
Licensed SEGGER software: emWin
License number:           GUI-00678
License model:            License and Service Agreement, signed December 16th, 2016, Amendment No. 1 signed May 16th, 2019 and Amendment No. 2, signed September 20th, 2021 by Carsten Jauch, Managing Director
License valid for:        RX (based on RX-V1, RX-V2 or RX-V3)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2016-12-22 - 2023-12-31
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LISTBOX.h
Purpose     : LISTBOX widget include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef LISTBOX_H
#define LISTBOX_H

#include "WM.h"
#include "WIDGET.h"             /* Req. for WIDGET_DRAW_ITEM_FUNC */
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       defines
*
**********************************************************************
*/

#define LISTBOX_ALL_ITEMS  -1

/*********************************************************************
*
*       LISTBOX color indexes
*
*  Description
*    Color indexes used by the LISTBOX widget.
*/
#define LISTBOX_CI_UNSEL    0     // Color of unselected element.
#define LISTBOX_CI_SEL      1     // Color of selected element.
#define LISTBOX_CI_SELFOCUS 2     // Color of selected element with focus.
#define LISTBOX_CI_DISABLED 3     // Color of disabled element.

/************************************************************
*
*       States
*/
typedef WM_HMEM LISTBOX_Handle;

/*********************************************************************
*
*       LISTBOX Notification codes
*
* The following is the list of notification codes specific to this widget,
* Send with the WM_NOTIFY_PARENT message
*/
#define LISTBOX_NOTIFICATION_LOST_FOCUS    (WM_NOTIFICATION_WIDGET + 0)

/************************************************************
*
*       Create / Status flags
*/
#define LISTBOX_CF_AUTOSCROLLBAR_H   (1 << 0)
#define LISTBOX_CF_AUTOSCROLLBAR_V   (1 << 1)
#define LISTBOX_CF_MULTISEL          (1 << 2)
#define LISTBOX_CF_WRAP              (1 << 3)
#define LISTBOX_CF_FIXEDSCROLLMODE   (1 << 4)
#define LISTBOX_CF_MOTION_H          (1 << 5)
#define LISTBOX_CF_MOTION_V          (1 << 6)
#define LISTBOX_SF_AUTOSCROLLBAR_H   LISTBOX_CF_AUTOSCROLLBAR_H
#define LISTBOX_SF_AUTOSCROLLBAR_V   LISTBOX_CF_AUTOSCROLLBAR_V
#define LISTBOX_SF_MULTISEL          LISTBOX_CF_MULTISEL
#define LISTBOX_SF_WRAP              LISTBOX_CF_WRAP
#define LISTBOX_SF_FIXEDSCROLLMODE   LISTBOX_CF_FIXEDSCROLLMODE
#define LISTBOX_SF_MOTION_H          LISTBOX_CF_MOTION_H
#define LISTBOX_SF_MOTION_V          LISTBOX_CF_MOTION_V

/************************************************************
*
*       LISTBOX fixed scroll mode flags
*
*  Description
*    Defines used for the fixed scroll mode of the widget.
*    Refer to LISTBOX_SetFixedScrollPos() for more information.
*/
#define LISTBOX_FM_OFF               0       // Disables the fixed scroll mode.
#define LISTBOX_FM_ON                1       // Enables the fixed scroll mode.
#define LISTBOX_FM_CENTER            2       // Tries to keep the selected item in the center.

/*********************************************************************
*
*       Create functions
*
**********************************************************************
*/

LISTBOX_Handle LISTBOX_Create        (const GUI_ConstString * ppText, int x0, int y0, int xSize, int ySize, int Flags);
LISTBOX_Handle LISTBOX_CreateAsChild (const GUI_ConstString * ppText, WM_HWIN hWinParent, int x0, int y0, int xSize, int ySize, int Flags);
LISTBOX_Handle LISTBOX_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const GUI_ConstString * ppText);
LISTBOX_Handle LISTBOX_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const GUI_ConstString * ppText, int NumExtraBytes);
LISTBOX_Handle LISTBOX_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void LISTBOX_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/

int              LISTBOX_AddKey                (LISTBOX_Handle hObj, int Key);
void             LISTBOX_AddString             (LISTBOX_Handle hObj, const char * s);
void             LISTBOX_AddStringH            (LISTBOX_Handle hObj, WM_HMEM hString); /* Not to be documented!!! */
void             LISTBOX_Clear                 (LISTBOX_Handle hObj);
void             LISTBOX_DecSel                (LISTBOX_Handle hObj);
void             LISTBOX_DeleteItem            (LISTBOX_Handle hObj, unsigned Index);
void             LISTBOX_EnableMotion          (LISTBOX_Handle hObj, int Flags);
void             LISTBOX_EnableWrapMode        (LISTBOX_Handle hObj, int OnOff);
GUI_COLOR        LISTBOX_GetBkColor            (LISTBOX_Handle hObj, unsigned Index);
U16              LISTBOX_GetFixedScrollPos     (LISTBOX_Handle hObj);
const GUI_FONT * LISTBOX_GetFont               (LISTBOX_Handle hObj);
unsigned         LISTBOX_GetItemSpacing        (LISTBOX_Handle hObj);
unsigned         LISTBOX_GetNumItems           (LISTBOX_Handle hObj);
U32              LISTBOX_GetVisItemIndices     (LISTBOX_Handle hObj, int * pFirst, int * pLast);
int              LISTBOX_GetSel                (LISTBOX_Handle hObj);
GUI_COLOR        LISTBOX_GetTextColor          (LISTBOX_Handle hObj, unsigned Index);
int              LISTBOX_GetItemDisabled       (LISTBOX_Handle hObj, unsigned Index);
int              LISTBOX_GetItemSel            (LISTBOX_Handle hObj, unsigned Index);
void             LISTBOX_GetItemText           (LISTBOX_Handle hObj, unsigned Index, char * pBuffer, int MaxSize);
int              LISTBOX_GetMulti              (LISTBOX_Handle hObj);
WM_HWIN          LISTBOX_GetOwner              (LISTBOX_Handle hObj);
int              LISTBOX_GetScrollStepH        (LISTBOX_Handle hObj);
int              LISTBOX_GetTextAlign          (LISTBOX_Handle hObj);
int              LISTBOX_GetUserData           (LISTBOX_Handle hObj, void * pDest, int NumBytes);
void             LISTBOX_IncSel                (LISTBOX_Handle hObj);
void             LISTBOX_InsertString          (LISTBOX_Handle hObj, const char * s, unsigned Index);
void             LISTBOX_InvalidateItem        (LISTBOX_Handle hObj, int Index);
U32              LISTBOX_IsItemPartiallyVisible(LISTBOX_Handle hObj, int Index);
int              LISTBOX_OwnerDraw             (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
void             LISTBOX_SetAutoScrollH        (LISTBOX_Handle hObj, int OnOff);
void             LISTBOX_SetAutoScrollV        (LISTBOX_Handle hObj, int OnOff);
void             LISTBOX_SetBkColor            (LISTBOX_Handle hObj, unsigned Index, GUI_COLOR color);
void             LISTBOX_SetFixedScrollPos     (LISTBOX_Handle hObj, U16 FixedScrollPos, U8 Mode);
void             LISTBOX_SetFont               (LISTBOX_Handle hObj, const GUI_FONT * pFont);
void             LISTBOX_SetItemDisabled       (LISTBOX_Handle hObj, unsigned Index, int OnOff);
void             LISTBOX_SetItemSel            (LISTBOX_Handle hObj, unsigned Index, int OnOff);
void             LISTBOX_SetItemSpacing        (LISTBOX_Handle hObj, unsigned Value);
void             LISTBOX_SetMulti              (LISTBOX_Handle hObj, int Mode);
void             LISTBOX_SetOwner              (LISTBOX_Handle hObj, WM_HWIN hOwner);
void             LISTBOX_SetOwnerDraw          (LISTBOX_Handle hObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void             LISTBOX_SetScrollStepH        (LISTBOX_Handle hObj, int Value);
void             LISTBOX_SetSel                (LISTBOX_Handle hObj, int Sel);
void             LISTBOX_SetScrollbarColor     (LISTBOX_Handle hObj, unsigned Index, GUI_COLOR Color);
void             LISTBOX_SetScrollbarWidth     (LISTBOX_Handle hObj, unsigned Width);
void             LISTBOX_SetString             (LISTBOX_Handle hObj, const char * s, unsigned Index);
void             LISTBOX_SetText               (LISTBOX_Handle hObj, const GUI_ConstString * ppText);
void             LISTBOX_SetTextAlign          (LISTBOX_Handle hObj, int Align);
GUI_COLOR        LISTBOX_SetTextColor          (LISTBOX_Handle hObj, unsigned Index, GUI_COLOR Color);
int              LISTBOX_SetUserData           (LISTBOX_Handle hObj, const void * pSrc, int NumBytes);
int              LISTBOX_UpdateScrollers       (LISTBOX_Handle hObj);

/*********************************************************************
*
*       Managing default values
*
**********************************************************************
*/

const GUI_FONT * LISTBOX_GetDefaultFont(void);
int              LISTBOX_GetDefaultScrollStepH (void);
GUI_COLOR        LISTBOX_GetDefaultBkColor     (unsigned Index);
int              LISTBOX_GetDefaultTextAlign   (void);
GUI_COLOR        LISTBOX_GetDefaultTextColor   (unsigned Index);
void             LISTBOX_SetDefaultFont        (const GUI_FONT * pFont);
void             LISTBOX_SetDefaultScrollStepH (int Value);
void             LISTBOX_SetDefaultBkColor     (unsigned Index, GUI_COLOR Color);
void             LISTBOX_SetDefaultTextAlign   (int Align);
void             LISTBOX_SetDefaultTextColor   (unsigned Index, GUI_COLOR Color);

/*********************************************************************
*
*       Compatibility to older versions
*
**********************************************************************
*/

#define LISTBOX_SetBackColor(hObj, Index, Color) LISTBOX_SetBkColor(hObj, Index, Color)
#define LISTBOX_DeleteString  LISTBOX_DeleteItem

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // LISTBOX_H

/*************************** End of file ****************************/
