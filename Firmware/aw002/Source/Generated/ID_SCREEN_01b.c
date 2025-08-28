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
      240, 256, 0, 0, 0, 0
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
      240, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_WT, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 21, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_WT_WARN, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 33, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 2, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_IAT, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 64, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_IAT_WARN, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 76, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 2, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_MAP, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 146, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_OT, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 107, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_OT_WARN, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 119, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 2, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_OILP, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 185, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_OP_WARN, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 197, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 2, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_BATV, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 224, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 10, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_BATTV_WARN, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 43, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 236, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      80, 2, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_01, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 21, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_02, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 61, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_03, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 103, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_04, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 135, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 143, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_05, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 128, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 180, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      67, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_06, ID_SCREEN_01b,
    { { { DISPOSE_MODE_REL_PARENT, 127, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 215, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      68, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_00,           APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack6),
                                                         ARG_V(184374), } },
  { ID_IMAGE_00,           APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(120) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(100) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(120) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(100) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_VALUE,        { ARG_V(60) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(100) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_RANGE,        { ARG_V(20),
                                                         ARG_V(120) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(150) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(110) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(150) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_VALUE,        { ARG_V(200) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(800) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(100) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xffaa0000),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(800) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_VALUE,        { ARG_V(10) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_RANGE,        { ARG_V(80),
                                                         ARG_V(160) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_VALUE,        { ARG_V(10) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_COLORS,       { ARG_V(0xffaa0000),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_RANGE,        { ARG_V(8),
                                                         ARG_V(16) } },
  { ID_NUM_01,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_01,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_01,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_01,             APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_02,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_02,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_02,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_02,             APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_03,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_03,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_03,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_03,             APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_04,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_04,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_04,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_04,             APPW_SET_PROP_DECMODE,      { ARG_V(4),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_05,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_05,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_05,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_05,             APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_06,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_06,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_06,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_27_Bold_EXT) } },
  { ID_NUM_06,             APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
};

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_01b,         APPW_NOTIFICATION_INITDIALOG,     ID_VAR_SW,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__APPW_NOTIFICATION_INITDIALOG,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_01,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_WT,         APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_WT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_02,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_IAT,        APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_IAT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_03,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_OT,         APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_OT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_04,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_MAP,        APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_MAP__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_05,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_OILP,       APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_OILP__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_06,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_BATV,       APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_BATV__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_01,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_01,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_02,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_02,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_03,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_03,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_04,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_04,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_05,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_05,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_06,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_06,             APPW_JOB_SETVALUE,       ID_SCREEN_02_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE,
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
