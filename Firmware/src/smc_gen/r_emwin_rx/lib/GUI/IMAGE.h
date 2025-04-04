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
File        : IMAGE.h
Purpose     : Image include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef IMAGE_H
#define IMAGE_H

#include "WM.h"
#include "DIALOG_Type.h"
#include "WIDGET.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" { // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       IMAGE create flags
*
*  Description
*    Create flags for the IMAGE widget. These flags can be passed to ICONVIEW_CreateEx() via the
*    \a{ExFlags} parameter.
*/
#define IMAGE_CF_MEMDEV   (1 << 0)      // Makes the IMAGE widget use an internal Memory Device for
                                        // drawing. Contrary to the Memory Device which is created
                                        // by the Window Manager's automatic use of Memory Devices
                                        // (WM_CF_MEMDEV), this device stays valid all the time. It has to be
                                        // ensured that the emWin memory pool which is defined by the
                                        // function GUI_ALLOC_AssignMemory() (in \c{GUIConf.c}), is big enough
                                        // to store the complete data. If the Memory Device can not be created,
                                        // the image is drawn directly. This might possibly mean loss
                                        // of performance.
#define IMAGE_CF_TILE     (1 << 1)      // Uses tiling to fill up the whole area of the widget.
#define IMAGE_CF_ALPHA    (1 << 2)      // Needs to be set if alpha blending is required (PNG).
#define IMAGE_CF_ATTACHED (1 << 3)      // Widget size is fixed to the parent border.
#define IMAGE_CF_AUTOSIZE (1 << 4)      // Widget size is taken from the attached image.
#define IMAGE_CF_LQ       (1 << 5)      // Fast mode (lower quality) should be used for rotating/scaling.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef WM_HMEM IMAGE_Handle;

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
IMAGE_Handle IMAGE_CreateEx      (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id);
IMAGE_Handle IMAGE_CreateUser    (int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int NumExtraBytes);
IMAGE_Handle IMAGE_CreateIndirect(const GUI_WIDGET_CREATE_INFO * pCreateInfo, WM_HWIN hWinParent, int x0, int y0, WM_CALLBACK * cb);

void IMAGE_Callback(WM_MESSAGE * pMsg);

/*********************************************************************
*
*       Member functions
*
**********************************************************************
*/
int  IMAGE_GetImageSize(IMAGE_Handle hObj, int * pxSize, int * pySize);
int  IMAGE_GetUserData (IMAGE_Handle hObj, void * pDest, int NumBytes);
void IMAGE_SetBitmap   (IMAGE_Handle hWin, const GUI_BITMAP * pBitmap);
void IMAGE_SetBMP      (IMAGE_Handle hObj, const void * pData, U32 FileSize);
void IMAGE_SetBMPEx    (IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid);
void IMAGE_SetDTA      (IMAGE_Handle hObj, const void * pData, U32 FileSize);
void IMAGE_SetDTAEx    (IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid);
void IMAGE_SetGIF      (IMAGE_Handle hObj, const void * pData, U32 FileSize);
void IMAGE_SetGIFEx    (IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid);
void IMAGE_SetJPEG     (IMAGE_Handle hObj, const void * pData, U32 FileSize);
void IMAGE_SetJPEGEx   (IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid);
void IMAGE_SetPNG      (IMAGE_Handle hObj, const void * pData, U32 FileSize);
void IMAGE_SetPNGEx    (IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid);
void IMAGE_SetTiled    (IMAGE_Handle hObj, int OnOff);
int  IMAGE_SetUserData (IMAGE_Handle hObj, const void * pSrc, int NumBytes);
void IMAGE_SetAlign    (IMAGE_Handle hObj, int Align);
void IMAGE_SetOffset   (IMAGE_Handle hObj, int xOff, int yOff);

#if GUI_SUPPORT_MEMDEV
U8   IMAGE_SetAlpha    (IMAGE_Handle hObj, U8 Alpha);
int  IMAGE_SetAngle    (IMAGE_Handle hObj, unsigned Angle);
void IMAGE_EnableLQ    (IMAGE_Handle hObj, int OnOff);
int  IMAGE_SetScale    (IMAGE_Handle hObj, unsigned Scale);
#endif

#if defined(__cplusplus)
  }
#endif

#endif // GUI_WINSUPPORT
#endif // IMAGE_H

/*************************** End of file ****************************/
