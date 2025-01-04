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
File        : ID_SCREEN_02.h
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#ifndef ID_SCREEN_02_H
#define ID_SCREEN_02_H

#include "AppWizard.h"

/*********************************************************************
*
*       Objects
*/
#define ID_IMAGE_01   (GUI_ID_USER + 3)
#define ID_PROGBAR_01 (GUI_ID_USER + 2)
#define ID_IMAGE_00   (GUI_ID_USER + 1)
#define ID_NUM_FPS    (GUI_ID_USER + 6)
#define ID_BOX_N01    (GUI_ID_USER + 4)
#define ID_BOX_N02    (GUI_ID_USER + 5)
#define ID_BOX_N03    (GUI_ID_USER + 16)
#define ID_BOX_N04    (GUI_ID_USER + 17)
#define ID_BOX_N05    (GUI_ID_USER + 23)
#define ID_BOX_N06    (GUI_ID_USER + 24)
#define ID_NUM_01     (GUI_ID_USER + 7)
#define ID_NUM_02     (GUI_ID_USER + 8)
#define ID_NUM_03     (GUI_ID_USER + 9)
#define ID_NUM_04     (GUI_ID_USER + 10)
#define ID_NUM_05     (GUI_ID_USER + 11)
#define ID_NUM_06     (GUI_ID_USER + 12)
#define ID_NUM_AF     (GUI_ID_USER + 13)
#define ID_NUM_SPEED  (GUI_ID_USER + 14)
#define ID_NUM_GEAR   (GUI_ID_USER + 15)
#define ID_NUM_TRIP   (GUI_ID_USER + 21)
#define ID_NUM_ODO    (GUI_ID_USER + 20)
#define ID_PROGBAR_00 (GUI_ID_USER + 22)
#define ID_TEXT_01    (GUI_ID_USER + 18)
#define ID_TEXT_02    (GUI_ID_USER + 19)
#define ID_TEXT_03    (GUI_ID_USER + 25)
#define ID_TEXT_04    (GUI_ID_USER + 26)
#define ID_TEXT_05    (GUI_ID_USER + 27)
#define ID_TEXT_06    (GUI_ID_USER + 28)

/*********************************************************************
*
*       Slots
*/
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FPS__APPW_JOB_SETVALUE   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETVALUE    (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED__APPW_JOB_SETVALUE (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_ODO__APPW_JOB_SETVALUE   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_TRIP__APPW_JOB_SETVALUE  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_0 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_1 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_2 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_3 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_4 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_5 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_0 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_1 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_2 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N02__APPW_JOB_SETCOLOR   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N02__APPW_JOB_SETCOLOR_0 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR_0 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR_1 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N05__APPW_JOB_SETCOLOR   (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N05__APPW_JOB_SETCOLOR_0 (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_0  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_1  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_2  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_3  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED                                  (APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);
void ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_00__APPW_JOB_SETVALUE(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult);

/*********************************************************************
*
*       Callback
*/
void cbID_SCREEN_02(WM_MESSAGE * pMsg);

#endif  // ID_SCREEN_02_H

/*************************** End of file ****************************/
