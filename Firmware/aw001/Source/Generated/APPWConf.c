/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : APPWConf.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "AppWizard.h"
#include "Resource.h"

#ifdef WIN32
  #include "GUIDRV_Win32R.h"
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define XSIZE_PHYS       800
#define YSIZE_PHYS       256
#define COLOR_CONVERSION GUICC_565
#define DISPLAY_DRIVER   GUIDRV_WIN32R
#define NUM_BUFFERS      2
#define _appDrawing      NULL
#define _NumDrawings     0
#define _aScrollerList   NULL
#define _NumScrollers    0

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _apRootList
*/
static APPW_ROOT_INFO * _apRootList[] = {
  &ID_SCREEN_00_RootInfo,
  &ID_SCREEN_Telltale_RootInfo,
  &ID_SCREEN_01a_RootInfo,
  &ID_SCREEN_01b_RootInfo,
  &ID_SCREEN_01c_RootInfo,
};

/*********************************************************************
*
*       _NumScreens
*/
static unsigned _NumScreens = GUI_COUNTOF(_apRootList);

/*********************************************************************
*
*       _aVarList
*/
static APPW_VAR_OBJECT _aVarList[] = {
  { ID_VAR_01, 0, 805, NULL },
  { ID_VAR_02, 0, 201, NULL },
  { ID_VAR_03, 0, 0, NULL },
  { ID_VAR_04, 0, 0, NULL },
  { ID_VAR_05, 0, 0, NULL },
  { ID_VAR_06, 0, 0, NULL },
  { ID_VAR_AF, 0, 147, NULL },
  { ID_VAR_REV, 0, 0, NULL },
  { ID_VAR_BT, 0, 0, NULL },
  { ID_VAR_FL1, 0, 0, NULL },
  { ID_VAR_BL1, 0, 0, NULL },
  { ID_VAR_GEAR, 0, 0, NULL },
  { ID_VAR_BATT, 0, 0, NULL },
  { ID_VAR_SPEED, 0, 0, NULL },
  { ID_VAR_FPS, 0, 0, NULL },
  { ID_VAR_ODO, 0, 0, NULL },
  { ID_VAR_TRIP, 0, 0, NULL },
  { ID_VAR_REV_A, 0, 0, NULL },
  { ID_VAR_SW, 0, 0, NULL },
  { ID_VAR_FUEL, 0, 0, NULL },
  { ID_VAR_AD1, 0, 0, NULL },
  { ID_VAR_AD2, 0, 0, NULL },
  { ID_VAR_AD3, 0, 0, NULL },
  { ID_VAR_AD4, 0, 0, NULL },
};

/*********************************************************************
*
*       _NumVars
*/
static unsigned _NumVars = GUI_COUNTOF(_aVarList);

/*********************************************************************
*
*       _aID_ANIM_000_Items
*/
static GUI_CONST_STORAGE APPW_ANIM_ITEM _aID_ANIM_000_Items[] = {
  { ANIM_LINEAR,
    { 0x0000, 0x7fff },
    { { 0x00000000, 0, ATOM_OBJECT_GEO, ATOM_DETAIL_X0 },
      { 0x00000000, 0, ATOM_OBJECT_GEO, ATOM_DETAIL_X0 },
      { 0x00000000, 0, ATOM_OBJECT_GEO, ATOM_DETAIL_X0 }
    }
  }
};

/*********************************************************************
*
*       ID_ANIM_000_Data
*/
GUI_CONST_STORAGE APPW_ANIM_DATA ID_ANIM_000_Data = { ID_ANIM_000, 10000, 1, -1, GUI_COUNTOF(_aID_ANIM_000_Items), _aID_ANIM_000_Items };

/*********************************************************************
*
*       Multibuffering
*/
static U8 _MultibufEnable = 1;

/*********************************************************************
*
*       _ShowMissingCharacters
*/
static U8 _ShowMissingCharacters = 1;

/*********************************************************************
*
*       _apLang
*/
static GUI_CONST_STORAGE char * _apLang[] = {
  (GUI_CONST_STORAGE char *)acAPPW_Language_0,
};

/*********************************************************************
*
*       _TextInit
*/
static GUI_CONST_STORAGE APPW_TEXT_INIT _TextInit = {
  _apLang,
  GUI_COUNTOF(_apLang),
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _InitText
*/
static void _InitText(void) {
  APPW_TextInitMem(&_TextInit);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       APPW_X_Setup
*/
void APPW_X_Setup(void) {
  APPW_SetpfInitText(_InitText);
  APPW_X_FS_Init();
  APPW_MULTIBUF_Enable(_MultibufEnable);
  APPW_SetData(_apRootList, _NumScreens, _aVarList, _NumVars, _aScrollerList, _NumScrollers, (APPW_DRAWING_ITEM **)_appDrawing, _NumDrawings);
  APPW_SetSupportScroller(0);
  GUI_ShowMissingCharacters(_ShowMissingCharacters);
}

/*********************************************************************
*
*       APPW_X_Config
*/
#ifdef WIN32
void APPW_X_Config(void) {
  GUI_MULTIBUF_Config(NUM_BUFFERS);
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
  }
}
#endif

/*************************** End of file ****************************/
