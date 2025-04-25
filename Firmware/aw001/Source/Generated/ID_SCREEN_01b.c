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
File        : ID_SCREEN_01b.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_01b.h"

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
    ID_SCREEN_01b, 0,
    { { { DISPOSE_MODE_REL_PARENT, 35, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      237, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      237, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_01, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_02, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 58, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_03, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 96, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_04, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 134, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_05, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 172, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_06, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 209, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      75, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_00,   APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_00,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack2b),
                                                 ARG_V(182326), } },
  { ID_NUM_01,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_01,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_01,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_01,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_02,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_02,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_02,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_02,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_03,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_03,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_03,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_03,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_04,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_04,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_04,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_04,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_05,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_05,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_05,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_05,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_06,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_06,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_06,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_06,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_VAR_01,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_01,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE,
    { ARG_V(4097),
    }, 65537, NULL
  },
  { ID_VAR_02,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_02,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE,
    { ARG_V(4098),
    }, 65537, NULL
  },
  { ID_VAR_03,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_03,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE,
    { ARG_V(4099),
    }, 65537, NULL
  },
  { ID_VAR_04,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_04,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE,
    { ARG_V(4100),
    }, 65537, NULL
  },
  { ID_VAR_05,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_05,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE,
    { ARG_V(4101),
    }, 65537, NULL
  },
  { ID_VAR_06,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_06,     APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE,
    { ARG_V(4102),
    }, 65537, NULL
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
*       ID_SCREEN_01b_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_01b_RootInfo = {
  ID_SCREEN_01b,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_01b,
  0
};

/*************************** End of file ****************************/
