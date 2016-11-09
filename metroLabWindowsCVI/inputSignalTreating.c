//==============================================================================
//
// Title:       inputSignalTreating
// Purpose:     This code is attempted to acquire and processing signals coming
//				from cajon del PA in order to perform pattern recognition via LAMDA. 
//				All before will be applyed to fail diganosis of PA system at STC 
//				Metro, in Mexico City. 
// Created on:  11/04/2013 at 02:01 p.m. by Ignacio Arroyo Fernadez.
// Copyright:   El nacho. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <cviauto.h>
#include <analysis.h>
#include <NIDAQmx.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "inputSignalTreating.h"
#include "User Interface Application.h" 
#include "toolbox.h"
#include <cvitdms.h>

//==============================================================================
// Constants
#define CENTER_FREQ_23k 1E3//23.0E3
#define WIDTH_FREQ_23k  2000//5.00E3
#define CENTER_FREQ_100k 100.0E3
#define WIDTH_FREQ_100k  10.00E3

//#define samplesPerChannel 30000

/*****Mapas perfil de procesamiento de señales para cada cajón******/
#define mapaCajonI_PACMC		  "bbbbabbbbbbbbbbbbbbabbbabbbbbbbb"
#define mapaCajonII_PACMC		  "abd"
#define mapaCajonIII_PACMC		  "10110001"
#define mapaCajonIV_PACMC		  "11101001"
#define mapaCajonI_CMLCMR		  "11101000"
#define mapaCajonII_CMLCMR		  "00010110"
/*** Veámos a continuación una tabla con el significado de cada caracter en la 
cadena de cada mapa perfil de procesamiento:

'a' significa FFT 23  KHz,
'b'           Av  +5  V, 
'c' 		  FFT 100 KHz,
'd' 		  Av  -5  V,
'e' 		  Av +12  V,
'f' 		  Av -12  V,
'g' 		  Av +24  V,
'h' 		  Av -24  V,
'i' 		  Av +72  V,
'j'		  	  Av +15  V,	
'k'		señal no caracterizada

donde: Av  -> promediación de la señal
	   FFT -> extracción del pico de frecuencia mediante FFT.
*****/ 

//frecuencias de muestro para cada TASK depende de cuantas señales hay ke muestrear 
//en cada cajón, asi como de la naturaleza de las mismas.
#define sampCajonI_PACMC    2.1739e-5 
#define sampCajonII_PACMC   0.00025
#define sampCajonIII_PACMC  12.04E-6
#define sampCajonIV_PACMC   12.04E-6
#define sampCajonI_CMLCMR   12.04E-6
#define sampCajonII_CMLCMR  12.04E-6

// Aqui se tienen los limites inferior y superior de amplitud para los dos tipos de 
// señales de CA que se van a medir.
#define ampINF23 0.35
#define ampSUP23 0.75
#define ampINF100 0.45 //Verificar estos valores!!!!!!!!!!!!!!!!!!
#define ampSUP100 0.85
//==============================================================================
// Types
SearchType span; 
//==============================================================================
// Static global variables
static TaskHandle daqTask;
//==============================================================================
// Global variables

float64 ***dataFile, *dataIn, **signals, **processed, **mapaMaxsMins;
double **normalized;
int32 sampsReaded;
int32 samplesPerChannel = 100;
//==============================================================================
// Global functions

//==============================================================================
// UI callback function prototypes
void signalSorter(int *numCanales, int muestrasPorCanal, float64 *datosEntrada, float64 **canales);
void signalProcessing (void);
void memoryReservation (void);
void dataNormalization(void);
void dataAutoNormalization(void);
void stopTask(void);

