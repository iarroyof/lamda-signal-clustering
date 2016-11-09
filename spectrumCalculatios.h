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
#define  PANEL_AMPLITUD                   2       /* control type: numeric, callback function: (none) */
#define  PANEL_FASE                       3       /* control type: numeric, callback function: (none) */
#define  PANEL_FRECUENCIA                 4       /* control type: numeric, callback function: (none) */
#define  PANEL_START                      5       /* control type: command, callback function: cargaTareaStartButton */
#define  PANEL_PAUSA                      6       /* control type: command, callback function: pausarTareaPausaButton */
#define  PANEL_STOP                       7       /* control type: command, callback function: abortarTareaStopButton */
#define  PANEL_TIMER                      8       /* control type: timer, callback function: drawingTimerCalculation */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK abortarTareaStopButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cargaTareaStartButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK drawingTimerCalculation(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK pausarTareaPausaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
