/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : Resource.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "AppWizard.h"

/*********************************************************************
*
*       Text
*/
#define ID_RTEXT_1 0
#define ID_RTEXT_GN 1
#define ID_RTEXT_G1 2
#define ID_RTEXT_G2 3
#define ID_RTEXT_G3 4
#define ID_RTEXT_G4 5
#define ID_RTEXT_G5 6
#define ID_RTEXT_G6 7
#define ID_RTEXT_T1 8
#define ID_RTEXT_T2 9
#define ID_RTEXT_T3 10
#define ID_RTEXT_T4 11
#define ID_RTEXT_T5 12
#define ID_RTEXT_T6 13
#define ID_RTEXT_0 14
#define ID_RTEXT_2 15
#define ID_RTEXT_3 16
#define ID_RTEXT_4 17

#define APPW_MANAGE_TEXT APPW_MANAGE_TEXT_EXT
extern GUI_CONST_STORAGE unsigned char acAPPW_Language_0[];

/*********************************************************************
*
*       Fonts
*/
extern GUI_CONST_STORAGE unsigned char acArial_19_Normal_EXT[];
extern GUI_CONST_STORAGE unsigned char ac51_30_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char ac51_40_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acSuiGenerisRg_120_Bold_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char ac51_20_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acArial_16_Bold_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acSuiGenerisRg_16_Normal_EXT_AA4[];
extern GUI_CONST_STORAGE unsigned char acArialBlack_30_Black_EXT[];
extern GUI_CONST_STORAGE unsigned char ac51_60_Normal_EXT_AA4[];

/*********************************************************************
*
*       Images
*/
extern GUI_CONST_STORAGE unsigned char acmotec[];
extern GUI_CONST_STORAGE unsigned char acBack2[];
extern GUI_CONST_STORAGE unsigned char acMarker_red_bar_80x7s[];
extern GUI_CONST_STORAGE unsigned char acfuel1[];
extern GUI_CONST_STORAGE unsigned char acfuel2[];
extern GUI_CONST_STORAGE unsigned char acBack3[];
extern GUI_CONST_STORAGE unsigned char acrev[];

/*********************************************************************
*
*       Variables
*/
#define ID_VAR_01 (GUI_ID_USER + 2049)
#define ID_VAR_02 (GUI_ID_USER + 2050)
#define ID_VAR_03 (GUI_ID_USER + 2051)
#define ID_VAR_04 (GUI_ID_USER + 2052)
#define ID_VAR_05 (GUI_ID_USER + 2053)
#define ID_VAR_06 (GUI_ID_USER + 2054)
#define ID_VAR_AF (GUI_ID_USER + 2055)
#define ID_VAR_REV (GUI_ID_USER + 2056)
#define ID_VAR_BT (GUI_ID_USER + 2057)
#define ID_VAR_FL1 (GUI_ID_USER + 2058)
#define ID_VAR_BL1 (GUI_ID_USER + 2059)
#define ID_VAR_GEAR (GUI_ID_USER + 2048)
#define ID_VAR_BATT (GUI_ID_USER + 2060)
#define ID_VAR_SPEED (GUI_ID_USER + 2061)
#define ID_VAR_FPS (GUI_ID_USER + 2062)
#define ID_VAR_ODO (GUI_ID_USER + 2063)
#define ID_VAR_TRIP (GUI_ID_USER + 2064)
#define ID_VAR_REV_A (GUI_ID_USER + 2066)
#define ID_VAR_SW (GUI_ID_USER + 2065)
#define ID_VAR_FUEL (GUI_ID_USER + 2067)

/*********************************************************************
*
*       Animations
*/
#define ID_ANIM_000 (GUI_ID_USER + 0)

extern GUI_CONST_STORAGE APPW_ANIM_DATA ID_ANIM_000_Data;

/*********************************************************************
*
*       Screens
*/
#define ID_SCREEN_00 (GUI_ID_USER + 4096)
#define ID_SCREEN_01 (GUI_ID_USER + 4098)
#define ID_SCREEN_02 (GUI_ID_USER + 4097)

extern APPW_ROOT_INFO ID_SCREEN_00_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_01_RootInfo;
extern APPW_ROOT_INFO ID_SCREEN_02_RootInfo;

#define APPW_INITIAL_SCREEN &ID_SCREEN_00_RootInfo

/*********************************************************************
*
*       Project path
*/
#define APPW_PROJECT_PATH "D:/e2_studio/workspace/FULLMONI_WIDEgcc/aw"

#endif  // RESOURCE_H

/*************************** End of file ****************************/
