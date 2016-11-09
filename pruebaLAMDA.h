/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2013. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: panelCB */
#define  PANEL_APRENDER                   2       /* control type: command, callback function: aprenderFunctionButton */
#define  PANEL_DIAG                       3       /* control type: command, callback function: diagFunctionButton */
#define  PANEL_CLASES_READED              4       /* control type: numeric, callback function: (none) */
#define  PANEL_ALPHA                      5       /* control type: numeric, callback function: (none) */
#define  PANEL_CADENACLASES               6       /* control type: string, callback function: (none) */
#define  PANEL_CLASES_STORED              7       /* control type: string, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK aprenderFunctionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK diagFunctionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
