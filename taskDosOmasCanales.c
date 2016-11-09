//==============================================================================
//
// Title:       taskDosOmasCanales
// Purpose:     A short description of the application.
//
// Created on:  11/02/2013 at 12:12:20 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <analysis.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "taskDosOmasCanales.h"
#include "toolbox.h"

//==============================================================================
// Constants
#define samplesPerChannel 100
//#define NB_PTS      512
#define SAMPLING    1.2E-5  //seconds
#define CENTER_FREQ 23.0E3
#define WIDTH_FREQ  5.00E3
//==============================================================================
// Types
SearchType span; 
//==============================================================================
// Static global variables

static int panelHandle;
static TaskHandle daqTask;

//==============================================================================
// Static functions

//==============================================================================
// Global variables
uInt32 running = 0, numChannels, dataSize;
static float64 *data, signalArray[samplesPerChannel];  

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "taskDosOmasCanales.uir", PANEL));
	DAQmxLoadTask ("voltageTask", &daqTask);
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

int CVICALLBACK timerProcesingFunction (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	
	double frec, amp, fase, signal[samplesPerChannel], exSP, spectrum[samplesPerChannel], f0, res;
	switch (event)
	{
		int j, i;
		case EVENT_TIMER_TICK:
			DAQmxReadAnalogF64 (daqTask, samplesPerChannel, 10.0, DAQmx_Val_GroupByScanNumber, data, dataSize, NULL, 0);
			//Existe la necesidad de extraer primero cada canal para hacerle el analisis correspondiente. 
			//No es posible pasar *data completo ya que las muestras de cada canal estan intercaladas en *data de
			//la siguiente forma:
//data = {chan_1[0], chan_2[0], chan_3[0], chan_1[1], chan_2[1], chan_3[1],..., chan_1[samplesPerChannel-1], chan_2[samplesPerChannel-1], chan_3[samplesPerChannel-1]}
			//donde chan_i[j] el nivel de voltaje de la j-esima muestra del i-esimo canal de entrada.
			for(i = 0, j = 0; j < samplesPerChannel; i+=3, j++)
				signalArray[j] = data[i];
			//Solo se ocupará frec. Las demas variables se han colocado para no tener errores en la funcion:	
			SingleToneSignal (signalArray, samplesPerChannel, SAMPLING, &span, TONE_EXPORT_NONE, 
				 				&frec, &amp, &fase, signal, &exSP, spectrum, &f0, &res);
			SetCtrlVal (panelHandle, PANEL_F1, frec);
			SetCtrlVal (panelHandle, PANEL_V1, data[1]);
			SetCtrlVal (panelHandle, PANEL_V2, data[2]);
			
			break;
	}
	return 0;
}

int CVICALLBACK startAquiringButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(!running)
			{
				DAQmxStartTask (daqTask);
				DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &numChannels);
				dataSize = numChannels * samplesPerChannel;
				data = (float64 *) malloc(dataSize * sizeof(float64));
				SetCtrlVal (panelHandle, PANEL_CHANNS, (double)numChannels);
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				span.centerFrequency = CENTER_FREQ;
				span.frequencyWidth = WIDTH_FREQ;
				running = 1;
			}
			break;
	}
	return 0;
}


int CVICALLBACK functionStopButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(running)
			{
				DAQmxStopTask (daqTask);
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0); 
				running = 0;
			}
			break;
	}
	return 0;
}

int CVICALLBACK functionPausaButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(running)
			{
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_LABEL_TEXT, "__PLAY");
				running = 0;
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



