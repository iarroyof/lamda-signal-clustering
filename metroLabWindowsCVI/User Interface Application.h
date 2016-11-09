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
#define  PANEL_APRENDER_FROM_FILE         2       /* control type: command, callback function: aprenderArchivoButton */
#define  PANEL_LAMBDA                     3       /* control type: numeric, callback function: (none) */
#define  PANEL_CAJON                      4       /* control type: ring, callback function: (none) */
#define  PANEL_MODO                       5       /* control type: slide, callback function: (none) */
#define  PANEL_TIMER                      6       /* control type: timer, callback function: realTimeR_Recognition */
#define  PANEL_TASKID                     7       /* control type: string, callback function: (none) */
#define  PANEL_LED_SUJETOS_LOADED         8       /* control type: LED, callback function: (none) */
#define  PANEL_LED_CLASES_FILE            9       /* control type: LED, callback function: (none) */
#define  PANEL_COLORNUM                   10      /* control type: color, callback function: (none) */
#define  PANEL_TOGGLE_ONLINE_LEARN        11      /* control type: textButton, callback function: onLineLearning */
#define  PANEL_CLASES_FILE                12      /* control type: string, callback function: (none) */
#define  PANEL_VIEW_CHANN                 13      /* control type: numeric, callback function: (none) */
#define  PANEL_FREC                       14      /* control type: numeric, callback function: (none) */
#define  PANEL_TOGGLE_DIAGNOSIS           15      /* control type: textButton, callback function: onlineDiagnosisButton */
#define  PANEL_SELECT_FILE                16      /* control type: command, callback function: fileSelectionButton */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK aprenderArchivoButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK fileSelectionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onlineDiagnosisButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onLineLearning(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK realTimeR_Recognition(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
