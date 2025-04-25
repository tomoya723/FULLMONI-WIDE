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
File        : ID_SCREEN_01a.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_01a.h"

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
    ID_SCREEN_01a, 0,
    { { { DISPOSE_MODE_REL_PARENT, 272, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      358, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      358, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_GAUGE_Create,
    ID_GAUGE_00, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      256, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_ROTARY_Create,
    ID_ROTARY_00, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      256, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AF, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 182, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 193, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      130, 50, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_GEAR, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 259, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 72, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 100, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_RV, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 128, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 143, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_TIME, ID_SCREEN_01a,
    { { { DISPOSE_MODE_REL_PARENT, 273, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      72, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_00,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack2a),
                                                 ARG_V(276534), } },
  { ID_IMAGE_00,   APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_GAUGE_00,   APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                 ARG_V(2700) } },
  { ID_GAUGE_00,   APPW_SET_PROP_SPAN,         { ARG_V(0),
                                                 ARG_V(9000) } },
  { ID_GAUGE_00,   APPW_SET_PROP_RADIUS,       { ARG_V(107) } },
  { ID_GAUGE_00,   APPW_SET_PROP_COLORS,       { ARG_V(GUI_INVALID_COLOR),
                                                 ARG_V(GUI_RED),
                                                 ARG_V(GUI_INVALID_COLOR) } },
  { ID_GAUGE_00,   APPW_SET_PROP_VALUES,       { ARG_V(19),
                                                 ARG_V(19) } },
  { ID_GAUGE_00,   APPW_SET_PROP_VALUE,        { ARG_V(500) } },
  { ID_ROTARY_00,  APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, NULL),
                                                 ARG_VP(0, acMarker_red_bar_80x7s), } },
  { ID_ROTARY_00,  APPW_SET_PROP_POS,          { ARG_V(76) } },
  { ID_ROTARY_00,  APPW_SET_PROP_OFFSET,       { ARG_V(2700) } },
  { ID_ROTARY_00,  APPW_SET_PROP_VALUE,        { ARG_V(3450) } },
  { ID_ROTARY_00,  APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ROTARY_00,  APPW_SET_PROP_ROTATE,       { ARG_V(0) } },
  { ID_NUM_AF,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AF,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_AF,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_40_Normal_EXT) } },
  { ID_NUM_AF,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_COLOR,        { ARG_V(0xffff5500) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_GN) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_FONT,         { ARG_VP(0, acSuiGenerisRg_120_Bold_EXT) } },
  { ID_NUM_RV,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_RV,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_RV,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_30_Normal_EXT) } },
  { ID_NUM_RV,     APPW_SET_PROP_DECMODE,      { ARG_V(4),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_TIME,   APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_NUM_TIME,   APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_20_Normal_EXT) } },
  { ID_NUM_TIME,   APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_4) } },
};

/*********************************************************************
*
*       Comparison(s)
*/
static APPW_COND_COMP _aComparison_06[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 0 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_07[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 1 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_08[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 2 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_09[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 3 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0a[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 4 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0b[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 5 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0c[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 6 } }, APPW__CompareIsEqual },
};

/*********************************************************************
*
*       Condition(s)
*/
static GUI_CONST_STORAGE APPW_COND _Condition_06 = { "A", _aComparison_06, GUI_COUNTOF(_aComparison_06) };
static GUI_CONST_STORAGE APPW_COND _Condition_07 = { "A", _aComparison_07, GUI_COUNTOF(_aComparison_07) };
static GUI_CONST_STORAGE APPW_COND _Condition_08 = { "A", _aComparison_08, GUI_COUNTOF(_aComparison_08) };
static GUI_CONST_STORAGE APPW_COND _Condition_09 = { "A", _aComparison_09, GUI_COUNTOF(_aComparison_09) };
static GUI_CONST_STORAGE APPW_COND _Condition_0a = { "A", _aComparison_0a, GUI_COUNTOF(_aComparison_0a) };
static GUI_CONST_STORAGE APPW_COND _Condition_0b = { "A", _aComparison_0b, GUI_COUNTOF(_aComparison_0b) };
static GUI_CONST_STORAGE APPW_COND _Condition_0c = { "A", _aComparison_0c, GUI_COUNTOF(_aComparison_0c) };

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_01a, APPW_NOTIFICATION_INITDIALOG,     0,             APPW_JOB_MODALMESSAGE,   ID_SCREEN_01a__APPW_NOTIFICATION_INITDIALOG,
    { ARG_V(ID_SCREEN_01b),
    }, 0, NULL
  },
  { ID_SCREEN_01a, APPW_NOTIFICATION_INITDIALOG,     0,             APPW_JOB_MODALMESSAGE,   ID_SCREEN_01a__APPW_NOTIFICATION_INITDIALOG_0,
    { ARG_V(ID_SCREEN_01c),
    }, 0, NULL
  },
  { ID_VAR_REV_A,  WM_NOTIFICATION_VALUE_CHANGED,    ID_ROTARY_00,  APPW_JOB_SETVALUE,       ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_ROTARY_00__APPW_JOB_SETVALUE,
    { ARG_V(4114),
    }, 65537, NULL
  },
  { ID_VAR_REV,    WM_NOTIFICATION_VALUE_CHANGED,    ID_GAUGE_00,   APPW_JOB_SETVALUE,       ID_SCREEN_01a__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE,
    { ARG_V(4104),
    }, 65537, NULL
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETVALUE,       ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETVALUE,
    { ARG_V(4103),
    }, 65537, NULL
  },
  { ID_VAR_REV,    WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_RV,     APPW_JOB_SETVALUE,       ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_RV__APPW_JOB_SETVALUE,
    { ARG_V(4104),
    }, 65537, NULL
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT,
    { ARG_V(ID_RTEXT_GN),
    }, 0, &_Condition_06
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_0,
    { ARG_V(ID_RTEXT_G1),
    }, 65536, &_Condition_07
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_1,
    { ARG_V(ID_RTEXT_G2),
    }, 65536, &_Condition_08
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_2,
    { ARG_V(ID_RTEXT_G3),
    }, 65536, &_Condition_09
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_3,
    { ARG_V(ID_RTEXT_G4),
    }, 65536, &_Condition_0a
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_4,
    { ARG_V(ID_RTEXT_G5),
    }, 65536, &_Condition_0b
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_01__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_5,
    { ARG_V(ID_RTEXT_G6),
    }, 65536, &_Condition_0c
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
*       ID_SCREEN_01a_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_01a_RootInfo = {
  ID_SCREEN_01a,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_01a,
  0
};

/*************************** End of file ****************************/
