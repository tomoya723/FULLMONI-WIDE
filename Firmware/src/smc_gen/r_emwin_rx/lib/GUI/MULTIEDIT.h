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
File        : MULTIEDIT.h
Purpose     : MULTIEDIT include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef MULTIEDIT_H
#define MULTIEDIT_H

#include "WM.h"
#include "WIDGET.h"
#include "DIALOG_Type.h"      /* Req. for Create indirect data structure */

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       MULTIEDIT create flags
*
*  Description
*    Create flags that define the behavior of the FRAMEWIN widget. These flags are OR-combinable
*    and can be specified upon creation of the widget via the \a{ExFlags} parameter of MULTIEDIT_CreateEx().
*/
#define MULTIEDIT_CF_READONLY           (1 << 0)     // Enables read only mode.
#define MULTIEDIT_CF_INSERT             (1 << 2)     // Enables insert mode.
#define MULTIEDIT_CF_AUTOSCROLLBAR_V    (1 << 3)     // Automatic use of a vertical scroll bar.
#define MULTIEDIT_CF_AUTOSCROLLBAR_H    (1 << 4)     // Automatic use of a horizontal scroll bar.
#define MULTIEDIT_CF_PASSWORD           (1 << 5)     // Enables password mode.
#define MULTIEDIT_CF_SHOWCURSOR         (1 << 6)     // Shows the cursor.
#define MULTIEDIT_CF_MOTION_H           (1 << 7)     // Enables motion support on X-axis.
#define MULTIEDIT_CF_MOTION_V           (1 << 8)     // Enables motion support on Y-axis.
/* status flags */
#define MULTIEDIT_SF_READONLY           MULTIEDIT_CF_READONLY
#define MULTIEDIT_SF_INSERT             MULTIEDIT_CF_INSERT
#define MULTIEDIT_SF_AUTOSCROLLBAR_V    MULTIEDIT_CF_AUTOSCROLLBAR_V
#define MULTIEDIT_SF_AUTOSCROLLBAR_H    MULTIEDIT_CF_AUTOSCROLLBAR_H
#define MULTIEDIT_SF_PASSWORD           MULTIEDIT_CF_PASSWORD
#define MULTIEDIT_SF_MOTION_H           MULTIEDIT_CF_MOTION_H
#define MULTIEDIT_SF_MOTION_V           MULTIEDIT_CF_MOTION_V

/*********************************************************************
*
*       MULTIEDIT color indexes
*
*  Description
*    Color indexes used by the MULTIEDIT widget.
*/
#define MULTIEDIT_CI_EDIT               0                   // Color in edit mode.
#define MULTIEDIT_CI_READONLY           1                   // Color in read-only mode.

/*********************************************************************
*
*       MULTIEDIT cursor color indexes
*
*  Description
*    Color indexes used for the cursor of the MULTIEDIT widget.
*/
#define MULTIEDIT_CI_CURSOR_BK          2                   // Background color for cursor
#define MULTIEDIT_CI_CURSOR_FG          3                   // Foreground color for cursor

/*********************************************************************
*
*                         Public Types
*
**********************************************************************
*/

typedef WM_HMEM MULTIEDIT_HANDLE;

/*********************************************************************
*
*                 Create functions
*
**********************************************************************
*/
MULTIEDIT_HANDLE MULTIEDIT_Create        (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int Id, int Flags, int ExFlags, const char * pText, int MaxLen);
MULTIEDIT_HANDLE MULTIEDIT_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int BufferSize, const char * pText);
MULTIEDIT_HANDLE MULTIEDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);
MULTIEDIT_HANDLE MULTIEDIT_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int BufferSize, const char * pText, int NumExtraBytes);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MULTIEDIT_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*                 Member functions
*
**********************************************************************
*/

