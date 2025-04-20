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
File        : ID_SCREEN_02b.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_02B_H
#define ID_SCREEN_02B_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_00           (GUI_ID_USER + 1)
#define ID_PROGBAR_WT         (GUI_ID_USER + 9)
#define ID_PROGBAR_WT_WARN    (GUI_ID_USER + 26)
#define ID_PROGBAR_IAT        (GUI_ID_USER + 10)
#define ID_PROGBAR_IAT_WARN   (GUI_ID_USER + 28)
#define ID_PROGBAR_MAP        (GUI_ID_USER + 11)
#define ID_PROGBAR_OT         (GUI_ID_USER + 27)
#define ID_PROGBAR_OT_WARN    (GUI_ID_USER + 30)
#define ID_PROGBAR_OILP       (GUI_ID_USER + 12)
#define ID_PROGBAR_OP_WARN    (GUI_ID_USER + 29)
#define ID_PROGBAR_BATV       (GUI_ID_USER + 31)
#define ID_PROGBAR_BATTV_WARN (GUI_ID_USER + 32)
#define ID_NUM_01             (GUI_ID_USER + 33)
#define ID_NUM_02             (GUI_ID_USER + 34)
#define ID_NUM_03             (GUI_ID_USER + 35)
#define ID_NUM_04             (GUI_ID_USER + 38)
#define ID_NUM_05             (GUI_ID_USER + 41)
#define ID_NUM_06             (GUI_ID_USER + 42)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG                                     (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_WT__APPW_JOB_SETVALUE  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_IAT__APPW_JOB_SETVALUE (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_OT__APPW_JOB_SETVALUE  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_MAP__APPW_JOB_SETVALUE (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_OILP__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_BATV__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_02b(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_02B_H

/*************************** End of file ****************************/
