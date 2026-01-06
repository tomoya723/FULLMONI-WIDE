/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2026  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_PRM.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_PRM.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _aCreate
*/
static APPW_CREATE_ITEM _aCreate[] = {
  { WM_OBJECT_WINDOW_Create,
    ID_SCREEN_PRM, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_00, ID_SCREEN_PRM,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00, ID_SCREEN_PRM,
    { { { DISPOSE_MODE_REL_PARENT, 162, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 115, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      478, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_BOX_00,     APPW_SET_PROP_COLOR,        { ARG_V(0xc8000000) } },
  { ID_TEXT_00,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_00,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_TEXT_00,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_PRM) } },
  { ID_TEXT_00,    APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_40_Normal_EXT) } },
};

/*********************************************************************
*
*       Comparison(s)
*/
static APPW_COND_COMP _aComparison_00[] = {
  { { { APPW_IS_VAR, ID_VAR_PRM }, { APPW_IS_VAL, 0 } }, APPW__CompareIsEqual },
};

/*********************************************************************
*
*       Condition(s)
*/
static GUI_CONST_STORAGE APPW_COND _Condition_00 = { "A", _aComparison_00, GUI_COUNTOF(_aComparison_00) };

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_VAR_PRM,    WM_NOTIFICATION_VALUE_CHANGED,    0,             APPW_JOB_CLOSESCREEN,    ID_SCREEN_PRM__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(ID_SCREEN_PRM),
    }, 0, &_Condition_00
  },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_PRM_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_PRM_RootInfo = {
  ID_SCREEN_PRM,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_PRM,
  0
};

/*************************** End of file ****************************/
