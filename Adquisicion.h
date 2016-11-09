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

#define  HELP                             1
#define  HELP_TEXTBOX                     2       /* control type: textBox, callback function: (none) */
#define  HELP_OK                          3       /* control type: command, callback function: ok */

#define  MAINPANEL                        2       /* callback function: panelCB */
#define  MAINPANEL_DECORATION             2       /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_2           3       /* control type: deco, callback function: (none) */
#define  MAINPANEL_HYPERLINK              4       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_DECORATION_3           5       /* control type: deco, callback function: (none) */
#define  MAINPANEL_HYPERLINK_2            6       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_ACQUIRE                7       /* control type: command, callback function: AquireData */
#define  MAINPANEL_RINGSLIDE              8       /* control type: slide, callback function: (none) */
#define  MAINPANEL_ANALYZE                9       /* control type: command, callback function: AnalyzeData */
#define  MAINPANEL_ORIGINALGRAPH          10      /* control type: graph, callback function: (none) */
#define  MAINPANEL_ANALYZEDGRAPH          11      /* control type: graph, callback function: (none) */
#define  MAINPANEL_HELP                   12      /* control type: command, callback function: Help */
#define  MAINPANEL_NUMBERPOINTS           13      /* control type: ring, callback function: (none) */
#define  MAINPANEL_HYPERLINK_3            14      /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_STATUS                 15      /* control type: textBox, callback function: (none) */
#define  MAINPANEL_URL                    16      /* control type: string, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AnalyzeData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AquireData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Help(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ok(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
