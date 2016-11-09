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

#include <NIDAQmx.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "User Interface Application.h"
#include "toolbox.h"

#include "fileDriving.h"
#include "lamda.h"
#include "inputSignalTreating.h"

//==============================================================================
// Constants
#define acquirementsPerAccess 100
//==============================================================================
// Types

//==============================================================================
// Static global variables

int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables
int Ndescriptors = 0;
int Nsubjects	 = 1;
int Nclases		 = 0;
int online		 = 0;

int vacio = 0;

char clasesRecs[N_OPERATIONS_TO_SAVE * 5 + 1] = {0}; // 5 = 3(indice hasta de 3 digitos) + 1(coma) + 1(espacio)
char cl[5] = {0};									 // + 1(terminador nulo)
FILE *classLogFile;
char pathFile[260] = "c:\\Users\\utm\\Documents\\clases00.csv", **stateArray;
double **MADs, *GADs, **clasesROs;
char errorMap[6][100] = {"No se pudo abrir el archivo de Clases Eexistentes", 
						 "No se pudo abrir el archivo de Sujetos a Observar", 
						 "No se pudo actualizar el archivo de clases Existentes",
						 "Cabecera de archivo incorrecta",
						 "Operacion exitosa!!",
						 "La dimension del espacio de entrada no es la requerida"};

int x = 0, cajonID = 0, Nstates = 0;
//==============================================================================
// Global functions
void freeingMemories (void);
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
	int modo = 0;
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
	
	GetCtrlVal (panelHandle, PANEL_MODO, &modo);
	if (!modo)
		{
			SetCtrlAttribute (panelHandle, PANEL_APRENDER_FROM_FILE, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_TOGGLE_ONLINE_LEARN, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_CLASES_FILE, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_LAMBDA, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_SELECT_FILE, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_LED_SUJETOS_LOADED, ATTR_DIMMED, 0); 
			SetCtrlAttribute (panelHandle, PANEL_TASKID, ATTR_DIMMED, 1);
			SetCtrlAttribute (panelHandle, PANEL_TOGGLE_DIAGNOSIS, ATTR_DIMMED, 0);
		}
		else
		{
			SetCtrlAttribute (panelHandle, PANEL_APRENDER_FROM_FILE, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_TOGGLE_ONLINE_LEARN, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_CLASES_FILE, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_LAMBDA, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_SELECT_FILE, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_LED_SUJETOS_LOADED, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_TASKID, ATTR_DIMMED, 0);
			SetCtrlAttribute (panelHandle, PANEL_TOGGLE_DIAGNOSIS, ATTR_DIMMED, 1);
		
		}
	SetCtrlVal (panelHandle, PANEL_CLASES_FILE, pathFile);
    return 0;
}

int CVICALLBACK aprenderArchivoButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int err = 1;
	vacio = 0;
	if (event == EVENT_COMMIT)
	{
		err = LAMDA_LFF ();
		MessagePopup ("Mensaje del sistema...", errorMap[err]);
	}
	return 0;
}

int CVICALLBACK onLineLearning (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int actLearning = 0, i = 0, error = 0, err = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_TOGGLE_ONLINE_LEARN, &actLearning);
			if (actLearning)
			{
				GetCtrlVal (panelHandle, PANEL_CAJON, &cajonID);  
				errChk (taskSelectingAndSignalMapping (cajonID));
			/*	if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile))*/
				if (GetCtrlVal (panelHandle, PANEL_CLASES_FILE, pathFile) >= 0)
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
				
					MADs = (double **) malloc(Nclases * sizeof(double *));
					for (i = 0; i < Nclases; i++)
						MADs[i] = (double *) malloc((Ndescriptors-1) * sizeof(double));
		
					mapaMaxsMins= (double **) malloc (perfil.numChannels * sizeof(double *));
					for (i = 0; i < perfil.numChannels; i++)
					{
						mapaMaxsMins[i]= (double *) malloc (2 * sizeof(double)); 
						mapaMaxsMins[i][0] = -10; mapaMaxsMins[i][1] = 400000; 
					}
					
					GADs = (double *) malloc(Nclases * sizeof(double));
					SetCtrlVal (panelHandle, PANEL_LED_CLASES_FILE, 1);
					stateArray = loadStates ("c:\\Users\\utm\\Documents\\states.csv", &Nstates);
					
					online = 1;
					SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				}	
			}
			else
			{
				stopTask();
			   	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
				storeKnoledgeFile (pathFile, clasesROs, vacio, Ndescriptors);
			//Procedemos a liberar toda la memoria reservada para no generar replicas de cualquier tipo o datos indeseables en en-
			//trenamientos posteriores:
				freeingMemories ();
				
			}

			break;
	}
Error: 
	return 0;
}

