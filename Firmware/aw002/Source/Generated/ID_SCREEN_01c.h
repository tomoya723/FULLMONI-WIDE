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
File        : ID_SCREEN_01c.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_01C_H
#define ID_SCREEN_01C_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_00      (GUI_ID_USER + 1)
#define ID_NUM_FPS       (GUI_ID_USER + 6)
#define ID_NUM_SPEED     (GUI_ID_USER + 14)
#define ID_NUM_BT_01     (GUI_ID_USER + 17)
#define ID_NUM_BL_01     (GUI_ID_USER + 18)
#define ID_NUM_FL_01     (GUI_ID_USER + 19)
#define ID_NUM_AD_01     (GUI_ID_USER + 4)
#define ID_NUM_AD_02     (GUI_ID_USER + 23)
#define ID_NUM_AD_03     (GUI_ID_USER + 24)
#define ID_NUM_AD_04     (GUI_ID_USER + 25)
#define ID_NUM_TRIP      (GUI_ID_USER + 21)
#define ID_NUM_ODO       (GUI_ID_USER + 20)
#define ID_PROGBAR_00    (GUI_ID_USER + 22)
#define ID_NUM_SPEED_CAN (GUI_ID_USER + 39)
#define ID_TEXT_00       (GUI_ID_USER + 40)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_BT_01__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_FL_01__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_BL_01__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_01__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_02__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_03__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_04__APPW_JOB_SETVIS       (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FPS__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_BT_01__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_BL_01__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FL_01__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_ODO__APPW_JOB_SETVALUE      (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_TRIP__APPW_JOB_SETVALUE     (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG_0                                    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_00__APPW_JOB_SETVALUE   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_01__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_02__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_03__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_04__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED_CAN__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_01c(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_01C_H

/*************************** End of file ****************************/
