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
File        : DROPDOWN_Private.h
Purpose     : DROPDOWN private header file
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef DROPDOWN_PRIVATE_H
#define DROPDOWN_PRIVATE_H

#include "WM_Intern.h"
#include "DROPDOWN.h"
#include "WIDGET.h"
#include "GUI_ARRAY.h"
#include "SCROLLER_Private.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define DROPDOWN_SF_AUTOSCROLLBAR DROPDOWN_CF_AUTOSCROLLBAR
#define DROPDOWN_SF_MOTION        DROPDOWN_CF_MOTION

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} DROPDOWN_SKIN_PRIVATE;

typedef struct {
  const GUI_FONT * pFont;
  GUI_COLOR aBackColor[3];
  GUI_COLOR aTextColor[3];
  GUI_COLOR aColor[2];
  GUI_COLOR aScrollbarColor[3];
  DROPDOWN_SKIN_PRIVATE SkinPrivate;
  I16       TextBorderSize;
  I16       Align;
} DROPDOWN_PROPS;

typedef struct {
  WIDGET                 Widget;
  I16                    Sel;        // Current selection
  I16                    ySizeLB;    // ySize of assigned LISTBOX in expanded state
  I16                    TextHeight;
  GUI_ARRAY              Handles;
  WM_SCROLL_STATE        ScrollState;
  DROPDOWN_PROPS         Props;
  WIDGET_SKIN const    * pWidgetSkin;
  WM_HWIN                hListWin;
  U8                     Flags;
  U16                    ItemSpacing;
  U8                     ScrollbarWidth;
  char                   IsPressed;
  WM_HMEM                hDisabled;
  int                    LastMotionPosY;
  SCROLLER_Handle        hScrollerV;
  SCROLLER_Handle        hScrollerH;
} DROPDOWN_Obj;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define DROPDOWN_INIT_ID(p) (p->Widget.DebugId = WIDGET_TYPE_DROPDOWN)
#else
  #define DROPDOWN_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  DROPDOWN_Obj * DROPDOWN_LockH(DROPDOWN_Handle h);
  #define DROPDOWN_LOCK_H(h)   DROPDOWN_LockH(h)
#else
  #define DROPDOWN_LOCK_H(h)   (DROPDOWN_Obj *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private (module internal) data
*
**********************************************************************
*/

extern DROPDOWN_PROPS DROPDOWN__DefaultProps;

extern const WIDGET_SKIN DROPDOWN__SkinClassic;
extern       WIDGET_SKIN DROPDOWN__Skin;

extern WIDGET_SKIN const * DROPDOWN__pSkinDefault;

/*********************************************************************
*
*       Private functions
*
**********************************************************************
*/

void         DROPDOWN__AdjustHeight          (DROPDOWN_Handle hObj);
int          DROPDOWN__GetNumItems           (DROPDOWN_Obj * pObj);
void         DROPDOWN__Expand                (DROPDOWN_Handle hObj);
const char * DROPDOWN__GetpItemLocked        (DROPDOWN_Handle hObj, unsigned int Index);
void         DROPDOWN__RegisterPostExpandHook(GUI_REGISTER_HOOK * pRegisterHook);
void         DROPDOWN__RegisterPreExpandHook (GUI_REGISTER_HOOK * pRegisterHook);

#endif // GUI_WINSUPPORT
#endif // DROPDOWN_PRIVATE_H

/*************************** End of file ****************************/
