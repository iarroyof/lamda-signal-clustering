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
#define  PANEL_START                      2       /* control type: command, callback function: startAquiringButton */
#define  PANEL_PAUSA                      3       /* control type: command, callback function: functionPausaButton */
#define  PANEL_STOP                       4       /* control type: command, callback function: functionStopButton */
#define  PANEL_V2                         5       /* control type: numeric, callback function: (none) */
#define  PANEL_V1                         6       /* control type: numeric, callback function: (none) */
#define  PANEL_CHANNS                     7       /* control type: numeric, callback function: (none) */
#define  PANEL_F1                         8       /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER                      9       /* control type: timer, callback function: timerProcesingFunction */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK functionPausaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK functionStopButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK startAquiringButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerProcesingFunction(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
