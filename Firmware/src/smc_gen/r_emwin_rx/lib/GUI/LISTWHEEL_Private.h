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
File        : LISTWHEEL_Private.h
Purpose     : Private LISTWHEEL include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef LISTWHEEL_PRIVATE_H
#define LISTWHEEL_PRIVATE_H

#include "GUI_Private.h"
#include "WM_Intern.h"
#include "LISTWHEEL.h"
#include "GUI_ARRAY.h"
#include "WIDGET.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define LISTWHEEL_STATE_PRESSED WIDGET_STATE_USER0

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/

typedef struct {
  void * pData;
  char   acText[1];
} LISTWHEEL_ITEM;

typedef struct {
  const GUI_FONT * pFont;
  GUI_COLOR        aBackColor[2];
  GUI_COLOR        aTextColor[2];
  I16              Align;
  unsigned         Deceleration;
} LISTWHEEL_PROPS;

typedef struct {
  WIDGET                  Widget;
  GUI_ARRAY               ItemArray;
  WIDGET_DRAW_ITEM_FUNC * pfOwnerDraw;
  LISTWHEEL_PROPS         Props;
  WM_HMEM                 hTimer;
  unsigned                LBorder;
  unsigned                RBorder;
  unsigned                LineHeight;
  int                     Sel;
  GUI_TIMER_TIME          TimeTouched;      // Time stamp of last touch event
  GUI_TIMER_TIME          TimeTouchedLast;  // Time of the last touch
  int                     PosTouchedLast;   // Last touched position in pixels
  int                     Pos;              // Current position in pixels
  int                     Velocity;         // Motion in pixels
  int                     SnapPosition;     // Snap position in pixels
  int                     TouchPos;         // Y-position of last touch event
  int                     ySizeData;        // Data size in pixels
  int                     Destination;      // Destination position in pixels
  GUI_TIMER_TIME          TimerPeriod;      // Period of timer events
} LISTWHEEL_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define LISTWHEEL_INIT_ID(p)  (p->Widget.DebugId = WIDGET_TYPE_LISTWHEEL)
#else
  #define LISTWHEEL_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  LISTWHEEL_OBJ * LISTWHEEL_LockH(LISTWHEEL_Handle h);
  #define LISTWHEEL_LOCK_H(h)   LISTWHEEL_LockH(h)
#else
  #define LISTWHEEL_LOCK_H(h)   (LISTWHEEL_OBJ *)WM_LOCK_H(h)
#endif

/*********************************************************************
*
*       Private (module internal) data
*
**********************************************************************
*/
extern LISTWHEEL_PROPS LISTWHEEL_DefaultProps;

/*********************************************************************
*
*       Private (module internal) functions
*
**********************************************************************
*/
const char * LISTWHEEL__GetpStringLocked(LISTWHEEL_Handle hObj, int Index, LISTWHEEL_ITEM ** ppItem);

#endif // GUI_WINSUPPORT
#endif // LISTWHEEL_PRIVATE_H

/*************************** End of file ****************************/
