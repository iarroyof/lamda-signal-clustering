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
#define  PANEL_DIAGNOSTICAR               2       /* control type: command, callback function: diagnosisButton */
#define  PANEL_APRENDER_FROM_FILE         3       /* control type: command, callback function: aprenderArchivoButton */
#define  PANEL_APRENDER_EN_LINEA          4       /* control type: command, callback function: aprenderEnLineaButton */
#define  PANEL_STOP                       5       /* control type: command, callback function: stopButton */
#define  PANEL_LAMBDA                     6       /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER                      7       /* control type: timer, callback function: realTimeR_Recognition */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK aprenderArchivoButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK aprenderEnLineaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK diagnosisButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK realTimeR_Recognition(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
