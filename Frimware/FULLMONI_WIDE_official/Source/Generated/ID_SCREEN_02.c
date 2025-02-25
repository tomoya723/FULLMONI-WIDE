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
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      800, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 102, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 25, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      594, 60, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 3, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 20, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      794, 70, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_FPS, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 683, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      73, 17, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N01, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 30, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N02, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N03, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 290, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N04, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 420, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N05, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 550, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_BOX_Create,
    ID_BOX_N06, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 680, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 30, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_02, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_03, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 290, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_04, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 420, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 185, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_05, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 550, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 185, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_06, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 680, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 185, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      94, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_AF, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 62, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 120, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      210, 50, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_SPEED, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 545, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 120, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      150, 50, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_GEAR, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 350, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 90, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 100, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_TRIP, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 230, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 648, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      50, 20, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_NUM_ODO, ID_SCREEN_02,
    { { { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 234, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 361, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      60, 20, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_PROGBAR_Create,
    ID_PROGBAR_00, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 647, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 234, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      100, 15, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_01, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 30, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_02, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_03, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 290, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_04, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 420, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_05, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 550, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_TEXT_Create,
    ID_TEXT_06, ID_SCREEN_02,
    { { { DISPOSE_MODE_REL_PARENT, 680, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 186, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 35, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_01,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acBack3),
                                                 ARG_V(614454), } },
  { ID_IMAGE_01,   APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_PROGBAR_01, APPW_SET_PROP_RANGE,        { ARG_V(0),
                                                 ARG_V(9000) } },
  { ID_PROGBAR_01, APPW_SET_PROP_VALUE,        { ARG_V(1000) } },
  { ID_PROGBAR_01, APPW_SET_PROP_COLORS,       { ARG_V(0xffc8c8c8),
                                                 ARG_V(GUI_BLACK),
                                                 ARG_V(GUI_INVALID_COLOR) } },
  { ID_IMAGE_00,   APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acrev),
                                                 ARG_V(4095), } },
  { ID_IMAGE_00,   APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_NUM_FPS,    APPW_SET_PROP_COLOR,        { ARG_V(0xffc0c0c0) } },
  { ID_NUM_FPS,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_FPS,    APPW_SET_PROP_FONT,         { ARG_VP(0, acSuiGenerisRg_16_Normal_EXT_AA4) } },
  { ID_NUM_FPS,    APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_BOX_N01,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_BOX_N02,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_BOX_N03,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_BOX_N04,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_BOX_N05,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_BOX_N06,    APPW_SET_PROP_COLOR,        { ARG_V(0xff4b4b4b) } },
  { ID_NUM_01,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_01,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_01,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_01,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_02,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_02,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_02,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_02,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_03,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_03,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_03,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_03,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_04,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_04,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_04,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_04,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_05,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_05,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_05,     APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_05,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_06,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_06,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(4294967294),
                                                 ARG_V(0) } },
  { ID_NUM_06,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_06,     APPW_SET_PROP_FONT,         { ARG_VP(0, acArialBlack_30_Black_EXT) } },
  { ID_NUM_AF,     APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_AF,     APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_AF,     APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_60_Normal_EXT_AA4) } },
  { ID_NUM_AF,     APPW_SET_PROP_DECMODE,      { ARG_V(5),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_FONT,         { ARG_VP(0, ac51_60_Normal_EXT_AA4) } },
  { ID_NUM_SPEED,  APPW_SET_PROP_DECMODE,      { ARG_V(3),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_HCENTER | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_GN) } },
  { ID_NUM_GEAR,   APPW_SET_PROP_FONT,         { ARG_VP(0, acSuiGenerisRg_120_Bold_EXT_AA4) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT_AA4) } },
  { ID_NUM_TRIP,   APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                 ARG_V(1),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_NUM_ODO,    APPW_SET_PROP_COLOR,        { ARG_V(0xffc8c8c8) } },
  { ID_NUM_ODO,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER),
                                                 ARG_V(0),
                                                 ARG_V(0) } },
  { ID_NUM_ODO,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_16_Bold_EXT_AA4) } },
  { ID_NUM_ODO,    APPW_SET_PROP_DECMODE,      { ARG_V(6),
                                                 ARG_V(0),
                                                 ARG_V(0),
                                                 ARG_V(1) } },
  { ID_PROGBAR_00, APPW_SET_PROP_VALUE,        { ARG_V(40) } },
  { ID_PROGBAR_00, APPW_SET_PROP_SBITMAPS,     { ARG_VP(0, acfuel1),
                                                 ARG_VP(0, acfuel2), } },
  { ID_PROGBAR_00, APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_TEXT_01,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_01,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_01,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T1) } },
  { ID_TEXT_01,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_TEXT_02,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_02,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_02,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T2) } },
  { ID_TEXT_02,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_TEXT_03,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_03,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_03,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T3) } },
  { ID_TEXT_03,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_TEXT_04,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_04,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_04,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T4) } },
  { ID_TEXT_04,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_TEXT_05,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_05,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_05,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T5) } },
  { ID_TEXT_05,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
  { ID_TEXT_06,    APPW_SET_PROP_COLOR,        { ARG_V(GUI_WHITE) } },
  { ID_TEXT_06,    APPW_SET_PROP_ALIGNTEXT,    { ARG_V(GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER),
                                                 ARG_V(1),
                                                 ARG_V(0) } },
  { ID_TEXT_06,    APPW_SET_PROP_TEXTID,       { ARG_V(ID_RTEXT_T6) } },
  { ID_TEXT_06,    APPW_SET_PROP_FONT,         { ARG_VP(0, acArial_19_Normal_EXT) } },
};

