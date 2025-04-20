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
File        : ID_SCREEN_02.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_02.h"

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
    ID_SCREEN_02, 0,
    { { { DISPOSE_MODE_REL_PARENT, 35, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      765, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_GAUGE_Create,
    ID_GAUGE_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 237, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      256, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_ROTARY_Create,
    ID_ROTARY_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 237, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      256, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_FPS, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 34, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      32, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AF, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 433, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 193, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      130, 39, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_SPEED, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 604, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 150, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_GEAR, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 496, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 72, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 100, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_RV, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 365, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 143, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_BT_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 17, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_BL_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 31, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_FL_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 45, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AD_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 58, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AD_02, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 72, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AD_03, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 86, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AD_04, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 99, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 19, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      40, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_TRIP, ID_SCREEN_02,
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
    ID_NUM_ODO, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 209, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 32, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 20, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_TIME, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 510, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 31, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 612, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 234, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 15, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_AFR_WARN, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 427, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 237, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      139, 3, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_AFR, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 427, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 230, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      139, 6, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_SPEED_CAN, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 604, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 117, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 604, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 84, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 32, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_WT, ID_SCREEN_02,
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
    ID_PROGBAR_WT_WARN, ID_SCREEN_02,
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
    ID_PROGBAR_IAT, ID_SCREEN_02,
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
    ID_PROGBAR_IAT_WARN, ID_SCREEN_02,
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
    ID_PROGBAR_MAP, ID_SCREEN_02,
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
    ID_PROGBAR_OT, ID_SCREEN_02,
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
    ID_PROGBAR_OT_WARN, ID_SCREEN_02,
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
    ID_PROGBAR_OILP, ID_SCREEN_02,
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
    ID_PROGBAR_OP_WARN, ID_SCREEN_02,
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
    ID_PROGBAR_BATV, ID_SCREEN_02,
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
    ID_PROGBAR_BATTV_WARN, ID_SCREEN_02,
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
    ID_NUM_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 17, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_02, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 61, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_03, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 103, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_04, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 143, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_05, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 180, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      67, 28, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_06, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 215, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 570, 0, 0 },
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
  { ID_IMAGE_00,           APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack4),
                                                         ARG_V(587832), } },
  { ID_IMAGE_00,           APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_GAUGE_00,           APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(2700) } },
  { ID_GAUGE_00,           APPW_SET_PROP_SPAN,         { ARG_V(0),
                                                         ARG_V(9000) } },
  { ID_GAUGE_00,           APPW_SET_PROP_RADIUS,       { ARG_V(107) } },
  { ID_GAUGE_00,           APPW_SET_PROP_COLORS,       { ARG_V(GUI_INVALID_COLOR),
                                                         ARG_V(0xfffc0004),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_GAUGE_00,           APPW_SET_PROP_VALUES,       { ARG_V(19),
                                                         ARG_V(19) } },
  { ID_GAUGE_00,           APPW_SET_PROP_VALUE,        { ARG_V(500) } },
  { ID_ROTARY_00,          APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, NULL),
                                                         ARG_VP(0, acMarker_red_bar_80x7s), } },
  { ID_ROTARY_00,          APPW_SET_PROP_POS,          { ARG_V(78) } },
  { ID_ROTARY_00,          APPW_SET_PROP_ROTATE,       { ARG_V(0) } },
  { ID_ROTARY_00,          APPW_SET_PROP_OFFSET,       { ARG_V(2700) } },
  { ID_ROTARY_00,          APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ROTARY_00,          APPW_SET_PROP_VALUE,        { ARG_V(3450) } },
  { ID_NUM_FPS,            APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_NUM_FPS,            APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_FPS,            APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_NUM_FPS,            APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_AF,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AF,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_AF,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_36_Bold_EXT) } },
  { ID_NUM_AF,             APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_SPEED,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_SPEED,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_SPEED,          APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_36_Bold_EXT) } },
  { ID_NUM_SPEED,          APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_SPEED,          APPW_SET_PROP_WRAP,         { ARG_V(0) } },
  { ID_NUM_GEAR,           APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_GEAR,           APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_GEAR,           APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_GN) } },
  { ID_NUM_GEAR,           APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_72_Bold_EXT) } },
  { ID_NUM_RV,             APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_RV,             APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_RV,             APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_30_Bold_EXT_AA2) } },
  { ID_NUM_RV,             APPW_SET_PROP_DECMODE,      { ARG_V(4),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_BT_01,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_BT_01,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_BT_01,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_BL_01,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_BL_01,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_BL_01,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_FL_01,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_FL_01,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_FL_01,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_AD_01,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AD_01,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_AD_01,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_AD_02,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AD_02,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_AD_02,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_AD_03,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AD_03,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_AD_03,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_AD_04,          APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AD_04,          APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_AD_04,          APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_TRIP,           APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_TRIP,           APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_TRIP,           APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT) } },
  { ID_NUM_TRIP,           APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(1),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_ODO,            APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_ODO,            APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_ODO,            APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT) } },
  { ID_NUM_ODO,            APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_NUM_TIME,           APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_NUM_TIME,           APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_18_Normal_EXT) } },
  { ID_NUM_TIME,           APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_4) } },
  { ID_PROGBAR_00,         APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_00,         APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acfuel1),
                                                         ARG_VP(0, acfuel2), } },
  { ID_PROGBAR_00,         APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_00,         APPW_SET_PROP_COLORS,       { ARG_V(GUI_INVALID_COLOR),
                                                         ARG_V(GUI_INVALID_COLOR),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_VALUE,        { ARG_V(13) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_COLORS,       { ARG_V(0xff0055ff),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_AFR_WARN,   APPW_SET_PROP_RANGE,        { ARG_V(10),
                                                         ARG_V(20) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_VALUE,        { ARG_V(100) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_AFR,        APPW_SET_PROP_RANGE,        { ARG_V(100),
                                                         ARG_V(200) } },
  { ID_NUM_SPEED_CAN,      APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_SPEED_CAN,      APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_NUM_SPEED_CAN,      APPW_SET_PROP_FONT,         { ARG_VP(0, acx14y24pxHeadUpDaisy_36_Bold_EXT) } },
  { ID_NUM_SPEED_CAN,      APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                         ARG_V(0),
                                                         ARG_V(0),
                                                         ARG_V(1) } },
  { ID_TEXT_00,            APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_TEXT_00,            APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_BOTTOM),
                                                         ARG_V(0),
                                                         ARG_V(0) } },
  { ID_TEXT_00,            APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_6) } },
  { ID_TEXT_00,            APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_WT,         APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(120) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(100) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_WT_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(120) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_IAT,        APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(100) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_VALUE,        { ARG_V(60) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_IAT_WARN,   APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(100) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_MAP,        APPW_SET_PROP_RANGE,        { ARG_V(20),
                                                         ARG_V(120) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OT,         APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(150) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(110) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xff323232),
                                                         ARG_V(0xffaa0000),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OT_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(150) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_VALUE,        { ARG_V(200) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OILP,       APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(800) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_VALUE,        { ARG_V(100) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_COLORS,       { ARG_V(0xffaa0000),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_OP_WARN,    APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                         ARG_V(800) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_VALUE,        { ARG_V(10) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_COLORS,       { ARG_V(0xffaaaaaa),
                                                         ARG_V(0xff323232),
                                                         ARG_V(GUI_INVALID_COLOR) } },
  { ID_PROGBAR_BATV,       APPW_SET_PROP_RANGE,        { ARG_V(80),
                                                         ARG_V(160) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_RADIUS,       { ARG_V(0) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_FRAME,        { ARG_V(0) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_COLOR,        { ARG_V(0xff2c2c30) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_VALUE,        { ARG_V(10) } },
  { ID_PROGBAR_BATTV_WARN, APPW_SET_PROP_TILE,         { ARG_V(0) } },
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
*       Comparison(s)
*/
static APPW_COND_COMP _aComparison_12[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 0 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_13[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 1 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_14[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 2 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_15[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 3 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_16[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 4 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_17[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 5 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_18[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 6 } }, APPW__CompareIsEqual },
};

/*********************************************************************
*
*       Condition(s)
*/
static GUI_CONST_STORAGE APPW_COND _Condition_12 = { "A", _aComparison_12, GUI_COUNTOF(_aComparison_12) };
static GUI_CONST_STORAGE APPW_COND _Condition_13 = { "A", _aComparison_13, GUI_COUNTOF(_aComparison_13) };
static GUI_CONST_STORAGE APPW_COND _Condition_14 = { "A", _aComparison_14, GUI_COUNTOF(_aComparison_14) };
static GUI_CONST_STORAGE APPW_COND _Condition_15 = { "A", _aComparison_15, GUI_COUNTOF(_aComparison_15) };
static GUI_CONST_STORAGE APPW_COND _Condition_16 = { "A", _aComparison_16, GUI_COUNTOF(_aComparison_16) };
static GUI_CONST_STORAGE APPW_COND _Condition_17 = { "A", _aComparison_17, GUI_COUNTOF(_aComparison_17) };
static GUI_CONST_STORAGE APPW_COND _Condition_18 = { "A", _aComparison_18, GUI_COUNTOF(_aComparison_18) };

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_BT_01,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_BT_01__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_FL_01,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_FL_01__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_BL_01,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_BL_01__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_AD_01,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_01__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_AD_02,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_02__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_AD_03,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_03__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_NUM_AD_04,          APPW_JOB_SETVIS,         ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG__ID_NUM_AD_04__APPW_JOB_SETVIS,
    { ARG_V(APPW_SET_OFF),
    }, 0, NULL
  },
  { ID_VAR_FPS,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_FPS,            APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FPS__APPW_JOB_SETVALUE,
    { ARG_V(4110),
    }, 65537, NULL
  },
  { ID_VAR_REV_A,          WM_NOTIFICATION_VALUE_CHANGED,    ID_ROTARY_00,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_ROTARY_00__APPW_JOB_SETVALUE,
    { ARG_V(4114),
    }, 65537, NULL
  },
  { ID_VAR_REV,            WM_NOTIFICATION_VALUE_CHANGED,    ID_GAUGE_00,           APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE,
    { ARG_V(4104),
    }, 65537, NULL
  },
  { ID_VAR_AF,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,             APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETVALUE,
    { ARG_V(4103),
    }, 65537, NULL
  },
  { ID_VAR_REV,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_RV,             APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_RV__APPW_JOB_SETVALUE,
    { ARG_V(4104),
    }, 65537, NULL
  },
  { ID_VAR_BATT,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_BT_01,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_BT_01__APPW_JOB_SETVALUE,
    { ARG_V(4108),
    }, 65537, NULL
  },
  { ID_VAR_BL1,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_BL_01,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_BL_01__APPW_JOB_SETVALUE,
    { ARG_V(4107),
    }, 65537, NULL
  },
  { ID_VAR_FL1,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_FL_01,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FL_01__APPW_JOB_SETVALUE,
    { ARG_V(4106),
    }, 65537, NULL
  },
  { ID_VAR_SPEED,          WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_SPEED,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED__APPW_JOB_SETVALUE,
    { ARG_V(4109),
    }, 65537, NULL
  },
  { ID_VAR_ODO,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_ODO,            APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_ODO__APPW_JOB_SETVALUE,
    { ARG_V(4111),
    }, 65537, NULL
  },
  { ID_VAR_TRIP,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_TRIP,           APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_TRIP__APPW_JOB_SETVALUE,
    { ARG_V(4112),
    }, 65537, NULL
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT,
    { ARG_V(ID_RTEXT_GN),
    }, 65536, &_Condition_12
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_0,
    { ARG_V(ID_RTEXT_G1),
    }, 65536, &_Condition_13
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_1,
    { ARG_V(ID_RTEXT_G2),
    }, 65536, &_Condition_14
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_2,
    { ARG_V(ID_RTEXT_G3),
    }, 65536, &_Condition_15
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_3,
    { ARG_V(ID_RTEXT_G4),
    }, 65536, &_Condition_16
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_4,
    { ARG_V(ID_RTEXT_G5),
    }, 65536, &_Condition_17
  },
  { ID_VAR_GEAR,           WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,           APPW_JOB_SETTEXT,        ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_5,
    { ARG_V(ID_RTEXT_G6),
    }, 65536, &_Condition_18
  },
  { ID_SCREEN_02,          APPW_NOTIFICATION_INITDIALOG,     ID_VAR_SW,             APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__APPW_NOTIFICATION_INITDIALOG,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_FUEL,           WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_00,         APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_00__APPW_JOB_SETVALUE,
    { ARG_V(4115),
    }, 65537, NULL
  },
  { ID_VAR_AD1,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AD_01,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_01__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_AD2,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AD_02,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_02__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_AD3,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AD_03,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_03__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_AD4,            WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AD_04,          APPW_JOB_SETVALUE,       ID_SCREEN_01_Copy__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AD_04__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_AFR_BAR,        WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_AFR,        APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_AFR__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_SPEED_CAN,      WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_SPEED_CAN,      APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED_CAN__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_01,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_WT,         APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_WT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_02,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_IAT,        APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_IAT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_03,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_OT,         APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_OT__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_04,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_MAP,        APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_MAP__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_05,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_OILP,       APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_06,             WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_BATV,       APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_BATV__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_01,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_01,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_02,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_02,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_03,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_03,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_04,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_04,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_05,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_05,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE,
    { ARG_V(0),
    }, 0, NULL
  },
  { ID_VAR_06,             WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_06,             APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE,
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
*       ID_SCREEN_02_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_02_RootInfo = {
  ID_SCREEN_02,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  _aAction, GUI_COUNTOF(_aAction),
  cbID_SCREEN_02,
  0
};

/*************************** End of file ****************************/