/// HIFN  Esta función configura el Task que se vaya utilizar según el cajón seleccionado, pueden ir mas cases, en caso de que se añadiera JH al programa. 
/// HIPAR cajon/Numero de cajon que se va a diagnosticar.
int taskSelectingAndSignalMapping (int cajon)
{
	char *tarea = "                    ";
	int error = 0;

	if (!perfil.running)
			{
				switch (cajon)
				{
					case 0:		
						errChk (DAQmxLoadTask ("cajonI_PACMC_0", &daqTask)); //CArgar el Task asociado al cajón en cuestion.
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonI_PACMC;
						perfil.sampling = sampCajonI_PACMC;
						
					break;
					
					case 1:
						errChk (DAQmxLoadTask ("cajon_II_PACMC_6225", &daqTask));
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonII_PACMC;
						perfil.sampling = sampCajonII_PACMC;
						
					break;
					
					case 2:
						errChk (DAQmxLoadTask ("Tasker", &daqTask));
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonIII_PACMC;
						perfil.sampling = sampCajonIII_PACMC;
						
					break;
					
					case 3:
						errChk (DAQmxLoadTask ("voltageTask", &daqTask));
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonIV_PACMC;
						perfil.sampling = sampCajonIV_PACMC;
					
					break;
					
					case 4:
						errChk (DAQmxLoadTask ("voltageTask", &daqTask));
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonI_CMLCMR;
						perfil.sampling = sampCajonI_CMLCMR;
					
					break;
				
					case 5:
						errChk (DAQmxLoadTask ("voltageTask", &daqTask));
						errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20));
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonII_CMLCMR;
						perfil.sampling = sampCajonII_CMLCMR;
						
					break;
			
				}
				errChk (DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels));
				/*if (perfil.numChannels != (sizeof(perfil.perfilMap)/sizeof(perfil.perfilMap[0])) - 1)
				{
					error = -1;
					MessagePopup ("Error de configuracion", "El numero de canalaes del Task no coincide con el vector PERFIL");
					goto Error;
				}*/
				int32 bufferSize;
				
				perfil.dataSize = perfil.numChannels * samplesPerChannel;
				dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
				DAQmxCfgSampClkTiming (daqTask, NULL, 1/perfil.sampling, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samplesPerChannel*2);
				//DAQmxSetTrigAttribute (daqTask, DAQmx_RefTrig_PretrigSamples, 2); 
				DAQmxSetBufferAttribute (daqTask, DAQmx_Buf_Input_BufSize, 1000000);
				//DAQmxGetBufferAttribute (daqTask, DAQmx_Buf_Input_BufSize, &bufferSize);
				//////////////////////////7
				errChk (DAQmxStartTask (daqTask));
				perfil.running = 1;
			}

	return 0;
Error:
	if (error == -201003)
		MessagePopup ("Error de hardware", "El dispositivo de Adquisicion no esta disponible o se encuentra desconectado");
	else
		DAQmxClearTask (daqTask);  //Para cualkier otro caso hay ke destruir el task dado que no fue posible usarlo aunque ya fue creado.
	SetCtrlVal (panelHandle, PANEL_TOGGLE_ONLINE_LEARN, 0);
	return error;
}
/********************************************************************************************************
Una idea para procesar el perfil y cada una de sus señales seria declarar como char perfilMap[numChannels]
como cadena de 0's y 1's. representando cada elemento de dicha cadena el indice de cada canal y el tipo de 
procesamiento que se llevaria a cabo con cada uno (FFT o Av). En tonces dentro de un for() elejir uno de 
los dos segun sea perfilMap[i]. Por ejemplo: 0 para FFT y 1 para Av. dentro del for, en cada ciclo se puede 
obtener el numero deseado de cada señal. posteriormente, este seria normalizado para que lo procese LAMDA.
*********************************************************************************************************/

int ProcessingProfile (void)	//los cajones, ya que las funciones que participan estan generalizadas en base al perfil de medicion 
{					            //de cada cajon; mismo que se establece previmente  en la función del boton PRuEBA.
							 
	int error = 0; 
			
			errChk (DAQmxReadAnalogF64 (daqTask, samplesPerChannel, 10.0, DAQmx_Val_GroupByChannel, dataIn, perfil.dataSize, &sampsReaded, 0));
			 
			//samplesPerChannel = 1000;
		    //Reservado de memoria para la matriz de señales:
			memoryReservation (); 
			//Reordenamiento del vector de entrada, salida: **signals
			signalSorter(&perfil.numChannels, samplesPerChannel, dataIn, signals);  //pudo ser sin arguments jejeje. asi es mas gral
			//Procesado de cada señal de entrada ya sea como promedio o por FFT, según sea el perfilMap. salida: **processed
			signalProcessing();	
			//Normalizacion de los datos de entrada para que sean procesados por el algoritmo de reconocimiento. salida: **normalized
			dataAutoNormalization();
		    //samplesPerChannel = samplex;
	return 0;
Error:
	return error;
}

/******************************************************************************************************
The next fuction takes the acquired dataIn array provided by DAQmxReadAnalogF64(); which has the samples
of each channel in a inconvenient way to treat them. signalSorter(); give us each channel in a better
form to be treated, it means that **canales points to a matrix which number of rows is the number of 
channels and the number of columns is the number of samples per channel and per acquirement. 
*******************************************************************************************************/

void signalSorter(int *numCanales, int muestrasPorCanal, float64 *datosEntrada, float64 **canales)
{
	uInt32 i, j = 0, k;
	//reordenamiento:
	/*for (i = 0; i < *numCanales; i++)
	{
		for (j = i, k = 0; k < muestrasPorCanal;  j += *numCanales, k++)
			canales[i][k] = datosEntrada[j];
	}*/ 
	
	for (i = 0; i < *numCanales; i++)
	{
		for (j = 0; j < muestrasPorCanal; j++)
			canales[i][j] = datosEntrada[(muestrasPorCanal * i) + j];
	}
}

