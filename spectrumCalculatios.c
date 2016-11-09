//==============================================================================
//
// Title:       spectrumCalculatios
// Purpose:     A short description of the application.
//
// Created on:  07/02/2013 at 04:35:57 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <NIDAQmx.h>
#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "spectrumCalculatios.h"
#include "toolbox.h"

//==============================================================================
// Constants
#define NB_PTS      512
#define SAMPLING    0.000004  //seconds
#define CENTER_FREQ 23.0E3
#define WIDTH_FREQ  5.00E3
//==============================================================================
// Types
SearchType span;      
//==============================================================================
// Static global variables
static int panelHandle;
static double SignalArray[NB_PTS]; 
static TaskHandle daqTask;    // ESTA VARIABLE SIEMPRE DEBE SER DE TIPO static
// Static functions
//==============================================================================
// Global variables
int running = 0;
 
/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "spectrumCalculatios.uir", PANEL));
    DAQmxLoadTask ("Tasker", &daqTask); 
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());
	
Error:
    /* clean up */
    DiscardPanel (panelHandle);
    return 0;
}
//==============================================================================
// UI callback function prototypes.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int CVICALLBACK drawingTimerCalculation (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    double  f0, spectrum[NB_PTS], frec, fase, amp, signal[NB_PTS], exSP = 0, res; 
	span.centerFrequency = CENTER_FREQ;
	span.frequencyWidth = WIDTH_FREQ;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if(running)
			{
				DAQmxReadAnalogF64 (daqTask, NB_PTS, 10.0, DAQmx_Val_GroupByScanNumber, SignalArray, NB_PTS, NULL, 0);
				SingleToneSignal (SignalArray, NB_PTS, SAMPLING, &span, TONE_EXPORT_NONE, 
				 				&frec, &amp, &fase, signal, &exSP, spectrum, &f0, &res);
				SetCtrlVal (panelHandle, PANEL_FRECUENCIA, frec);
				SetCtrlVal (panelHandle, PANEL_FASE, fase);
				SetCtrlVal (panelHandle, PANEL_AMPLITUD, amp);
			}
			break;
	}
	return 0;
}

int CVICALLBACK cargaTareaStartButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(!running) 
			{
				DAQmxStartTask (daqTask);
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				running = 1;
			}
			break;
	}
	return 0;
}

int CVICALLBACK pausarTareaPausaButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(running)
			{
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_LABEL_TEXT, "__PLAY");
				running =0;
			}
			else
			{
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_LABEL_TEXT, "__PAUSA");
				running = 1;
			}
			break;
	}
	return 0;
}

int CVICALLBACK abortarTareaStopButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(!running)
				DAQmxStopTask (daqTask);
			break;
	}
	return 0;
}