int CVICALLBACK onlineDiagnosisButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int actLearning = 0, i = 0, error = 0, err = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_TOGGLE_DIAGNOSIS, &actLearning);
			if (actLearning)
			{
				GetCtrlVal (panelHandle, PANEL_CAJON, &cajonID);  
				errChk (taskSelectingAndSignalMapping (cajonID));

				if (GetCtrlVal (panelHandle, PANEL_CLASES_FILE, pathFile) >= 0)
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
				
					MADs = (double **) malloc(Nclases * sizeof(double *));
					for (i = 0; i < Nclases; i++)
						MADs[i] = (double *) malloc((Ndescriptors-1) * sizeof(double));
		
					mapaMaxsMins= (double **) malloc (perfil.numChannels * sizeof(double *));
					for (i = 0; i < perfil.numChannels; i++)
					{
						mapaMaxsMins[i]= (double *) malloc (2 * sizeof(double)); 
						mapaMaxsMins[i][0] = -10; mapaMaxsMins[i][1] = 400000; 
					}
					
					GADs = (double *) malloc(Nclases * sizeof(double));
					SetCtrlVal (panelHandle, PANEL_LED_CLASES_FILE, 1);
					stateArray = loadStates ("c:\\states.csv", &Nstates);
					
					online = 0;
					SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
				}	
			}
			else
			{
				stopTask();
			   	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			//Procedemos a liberar toda la memoria reservada para no generar replicas de cualquier tipo o datos indeseables en en-
			//trenamientos posteriores:
				freeingMemories ();
				
			}

			break;
	}
Error: 
	return 0;
}

int CVICALLBACK realTimeR_Recognition (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int clase = 0, error = 0, i = 0;
	double *norData;
	
	norData = (double *) malloc (perfil.numChannels * sizeof (double));
	
	switch (event)
	{
		case EVENT_TIMER_TICK:
			
			errChk (ProcessingProfile ());
			if (online)
			{   //Solo tomamos el primer elemento de la matriz de valores normalizados ya que los demas son iguales y es incorrecto
				//reforzar el aprendizaje de las clases con [samplesPerChannel] elementos que sabemos son identicos.
				for (i = 0; i < perfil.numChannels; i++)
				{
					norData[i] = normalized[i][0];
				}
				
				LAMDA_OLL (norData, &clase, perfil.numChannels + 1);
				sprintf(cl, "%d, ", clase);
				strcat(clasesRecs, cl);
				SetCtrlVal (panelHandle, PANEL_COLORNUM, clase);
				if (clase <= Nstates - 1)
					SetCtrlAttribute (panelHandle, PANEL_COLORNUM, ATTR_LABEL_TEXT, stateArray[clase]);
				if (clase > Nstates - 1)
					SetCtrlAttribute (panelHandle, PANEL_COLORNUM, ATTR_LABEL_TEXT, cl);
	
				x++;
				
				if (x > acquirementsPerAccess) 	//sólo vamos al disco duro cada [acquirementsPerAccess] operaciones
				{
					classLogFile = fopen ("c:\\Users\\utm\\Google Drive\\CAJONES Y SEÑALES\\CAJON I PA-CMC\\cadenaClases.csv", "w");
					clasesRecs[strlen (clasesRecs)-1] = 10;
					fputs (clasesRecs, classLogFile);
					sprintf(clasesRecs, ""); 
					x = 0;
					fclose(classLogFile); 
				}
			}
			else
			{
				for (i = 0; i < perfil.numChannels; i++)
				{
					norData[i] = normalized[i][0];
				}
				LAMDA_ROL (norData, &clase, perfil.numChannels + 1);//sizeof(normalized[0])); Modificado el 20-09-13
				sprintf(cl, "%d, ", clase);
				strcat(clasesRecs, cl);
				SetCtrlVal (panelHandle, PANEL_COLORNUM, clase);
				if (clase <= Nstates - 1)
					SetCtrlAttribute (panelHandle, PANEL_COLORNUM, ATTR_LABEL_TEXT, stateArray[clase]);
				if (clase > Nstates - 1)
					SetCtrlAttribute (panelHandle, PANEL_COLORNUM, ATTR_LABEL_TEXT, cl);
	
				x++;
				
				if (x < acquirementsPerAccess)		
				{
					classLogFile = fopen ("c:\\cadenaClases.csv", "a");
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
Error:
	MessagePopup ("Error de hardware", "El dispositivo dquisicion se encuentra desbordado. Reajustar parametros del TASK");
	freeingMemories ();
	return 0;
}

void freeingMemories ()
{
	int i = 0;
	
	for (i = 0; i < vacio + 1; i++)
		free(clasesROs[i]);
	free (clasesROs);
	for (i = 0; i < vacio; i++)
		free(MADs[i]);
	free (MADs);
	for (i = 0; i < Nstates; i++)
		free(stateArray[i]);
	free (stateArray);
	free (GADs);
}

int CVICALLBACK fileSelectionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile);
			//SetCtrlVal (panelHandle, PANEL_CLASES_FILE, pathFile);
			break;
	}
	return 0;
}
