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
#define  PANEL_ADQUIRIR                   2       /* control type: command, callback function: funcAdquirir */
#define  PANEL_STOP                       3       /* control type: command, callback function: stopBoton */
#define  PANEL_TIMER                      4       /* control type: timer, callback function: drawingByTimer */
#define  PANEL_PAUSA                      5       /* control type: command, callback function: botonPausa */
#define  PANEL_STRIPCHART_2               6       /* control type: strip, callback function: (none) */
#define  PANEL_STRIPCHART_3               7       /* control type: strip, callback function: (none) */
#define  PANEL_STRIPCHART                 8       /* control type: strip, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK botonPausa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK drawingByTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK funcAdquirir(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopBoton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
