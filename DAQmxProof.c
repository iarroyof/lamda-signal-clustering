//==============================================================================
//
// Title:       DAQmxProof
// Purpose:     A short description of the application.
//
// Created on:  01/02/2013 at 11:30:32 a.m. by utm.
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
#include "DAQmxProof.h"
#include "toolbox.h"

#define avWindow 100


//==============================================================================
// Static global variables

static int panelHandle, running = 0;
static TaskHandle daqTask;
static float64 data[avWindow];


int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));	  
	
	DAQmxLoadTask ("pruebaVoltageTask", &daqTask);
	errChk (panelHandle = LoadPanel (0, "DAQmxProof.uir", PANEL));
    
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


int CVICALLBACK funcAdquirir (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(!running)
			{
				DAQmxStartTask (daqTask);
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_DIMMED, 0);
				SetCtrlAttribute (panelHandle, PANEL_STOP, ATTR_DIMMED, 0);
				SetCtrlAttribute (panelHandle, PANEL_ADQUIRIR, ATTR_DIMMED, 1);
				running = 1;
			}
			break;
	}
	return 0;
}

int CVICALLBACK stopBoton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(running)
			{
				running = 0;
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute (panelHandle, PANEL_STOP, ATTR_DIMMED, 1);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_DIMMED, 1);
				DAQmxClearTask(daqTask);
			}
			break;
	}
	return 0;
}

int CVICALLBACK drawingByTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double prom[avWindow], av = 0, espectro[avWindow/2], fase[avWindow/2];

	switch (event)
	{
		case EVENT_TIMER_TICK:
			DAQmxReadAnalogF64 (daqTask, avWindow, 10.0, DAQmx_Val_GroupByScanNumber, data, avWindow, NULL, 0);
			PlotStripChart (panelHandle, PANEL_STRIPCHART, data, avWindow, 0, 0, VAL_DOUBLE); //GRafica la señal de entrada
	//******Procesamiento de la señal de entrada para obtener su promedio (filtro pasabajas)***************
			Mean(data, avWindow, &av);			
			for(int i = 0; i < avWindow; i++)	//avWindow puede establecerse como un parámetro de la interfaz del programa
			{									//que puede usarse en el program para reservar mmoria en tiempo de ejecución 
				prom[i] = av;					//de modo que se pueda modificar la frecuencia de corte del filtro, misma
			}									//que estaría relacionada con el tamaño de la ventana que se promedia (avWindow)..
			PlotStripChart (panelHandle, PANEL_STRIPCHART_2, prom, avWindow, 0, 0, VAL_DOUBLE);  //Grafica de la señal promediada (filtrada)
	//***********************************************************************************		

			break;
	}
	return 0;
}

int CVICALLBACK botonPausa (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int timerEN;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, &timerEN);
			if(running)
			{
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_LABEL_TEXT, "__PLAY");
				running = 0;
			}
			else{ 
				if(!running && timerEN == 0)
				{
					SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					SetCtrlAttribute (panelHandle, PANEL_PAUSA, ATTR_LABEL_TEXT, "__PAUSA");
					running = 1;
				}
			}
			break;
	}
	return 0;
}
