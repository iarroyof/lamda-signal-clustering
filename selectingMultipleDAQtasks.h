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
#define  PANEL_CAJON                      2       /* control type: ring, callback function: (none) */
#define  PANEL_MODO                       3       /* control type: slide, callback function: (none) */
#define  PANEL_PATYPE                     4       /* control type: slide, callback function: (none) */
#define  PANEL_PROBAR                     5       /* control type: command, callback function: probarFunctionButton */
#define  PANEL_TASKID                     6       /* control type: string, callback function: (none) */
#define  PANEL_TIMER                      7       /* control type: timer, callback function: timerProcessingProfile_0 */
#define  PANEL_AMPL                       8       /* control type: numeric, callback function: (none) */
#define  PANEL_FREC                       9       /* control type: numeric, callback function: (none) */
#define  PANEL_LOADING                    10      /* control type: command, callback function: cargarArchivoFunctionButton */
#define  PANEL_DECORATION                 11      /* control type: deco, callback function: (none) */
#define  PANEL_STRIPCHART                 12      /* control type: strip, callback function: (none) */
#define  PANEL_TRAIN                      13      /* control type: command, callback function: entrenarFunctionButton */
#define  PANEL_TEXTMSG                    14      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_2               15      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_2                  16      /* control type: textMsg, callback function: (none) */
#define  PANEL_LOADEDFILE                 17      /* control type: string, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK cargarArchivoFunctionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK entrenarFunctionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK probarFunctionButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerProcessingProfile_0(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
