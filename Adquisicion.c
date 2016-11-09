#include "hyperlinkctrl.h"
#include <cvirte.h>		
#include <userint.h>
#include "Adquisicion.h"
#include <analysis.h>
#include <ansi_c.h>
#include <cviauto.h>
#include "dataskt.h"

#define TRUE 1
#define FALSE 0
#define MAX_POINTS 2048

static int panelHandle, handle, numpoints = 16;
static double array[MAX_POINTS], oldarray[MAX_POINTS];
static DSHandle dsData;

void dsDataCallback(DSHandle dsHandle, int event, void* callbackData)
{
	if(event == DS_EVENT_STATUSUPDATED){
		HRESULT hr = NOERROR;
		char message[1000];
		hr = DS_GetLastMessage(dsHandle, message, 100);
		if(SUCCEEDED(hr))
			ResetTextBox(panelHandle, MAINPANEL_STATUS, message);
	}
}

int main (int argc, char *argv[])
{   const char *url;
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Adquisicion.uir", MAINPANEL)) < 0)
		return -1;
    NewHyperLinkCtrl (panelHandle, MAINPANEL_HYPERLINK_3, 0);
    NewHyperLinkCtrl (panelHandle, MAINPANEL_HYPERLINK_2, 0);
    NewHyperLinkCtrl (panelHandle, MAINPANEL_HYPERLINK, 0);
	DisplayPanel (panelHandle);
	
	//Abriendo conexion mediante el DataScket:
	DS_Open("dstp://weather.ni.com/msdemo",DSConst_Read, dsDataCallback, NULL, &dsData);
	
	RunUserInterface ();
	
	DS_DiscardObjHandle(dsData);
	
	DiscardPanel (panelHandle);
	return 0;
}



int CVICALLBACK panelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK ok (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK AquireData (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int size;
	double *value;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Borrando las graficas anteriores en la grafica de datos originales
			DeleteGraphPlot(panelHandle, MAINPANEL_ORIGINALGRAPH, -1, VAL_IMMEDIATE_DRAW);
			//Se adopta el numero de puntos requeridos por el usuario
			GetCtrlVal (panelHandle, MAINPANEL_NUMBERPOINTS, &numpoints);
			//respaldar los datos anteriores dentro de oldarray[]
			DS_Update(dsData); 
			DS_GetDataType(dsData, NULL, &size, NULL);
			if(size == 0)
				return -1;
			value = malloc(size* sizeof(double));
			DS_GetDataValue(dsData, CAVT_DOUBLE | CAVT_ARRAY, value, size*sizeof(double), 0, 0);
			Copy1D(value, numpoints, array);
			Copy1D(array, numpoints, oldarray);
			//Dibujando la grafica en la grafica original
			PlotY (panelHandle, MAINPANEL_ORIGINALGRAPH, array, numpoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1,
				   VAL_GREEN);
			free(value);
			break;
	}
	return 0;
}

int CVICALLBACK AnalyzeData (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int boolean, function, PointsToPlot = numpoints;
	double *tempArray, df, *staticArray, dt = 1/2048;
	
	switch (event)
	{
		case EVENT_COMMIT:
			Copy1D(oldarray, numpoints, array);
			tempArray = malloc(numpoints*sizeof(double));
			staticArray = malloc(numpoints*sizeof(double));
			staticArray = tempArray = array;
			GetCtrlAttribute (panelHandle, MAINPANEL_ANALYZE, ATTR_CTRL_VAL, &function);
			switch(function)
			{
				case 0: //power Spectrum
					AutoPowerSpectrum (staticArray, numpoints, dt, tempArray, &df);
					PointsToPlot = numpoints / 2;
				break;
					
				case 1: //Filter
					Bw_LPF (staticArray, numpoints, 2048, 25, 5, tempArray);
				break;
						
				case 2: //power spectrum with filter
					Bw_LPF (tempArray, numpoints, 2048, 25, 5, staticArray);
					AutoPowerSpectrum (staticArray, numpoints, dt, tempArray, &df);
					PointsToPlot = 100;
			 	break;
			}
			DeleteGraphPlot (panelHandle, MAINPANEL_ANALYZEDGRAPH, -1, VAL_IMMEDIATE_DRAW);
			break;
	}
	return 0;
}

int CVICALLBACK Help (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if((handle = LoadPanel (0, "Adquisicion.uir", HELP)) < 0)
				return -1;
			DisplayPanel (handle);
			RunUserInterface ();
			DiscardPanel (handle);
			break;
	}
	return 0;
}