void signalProcessing (void)
{
	double prom, frec, amp, fase, f0;
	
	int i, j;
	for (i = 0; i < perfil.numChannels; i++)
	{
		if(perfil.perfilMap[i] != 'a' && perfil.perfilMap[i] != 'c')  //TODAS las de CD, menos la de CA
		{
		   Median(signals[i], samplesPerChannel, &prom);
		   for (j = 0; j < samplesPerChannel; j++)
			   processed[i][j] = prom;
		}
		//A continuacion todas las de CA:
		if (perfil.perfilMap[i] == 'a')  //23KHZ
		{
			span.centerFrequency = CENTER_FREQ_23k;
			span.frequencyWidth = WIDTH_FREQ_23k;
			SingleToneInfo (signals[i], samplesPerChannel, perfil.sampling, &span, &frec, &amp, &fase);
			GetCtrlVal (panelHandle, PANEL_VIEW_CHANN, &j);
			if (j == i)
				SetCtrlVal (panelHandle, PANEL_FREC, frec);
			if (amp > ampINF23 && amp < ampSUP23)   //Como f0 no se usa, mas que para rellenar la
				f0 = 1; 							//función aterior; pues aqui la usamos.
			else 									//Lo que haría este if sería definir si la amplitud de la señal
				f0 = 0; 							//se encuentra dentro del rango permitido por ampINFxxx y ampSUPxxx.
			//Este ciclo al mismo tiempo hace que se promedie el valor de frecuencia obtenido en cada adquisición.
			for (j = 0; j < samplesPerChannel; j++)					  
				processed[i][j] = frec*f0;  //en caso de que la señal no cumpla con la amplitud, ésta se lleva a cero.
			
		}
		//Es necesario diferenciar entre 100khz y 23 khz dado ke la funcion que calcula la frecuencia pico de la señal
		//requiere que le sea especificado un rango de trabajo.
		if (perfil.perfilMap[i] == 'c')  //100KHZ
		{   
			span.centerFrequency = CENTER_FREQ_100k;
			span.frequencyWidth = WIDTH_FREQ_100k;
			SingleToneInfo (signals[i], samplesPerChannel, perfil.sampling, &span, &frec, &amp, &fase);
			GetCtrlVal (panelHandle, PANEL_VIEW_CHANN, &f0);
			if (f0 == i)
				SetCtrlVal (panelHandle, PANEL_FREC, frec);
			if (amp > ampINF100 && amp < ampSUP100) //Como f0 no se usa, mas que para rellenar la
				f0 = 1; 							//función aterior; pues aqui la usamos.
			else 
				f0 = 0; 
			
			for (j = 0; j < samplesPerChannel; j++)					     
				processed[i][j] = frec*f0;  //en caso de que la señal no cumpla con la amplitud, ésta se lleva a cero.
		}

	}
}

void memoryReservation (void)
{
	signals 	= (float64 **) malloc (perfil.numChannels * sizeof(float64 *));
	processed 	= (float64 **) malloc (perfil.numChannels * sizeof(float64 *));
	normalized	= (double **) malloc (perfil.numChannels * sizeof(double *));
	
	for ( int i = 0; i < perfil.numChannels; i++)
	{
		signals[i] 	 = (float64 *) malloc(samplesPerChannel * sizeof(float64));
		processed[i] = (float64 *) malloc(samplesPerChannel * sizeof(float64));
		normalized[i]= (double *) malloc(samplesPerChannel * sizeof(double));
	}
}

/***Se requiere generalizar esta funcion, con el fin de que tambien se pueda usar en el entrenamiento
del algoritmo. Se necesita que se le pasen parametros tales como processed[]:    O tambien podriamos 
copiar la informacion obtenida desde el archivo hasta las matrices ke la funcion manipula de mnera global*/

void dataAutoNormalization(void) 
{

int i = 0, j = 0, mai = 0, mii = 0;
double maxi = 0, mini = 0;

	for (i = 0; i < perfil.numChannels; i++)
	{
		
		if (perfil.perfilMap[i] == 'a' || perfil.perfilMap[i] == 'c')
		{
			/*if (perfil.perfilMap[i] == 'a') //23khz
			{
				mapaMaxsMins[i][0] = 2E3;	
				mapaMaxsMins[i][1] = 0;
			}
			else							//100khz
			{
				mapaMaxsMins[i][0] = 120E3;	
				mapaMaxsMins[i][1] = 0;
			}*/
			if (mapaMaxsMins[i][0] < processed[i][0])
				mapaMaxsMins[i][0] = processed[i][0];
			//if (mapaMaxsMins[i][1] > processed[i][0])
				mapaMaxsMins[i][1] = 0;		
		}
		else
		{
			MaxMin1D (signals[i], samplesPerChannel, &maxi, &mai, &mini, &mii);
			if (mapaMaxsMins[i][0] < maxi)
				mapaMaxsMins[i][0] = maxi;
			if (mapaMaxsMins[i][1] > mini)
				mapaMaxsMins[i][1] = mini;
			
		}

	}


	for (i = 0; i < perfil.numChannels; i++)
	{   
		for (j = 0; j < samplesPerChannel; j++)
			normalized[i][j] = (processed[i][0] - mapaMaxsMins[i][1])/(mapaMaxsMins[i][0] - mapaMaxsMins[i][1]);
	}

}

