//==============================================================================
//
// Title:       User Interface Application
// Purpose:     A short description of the application.
//
// Created on:  15/03/2013 at 07:31:33 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "User Interface Application.h"
#include "toolbox.h"

#include "fileDriving.h"
#include "lamda.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables
double normalized[6][3] = {{0.0, 0.200, 0.8}, 
						   {0.300000, 0.2000, 0.25}, 
						   {1.00, 0.000000, 1.0}, 
						   {0.400, 1.00, 0.5},
						   {0.50, 1.0000, 0.0},
						   {0.1, 0.1, 0.9}};
int Ndescriptors = 0;
int Nsubjects	 = 1;
int Nclases		 = 0;
int online		 = 0;

int vacio = 0;

char clasesRecs[N_OPERATIONS_TO_SAVE * 5 + 1] = {0}; // 5 = 3(indice hasta de 3 digitos) + 1(coma) + 1(espacio)
char cl[5] = {0};									 // + 1(terminador nulo)
FILE *classLogFile;
char pathFile[260] = {0};
double **MADs, *GADs, **clasesROs;
char errorMap[6][100] = {"No se pudo abrir el archivo de Clases Eexistentes", 
						 "No se pudo abrir el archivo de Sujetos a Observar", 
						 "No se pudo actualizar el archivo de clases Existentes",
						 "Cabecera de archivo incorrecta",
						 "Operacion exitosa!!",
						 "La dimension del espacio de entrada no es la requerida"};

int x = 0;
//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "User Interface Application.uir", PANEL));
    
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

int CVICALLBACK diagnosisButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int err = 0, i = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile))
			{
				if (err = matrixClasesConfig (pathFile, &Nclases, &Ndescriptors) >= 0)
				{
					MessagePopup ("Error de acceso", errorMap[err]);
					return 0;
				}
				 																		//Reservamos memoria para todas las matrices que seran  
				clasesROs = (double **) malloc((Nclases + 1) * sizeof(double *));		//usadas; dados los parametros proporcionados por 
				for (i = 0; i < Nclases + 1; i++)										//matrixClasesConfig().
					clasesROs[i] = (double *) malloc(Ndescriptors * sizeof(double));
				
				if (err = loadFile(pathFile, clasesROs, Nclases, Ndescriptors) >= 0)	//Cargamos el archivo de clases existentes.
				{
					MessagePopup ("Error de acceso", errorMap[err]);
					return 0;
				}
				online = 0;
				MADs = (double **) malloc(Nclases * sizeof(double *));
				for (i = 0; i < Nclases; i++)
					MADs[i] = (double *) malloc((Ndescriptors - 1)* sizeof(double));
		
				GADs = (double *) malloc(Nclases * sizeof(double));
				//SetCtrlVal (panelHandle, PANEL_CLASES_READED, Nclases);
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			}
		break;
	}
	return 0;
}

int CVICALLBACK aprenderArchivoButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int err = 1, j = 0;
	char *buferX, bufer[6] = {0};
	vacio = 0;
	if (event == EVENT_COMMIT)
	{
		err = LAMDA_LFF ();
		MessagePopup ("Mensaje del sistema...", errorMap[err]);
	}
	return 0;
}

int CVICALLBACK aprenderEnLineaButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i = 0, err = 0;
	vacio = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile))
			{
				if (err = matrixClasesConfig (pathFile, &Nclases, &Ndescriptors) >= 0)
				{
					MessagePopup ("Error de acceso", errorMap[err]);
					return 0;
				}
				online = 1;
				 																		//Reservamos memoria para todas las matrices que seran  
				clasesROs = (double **) malloc((Nclases + 1) * sizeof(double *));		//usadas; dados los parametros proporcionados por 
				for (i = 0; i < Nclases + 1; i++)										//matrixClasesConfig().
					clasesROs[i] = (double *) malloc(Ndescriptors * sizeof(double));
				
				if (err = loadFile(pathFile, clasesROs, Nclases, Ndescriptors) >= 0)	//Cargamos el archivo de clases existentes.
				{
					MessagePopup ("Error de acceso", errorMap[err]);
					return 0;
				}
				
				MADs = (double **) malloc(Nclases * sizeof(double *));
				for (i = 0; i < Nclases; i++)
					MADs[i] = (double *) malloc((Ndescriptors-1) * sizeof(double));
		
				GADs = (double *) malloc(Nclases * sizeof(double));
	
				
				SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			}
		break;
	}
	return 0;
}

int CVICALLBACK stopButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int i = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			//Procedemos a liberar toda la memoria reservada para no generar replicas de cualquier tipo o datos indeseables en en-
			//trenamientos posteriores:
			for (i = 0; i < vacio + 1; i++)
				free(clasesROs[i]);
			free (clasesROs);
			for (i = 0; i < vacio; i++)
				free(MADs[i]);
			free (MADs);
			free (GADs);
			fclose (classLogFile);
			break;
	}
	return 0;
}

int CVICALLBACK realTimeR_Recognition (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int clase = 0;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if (online)
			{   
				if (x < 6)
				{
				LAMDA_LOL (normalized[x], &clase, sizeof(normalized[x])/sizeof(normalized[0][0]));
				sprintf(cl, "%d, ", clase);
				strcat(clasesRecs, cl);
				storeKnoledgeFile (pathFile, clasesROs, vacio, Ndescriptors);					
				x++;
				}
				else 											//sólo vamos al disco duro cada 100 operaciones
				{
					classLogFile = fopen ("c:\\Users\\utm\\Documents\\cadenaClases.csv", "a");
					clasesRecs[strlen (clasesRecs)-1] = 10;
					fputs (clasesRecs, classLogFile);
					sprintf(clasesRecs, ""); 
					x = 0;
					fclose(classLogFile); 
				}
			}
			else
			{
				if (x < 6)   //100)
				{
				LAMDA_ROL (normalized[x], &clase, sizeof(normalized[x])/sizeof(normalized[0][0]));
				sprintf(cl, "%d, ", clase);
				strcat(clasesRecs, cl);
				x++;
				}
				else											//sólo vamos al disco duro cada 100 operaciones
				{
					classLogFile = fopen ("c:\\Users\\utm\\Documents\\cadenaClases.csv", "a");
					clasesRecs[strlen (clasesRecs)-1] = 10;
					fputs (clasesRecs, classLogFile);
					sprintf(clasesRecs, ""); 
					x = 0;
					fclose(classLogFile);
				}
			}
			break;
	}
	return 0;
}
