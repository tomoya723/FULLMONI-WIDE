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
File        : ID_SCREEN_Telltale.c
Purpose     : Generated file do NOT edit!
---------------------------END-OF-HEADER------------------------------
*/

#include "Resource.h"
#include "ID_SCREEN_Telltale.h"

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
    ID_SCREEN_Telltale, 0,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 765, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
      },
      0, 0, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_IMAGE_00, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      35, 256, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_00, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 10, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_01, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 40, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_02L, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 70, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_02H, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 70, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_03, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 100, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_04, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 130, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_05, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 160, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_06, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 190, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
  { WM_OBJECT_IMAGE_Create,
    ID_ICON_07, ID_SCREEN_Telltale,
    { { { DISPOSE_MODE_REL_PARENT, 5, 0, 0 },
        { DISPOSE_MODE_REL_PARENT, 220, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
        { DISPOSE_MODE_NULL, 0, 0, 0 },
      },
      25, 25, 0, 0, 0, 0
    },
    { 0, 0 }
  },
};

/*********************************************************************
*
*       _aSetup
*/
static GUI_CONST_STORAGE APPW_SETUP_ITEM _aSetup[] = {
  { ID_IMAGE_00,        APPW_SET_PROP_TILE,         { ARG_V(0) } },
  { ID_IMAGE_00,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acw_back),
                                                      ARG_V(156), } },
  { ID_IMAGE_00,        APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_00,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_masterwarning),
                                                      ARG_V(595), } },
  { ID_ICON_00,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_01,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_oilpresswarning),
                                                      ARG_V(614), } },
  { ID_ICON_01,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_02L,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_watarcool),
                                                      ARG_V(1048), } },
  { ID_ICON_02L,        APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_02H,        APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_waterwarning),
                                                      ARG_V(837), } },
  { ID_ICON_02H,        APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_03,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_exhaustwarning),
                                                      ARG_V(791), } },
  { ID_ICON_03,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_04,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_batterywarning),
                                                      ARG_V(712), } },
  { ID_ICON_04,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_05,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_breakwarning),
                                                      ARG_V(1016), } },
  { ID_ICON_05,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_06,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_beltwarning),
                                                      ARG_V(797), } },
  { ID_ICON_06,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
  { ID_ICON_07,         APPW_SET_PROP_SBITMAP,      { ARG_VP(0, acws_fuelcheck),
                                                      ARG_V(1098), } },
  { ID_ICON_07,         APPW_SET_PROP_LQ,           { ARG_V(0) } },
};

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       ID_SCREEN_Telltale_RootInfo
*/
APPW_ROOT_INFO ID_SCREEN_Telltale_RootInfo = {
  ID_SCREEN_Telltale,
  _aCreate, GUI_COUNTOF(_aCreate),
  _aSetup,  GUI_COUNTOF(_aSetup),
  NULL,  0,
  cbID_SCREEN_Telltale,
  0
};

/*************************** End of file ****************************/