void stopTask(void)
{
	DAQmxStopTask (daqTask);
}

int loadTDMS ()
{

	uInt64 **numChanValues;
	unsigned int numChannelGroups, *fileNumChannels, numValuesPerChan, i, j, errorCode = 0;
	TDMSChannelHandle **channels;
	TDMSChannelGroupHandle *channelGroups;
	TDMSFileHandle fileHandle;
	char pathFile[MAX_PATHNAME_LEN] = "c:\\";


			DirSelectPopup (pathFile, "Primero seleccione el directorio", 1, 1, pathFile);
			FileSelectPopup (pathFile, "*.tdms", "*.tdms", "", VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile);
			TDMS_OpenFileEx (pathFile, 1, 0, &fileHandle);
			TDMS_GetNumChannelGroups (fileHandle, &numChannelGroups);
			channelGroups = (TDMSChannelGroupHandle *) malloc (numChannelGroups * sizeof(TDMSChannelGroupHandle));
			fileNumChannels = (unsigned int *) malloc(numChannelGroups * sizeof(unsigned int));
			
			TDMS_GetChannelGroups (fileHandle, channelGroups, numChannelGroups);
			
			for (i = 0; i < numChannelGroups; i++)
				TDMS_GetNumChannels (channelGroups[i], &fileNumChannels[i]);
			
			channels = (TDMSChannelHandle **) malloc (numChannelGroups * sizeof(TDMSChannelHandle *));
			for (i = 0; i < numChannelGroups; i++)
				channels[i] = (TDMSChannelHandle *) malloc(fileNumChannels[i] * sizeof(TDMSChannelHandle));
			
			for (i = 0; i < numChannelGroups; i++)
				TDMS_GetChannels (channelGroups[i], channels[i], fileNumChannels[i]);
			
			numChanValues = (uInt64 **) malloc (numChannelGroups * sizeof(uInt64 *));
			for (i = 0; i < numChannelGroups; i++)
				numChanValues[i] = (uInt64 *) malloc(fileNumChannels[i] * sizeof(uInt64));
			
			for (i = 0; i < numChannelGroups; i++)
				for (j = 0; j < fileNumChannels[i]; j++)
					TDMS_GetNumDataValues (channels[i][j], &numChanValues[i][j]);
			
			dataFile = (float64 ***) malloc (numChannelGroups * sizeof(float64 **));
			for (i = 0; i < numChannelGroups; i++)
				dataFile[i] = (float64 **) malloc(fileNumChannels[i] * sizeof(float64 *));
			
			for (int i = 0; i < numChannelGroups; i++)
				for (j = 0; j < fileNumChannels[i]; j++)
						dataFile[i][j] = (float64 *) malloc((size_t) numChanValues[i][j] * sizeof(float64));
			
			
			for (i = 0; i < numChannelGroups; i++)
				for (j = 0; j < fileNumChannels[i]; j++)
					TDMS_GetDataValuesEx (channels[i][j], 0, (size_t) numChanValues[i][j], dataFile[i][j]);
			/*Hasta el momento todo fue generalizado para abrir un archivo con N numero de grupos. De aqui en adelante 
			  supondremos que solo existe un grupo de canales, el grupo 0. Ademas supondremos que todos los canales
			  tienen el mismo numero de muestras, asi que solo tomamos el numero de muestras del canal 0 del grupo 0
			  para reservarle memoria a la matriz de señales  ****/
			signals = (float64 **) malloc (fileNumChannels[0] * sizeof(float64 *));
			for (i = 0; i < fileNumChannels[0]; i++)
				signals[i] = (float64 *) malloc ((size_t) numChanValues[0][0] * sizeof(float64));
			/*Ahora, parovechando los apuntadores, vaceamos el archivo en la matriz de señales creada con las lineas anteriores: */
			signals = dataFile[0];
			
			
			if (!errorCode)
			{
				SetCtrlVal (panelHandle, PANEL_TASKID, "Archivo cargado con exito");
			}
			else
			{   
				MessagePopup ("Error message", "Se comprueba la existencia de un error al abrir el archivo especificado.");
			}

	return 0;
}


