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
File        : ID_SCREEN_01c.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_01c.h"

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
    ID_SCREEN_01c, 0,
    { { { DISPOSE_MODE_REL_PARENT, 630, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      170, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_01c,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      170, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_FPS, ID_SCREEN_01c,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 44, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      32, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_SPEED, ID_SCREEN_01c,
    { { { DISPOSE_MODE_REL_PARENT, 9, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 150, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_TRIP, ID_SCREEN_01c,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 190, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 32, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      50, 20, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_ODO, ID_SCREEN_01c,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 209, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 32, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 20, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_00, ID_SCREEN_01c,
    { { { DISPOSE_MODE_REL_PARENT, 16, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 234, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 15, 0, 0, 0, 0
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
  { ID_IMAGE_00,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack2c),
                                                 ARG_V(131126), } },
  { ID_NUM_FPS,    APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_NUM_FPS,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_FPS,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_NUM_FPS,    APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_40_Normal_EXT) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_ODO,    APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_ODO,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_ODO,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT) } },
  { ID_NUM_ODO,    APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_PROGBAR_00, APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_00, APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acfuel1),
                                                 ARG_VP(0, acfuel2), } },
  { ID_PROGBAR_00, APPW_SET_PROP_TILE,         { ARG_V(0) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_VAR_FPS,    WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_FPS,    APPW_JOB_SETVALUE,       ID_SCREEN_01c__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FPS__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_TRIP,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_TRIP,   APPW_JOB_SETVALUE,       ID_SCREEN_01c__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_TRIP__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_ODO,    WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_ODO,    APPW_JOB_SETVALUE,       ID_SCREEN_01c__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_FUEL,   WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_00, APPW_JOB_SETVALUE,       ID_SCREEN_01c__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_00__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
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
*       ID_SCREEN_01c_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_01c_RootInfo = {
  ID_SCREEN_01c,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_01c,
  0
};

/*************************** End of file ****************************/