int              MULTIEDIT_AddKey               (MULTIEDIT_HANDLE hObj, U16 Key);
int              MULTIEDIT_AddText              (MULTIEDIT_HANDLE hObj, const char * s);
void             MULTIEDIT_EnableBlink          (MULTIEDIT_HANDLE hObj, int Period, int OnOff);
void             MULTIEDIT_EnableMotion         (MULTIEDIT_HANDLE hObj, int Flags);
GUI_COLOR        MULTIEDIT_GetBkColor           (MULTIEDIT_HANDLE hObj, unsigned Index);
int              MULTIEDIT_GetCursorCharPos     (MULTIEDIT_HANDLE hObj);
void             MULTIEDIT_GetCursorPixelPos    (MULTIEDIT_HANDLE hObj, int * pxPos, int * pyPos);
const GUI_FONT * MULTIEDIT_GetFont              (MULTIEDIT_HANDLE hObj);
int              MULTIEDIT_GetNumChars          (MULTIEDIT_HANDLE hObj);
void             MULTIEDIT_GetPrompt            (MULTIEDIT_HANDLE hObj, char* sDest, int MaxNumChars);
void             MULTIEDIT_GetText              (MULTIEDIT_HANDLE hObj, char* sDest, int MaxNumChars);
GUI_COLOR        MULTIEDIT_GetTextColor         (MULTIEDIT_HANDLE hObj, unsigned Index);
int              MULTIEDIT_GetTextFromLine      (MULTIEDIT_HANDLE hObj, char * sDest, int MaxLen, unsigned CharPos, unsigned Line);
int              MULTIEDIT_GetTextFromPos       (MULTIEDIT_HANDLE hObj, char * sDest, int MaxLen, int CharStart, int LineStart, int CharEnd, int LineEnd);
int              MULTIEDIT_GetTextSize          (MULTIEDIT_HANDLE hObj);
int              MULTIEDIT_GetUserData          (MULTIEDIT_HANDLE hObj, void * pDest, int NumBytes);
void             MULTIEDIT_SetTextAlign         (MULTIEDIT_HANDLE hObj, int Align);
void             MULTIEDIT_SetAutoScrollH       (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetAutoScrollV       (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetBkColor           (MULTIEDIT_HANDLE hObj, unsigned Index, GUI_COLOR color);
void             MULTIEDIT_SetCursorColor       (MULTIEDIT_HANDLE hObj, unsigned Index, GUI_COLOR color);
void             MULTIEDIT_SetCursorCharPos     (MULTIEDIT_HANDLE hObj, int x, int y);       /* Not yet implemented */
void             MULTIEDIT_SetCursorPixelPos    (MULTIEDIT_HANDLE hObj, int x, int y);       /* Not yet implemented */
void             MULTIEDIT_SetCursorOffset      (MULTIEDIT_HANDLE hObj, int Offset);
void             MULTIEDIT_SetHBorder           (MULTIEDIT_HANDLE hObj, unsigned HBorder);
void             MULTIEDIT_SetFocusable         (MULTIEDIT_HANDLE hObj, int State);
void             MULTIEDIT_SetFont              (MULTIEDIT_HANDLE hObj, const GUI_FONT * pFont);
void             MULTIEDIT_SetInsertMode        (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetInvertCursor      (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetBufferSize        (MULTIEDIT_HANDLE hObj, int BufferSize);
void             MULTIEDIT_SetMaxNumChars       (MULTIEDIT_HANDLE hObj, unsigned MaxNumChars);
void             MULTIEDIT_SetPrompt            (MULTIEDIT_HANDLE hObj, const char* sPrompt);
void             MULTIEDIT_SetReadOnly          (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetPasswordMode      (MULTIEDIT_HANDLE hObj, int OnOff);
void             MULTIEDIT_SetText              (MULTIEDIT_HANDLE hObj, const char* s);
void             MULTIEDIT_SetTextColor         (MULTIEDIT_HANDLE hObj, unsigned Index, GUI_COLOR color);
int              MULTIEDIT_SetUserData          (MULTIEDIT_HANDLE hObj, const void * pSrc, int NumBytes);
void             MULTIEDIT_SetWrapNone          (MULTIEDIT_HANDLE hObj);
void             MULTIEDIT_SetWrapChar          (MULTIEDIT_HANDLE hObj);
void             MULTIEDIT_SetWrapWord          (MULTIEDIT_HANDLE hObj);
int              MULTIEDIT_ShowCursor           (MULTIEDIT_HANDLE hObj, unsigned OnOff);
//
// Default getters/setters
//
int              MULTIEDIT_GetDefaultAlign      (void);
GUI_COLOR        MULTIEDIT_GetDefaultBkColor    (unsigned Index);
GUI_COLOR        MULTIEDIT_GetDefaultCursorColor(unsigned Index);
const GUI_FONT * MULTIEDIT_GetDefaultFont       (void);
unsigned         MULTIEDIT_GetDefaultHBorder    (void);
GUI_COLOR        MULTIEDIT_GetDefaultTextColor  (unsigned Index);
void             MULTIEDIT_SetDefaultAlign      (int Align);
void             MULTIEDIT_SetDefaultBkColor    (GUI_COLOR Color, unsigned Index);
void             MULTIEDIT_SetDefaultCursorColor(GUI_COLOR Color, unsigned Index);
void             MULTIEDIT_SetDefaultFont       (const GUI_FONT * pFont);
void             MULTIEDIT_SetDefaultHBorder    (unsigned HBorder);
void             MULTIEDIT_SetDefaultTextColor  (GUI_COLOR Color, unsigned Index);

/*********************************************************************
*
*       Macros for compatibility with older versions
*
**********************************************************************
*/

#define MULTIEDIT_SetMaxLen(hObj, MaxLen) MULTIEDIT_SetBufferSize(hObj, MaxLen)
#define MULTIEDIT_GetStringSize           MULTIEDIT_GetTextSize
#define MULTIEDIT_SetFocussable           MULTIEDIT_SetFocusable

#if defined(__cplusplus)
  }
#endif

#endif  // GUI_WINSUPPORT
#endif  // MULTIEDIT_H

/*************************** End of file ****************************/