/*********************************************************************
*
*       Comparison(s)
*/
static APPW_COND_COMP _aComparison_0c[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 0 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0d[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 1 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0e[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 2 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_0f[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 3 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_10[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 4 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_11[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 5 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_12[] = {
  { { { APPW_IS_VAR, ID_VAR_GEAR }, { APPW_IS_VAL, 6 } }, APPW__CompareIsEqual },
};

static APPW_COND_COMP _aComparison_13[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 500 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_14[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 500 } }, APPW__CompareIsGreaterOrEqual },
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 800 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_15[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 800 } }, APPW__CompareIsGreaterOrEqual },
};

static APPW_COND_COMP _aComparison_16[] = {
  { { { APPW_IS_VAR, ID_VAR_02 }, { APPW_IS_VAL, 800 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_17[] = {
  { { { APPW_IS_VAR, ID_VAR_02 }, { APPW_IS_VAL, 800 } }, APPW__CompareIsGreaterOrEqual },
};

static APPW_COND_COMP _aComparison_18[] = {
  { { { APPW_IS_VAR, ID_VAR_03 }, { APPW_IS_VAL, 80 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_19[] = {
  { { { APPW_IS_VAR, ID_VAR_03 }, { APPW_IS_VAL, 800 } }, APPW__CompareIsGreaterOrEqual },
  { { { APPW_IS_VAR, ID_VAR_03 }, { APPW_IS_VAL, 1100 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_1a[] = {
  { { { APPW_IS_VAR, ID_VAR_01 }, { APPW_IS_VAL, 1100 } }, APPW__CompareIsGreaterOrEqual },
};

static APPW_COND_COMP _aComparison_1b[] = {
  { { { APPW_IS_VAR, ID_VAR_05 }, { APPW_IS_VAL, 100 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_1c[] = {
  { { { APPW_IS_VAR, ID_VAR_05 }, { APPW_IS_VAL, 100 } }, APPW__CompareIsGreaterOrEqual },
};

static APPW_COND_COMP _aComparison_1d[] = {
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 120 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_1e[] = {
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 120 } }, APPW__CompareIsGreaterOrEqual },
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 140 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_1f[] = {
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 140 } }, APPW__CompareIsGreaterOrEqual },
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 150 } }, APPW__CompareIsLess },
};

static APPW_COND_COMP _aComparison_20[] = {
  { { { APPW_IS_VAR, ID_VAR_AF }, { APPW_IS_VAL, 150 } }, APPW__CompareIsGreaterOrEqual },
};

/*********************************************************************
*
*       Condition(s)
*/
static GUI_CONST_STORAGE APPW_COND _Condition_0c = { "A", _aComparison_0c, GUI_COUNTOF(_aComparison_0c) };
static GUI_CONST_STORAGE APPW_COND _Condition_0d = { "A", _aComparison_0d, GUI_COUNTOF(_aComparison_0d) };
static GUI_CONST_STORAGE APPW_COND _Condition_0e = { "A", _aComparison_0e, GUI_COUNTOF(_aComparison_0e) };
static GUI_CONST_STORAGE APPW_COND _Condition_0f = { "A", _aComparison_0f, GUI_COUNTOF(_aComparison_0f) };
static GUI_CONST_STORAGE APPW_COND _Condition_10 = { "A", _aComparison_10, GUI_COUNTOF(_aComparison_10) };
static GUI_CONST_STORAGE APPW_COND _Condition_11 = { "A", _aComparison_11, GUI_COUNTOF(_aComparison_11) };
static GUI_CONST_STORAGE APPW_COND _Condition_12 = { "A", _aComparison_12, GUI_COUNTOF(_aComparison_12) };
static GUI_CONST_STORAGE APPW_COND _Condition_13 = { "A", _aComparison_13, GUI_COUNTOF(_aComparison_13) };
static GUI_CONST_STORAGE APPW_COND _Condition_14 = { "A", _aComparison_14, GUI_COUNTOF(_aComparison_14) };
static GUI_CONST_STORAGE APPW_COND _Condition_15 = { "A", _aComparison_15, GUI_COUNTOF(_aComparison_15) };
static GUI_CONST_STORAGE APPW_COND _Condition_16 = { "A", _aComparison_16, GUI_COUNTOF(_aComparison_16) };
static GUI_CONST_STORAGE APPW_COND _Condition_17 = { "A", _aComparison_17, GUI_COUNTOF(_aComparison_17) };
static GUI_CONST_STORAGE APPW_COND _Condition_18 = { "A", _aComparison_18, GUI_COUNTOF(_aComparison_18) };
static GUI_CONST_STORAGE APPW_COND _Condition_19 = { "A&B", _aComparison_19, GUI_COUNTOF(_aComparison_19) };
static GUI_CONST_STORAGE APPW_COND _Condition_1a = { "A", _aComparison_1a, GUI_COUNTOF(_aComparison_1a) };
static GUI_CONST_STORAGE APPW_COND _Condition_1b = { "A", _aComparison_1b, GUI_COUNTOF(_aComparison_1b) };
static GUI_CONST_STORAGE APPW_COND _Condition_1c = { "A", _aComparison_1c, GUI_COUNTOF(_aComparison_1c) };
static GUI_CONST_STORAGE APPW_COND _Condition_1d = { "A", _aComparison_1d, GUI_COUNTOF(_aComparison_1d) };
static GUI_CONST_STORAGE APPW_COND _Condition_1e = { "A&B", _aComparison_1e, GUI_COUNTOF(_aComparison_1e) };
static GUI_CONST_STORAGE APPW_COND _Condition_1f = { "A&B", _aComparison_1f, GUI_COUNTOF(_aComparison_1f) };
static GUI_CONST_STORAGE APPW_COND _Condition_20 = { "A", _aComparison_20, GUI_COUNTOF(_aComparison_20) };

/*********************************************************************
*
*       _aAction
*/
static GUI_CONST_STORAGE APPW_ACTION_ITEM _aAction[] = {
  { ID_VAR_FPS,    WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_FPS,    APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_FPS__APPW_JOB_SETVALUE,
    { ARG_V(4110),
    }, 65537, NULL
  },
  { ID_VAR_REV,    WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_01, APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_GAUGE_00__APPW_JOB_SETVALUE,
    { ARG_V(4104),
    }, 65537, NULL
  },
  { ID_VAR_01,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_01,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_01__APPW_JOB_SETVALUE,
    { ARG_V(4097),
    }, 65537, NULL
  },
  { ID_VAR_02,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_02,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_02__APPW_JOB_SETVALUE,
    { ARG_V(4098),
    }, 65537, NULL
  },
  { ID_VAR_03,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_03,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_03__APPW_JOB_SETVALUE,
    { ARG_V(4099),
    }, 65537, NULL
  },
  { ID_VAR_04,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_04,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_04__APPW_JOB_SETVALUE,
    { ARG_V(4100),
    }, 65537, NULL
  },
  { ID_VAR_05,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_05,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_05__APPW_JOB_SETVALUE,
    { ARG_V(4101),
    }, 65537, NULL
  },
  { ID_VAR_06,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_06,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_06__APPW_JOB_SETVALUE,
    { ARG_V(4102),
    }, 65537, NULL
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETVALUE,
    { ARG_V(4103),
    }, 65537, NULL
  },
  { ID_VAR_SPEED,  WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_SPEED,  APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_SPEED__APPW_JOB_SETVALUE,
    { ARG_V(4109),
    }, 65537, NULL
  },
  { ID_VAR_ODO,    WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_ODO,    APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_ODO__APPW_JOB_SETVALUE,
    { ARG_V(4111),
    }, 65537, NULL
  },
  { ID_VAR_TRIP,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_TRIP,   APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_TRIP__APPW_JOB_SETVALUE,
    { ARG_V(4112),
    }, 65537, NULL
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT,
    { ARG_V(ID_RTEXT_GN),
    }, 0, &_Condition_0c
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_0,
    { ARG_V(ID_RTEXT_G1),
    }, 65536, &_Condition_0d
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_1,
    { ARG_V(ID_RTEXT_G2),
    }, 65536, &_Condition_0e
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_2,
    { ARG_V(ID_RTEXT_G3),
    }, 65536, &_Condition_0f
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_3,
    { ARG_V(ID_RTEXT_G4),
    }, 65536, &_Condition_10
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_4,
    { ARG_V(ID_RTEXT_G5),
    }, 65536, &_Condition_11
  },
  { ID_VAR_GEAR,   WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_GEAR,   APPW_JOB_SETTEXT,        ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_GEAR__APPW_JOB_SETTEXT_5,
    { ARG_V(ID_RTEXT_G6),
    }, 65536, &_Condition_12
  },
  { ID_VAR_01,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N01,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_0,
    { ARG_V(GUI_BLUE),
    }, 65536, &_Condition_13
  },
  { ID_VAR_01,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N01,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_1,
    { ARG_V(0xff4b4b4b),
    }, 65536, &_Condition_14
  },
  { ID_VAR_01,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N01,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N01__APPW_JOB_SETCOLOR_2,
    { ARG_V(GUI_RED),
    }, 65536, &_Condition_15
  },
  { ID_VAR_02,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N02,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N02__APPW_JOB_SETCOLOR,
    { ARG_V(0xff4b4b4b),
    }, 65536, &_Condition_16
  },
  { ID_VAR_02,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N02,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N02__APPW_JOB_SETCOLOR_0,
    { ARG_V(GUI_RED),
    }, 65536, &_Condition_17
  },
  { ID_VAR_03,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N03,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR,
    { ARG_V(GUI_BLUE),
    }, 65536, &_Condition_18
  },
  { ID_VAR_03,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N03,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR_0,
    { ARG_V(0xff4b4b4b),
    }, 65536, &_Condition_19
  },
  { ID_VAR_03,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N03,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N03__APPW_JOB_SETCOLOR_1,
    { ARG_V(GUI_RED),
    }, 65536, &_Condition_1a
  },
  { ID_VAR_05,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N05,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N05__APPW_JOB_SETCOLOR,
    { ARG_V(GUI_RED),
    }, 65536, &_Condition_1b
  },
  { ID_VAR_05,     WM_NOTIFICATION_VALUE_CHANGED,    ID_BOX_N05,    APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_BOX_N05__APPW_JOB_SETCOLOR_0,
    { ARG_V(0xff4b4b4b),
    }, 65536, &_Condition_1c
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_0,
    { ARG_V(GUI_RED),
    }, 65536, &_Condition_1d
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_1,
    { ARG_V(0xffffaa00),
    }, 65536, &_Condition_1e
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_2,
    { ARG_V(0xff00aa00),
    }, 65536, &_Condition_1f
  },
  { ID_VAR_AF,     WM_NOTIFICATION_VALUE_CHANGED,    ID_NUM_AF,     APPW_JOB_SETCOLOR,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_NUM_AF__APPW_JOB_SETCOLOR_3,
    { ARG_V(0xff00aaff),
    }, 65536, &_Condition_20
  },
  { ID_VAR_SW,     WM_NOTIFICATION_VALUE_CHANGED,    0,             APPW_JOB_SWAPSCREEN,     ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED,
    { ARG_V(ID_SCREEN_01),
    }, 0, NULL
  },
  { ID_VAR_FUEL,   WM_NOTIFICATION_VALUE_CHANGED,    ID_PROGBAR_00, APPW_JOB_SETVALUE,       ID_SCREEN_02__WM_NOTIFICATION_VALUE_CHANGED__ID_PROGBAR_00__APPW_JOB_SETVALUE,
    { ARG_V(4115),
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
