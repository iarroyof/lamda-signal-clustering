//==============================================================================
//
// Title:       pruebaDAQ
// Purpose:     A short description of the application.
//
// Created on:  31/01/2013 at 07:57:04 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "pruebaDAQ.h"
#include "toolbox.h"
#include "NIDAQmx.h"
#include "pruebaVoltageTaskExampleCode.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;
static int32 samplesPerChannel;
static float64 data[1000];

static TaskHandle daqTask;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "pruebaDAQ.uir", PANEL));
    
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());
	
Error:
    /* clean up */
    DiscardPanel (panelHandle);
    return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int CVICALLBACK acquiringFunc (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Llamando a la funcion que carga la DAQtask:
			DAQmxLoadTask ("voltageTask", &daqTask);
			RunpruebaVoltageTask (daqTask);
		break;
	}
	return 0;
}
