//==============================================================================
//
// Title:       selectingMultipleDAQtasks
// Purpose:     This code is attempted to acquire, processing and performing .
//				pattern recognition via LAMDA. All before will be applyed to
//				fail diganosis of PA system at STC Metro, in Mexico City. 
// Created on:  12/02/2013 at 05:28:25 p.m. by utm.
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
#include "selectingMultipleDAQtasks.h"
#include "toolbox.h"
#include <cvitdms.h>

//==============================================================================
// Constants
#define CENTER_FREQ1 23.0E3
#define WIDTH_FREQ1  5.00E3
#define CENTER_FREQ2 100.0E3
#define WIDTH_FREQ2  10.00E3

#define samplesPerChannel 256

/*****Mapas perfil de procesamiento de señales para cada cajón******/
#define mapaCajonI_PACMC		  "01601101xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define mapaCajonII_PACMC		  "01201011"
#define mapaCajonIII_PACMC		  "10110001"
#define mapaCajonIV_PACMC		  "11101001"
#define mapaCajonI_CMLCMR		  "11101000"
#define mapaCajonII_CMLCMR		  "00010110"
/*** Veámos a continuación una tabla con el significado de cada caracter en la 
cadena de cada mapa perfil de procesamiento:

'0' significa FFT 23  KHz,
'1'           Av  +5  V, 
'2' 		  FFT 100 KHz,
'3' 		  Av  -5  V,
'4' 		  Av +12  V,
'5' 		  Av -12  V,
'6' 		  Av +24  V,
'7' 		  Av -24  V,
'8' 		  Av +72  V.

donde: Av  -> promediación de la señal
	   FFT -> extracción del pico de frecuencia mediante FFT.
*****/ 

//frecuencias de muestro para cada TASK depende de cuantas señales hay ke muestrear 
//en cada cajón, asi como de la naturaleza de las mismas.
#define sampCajonI_PACMC    1.4285E-5 
#define sampCajonII_PACMC   12.04E-6
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

struct profileInfo{
	int numChannels;
	uInt32 dataSize;
	uInt8  running;
	char  *perfilMap;
	double sampling;
} perfil;


//==============================================================================
// Static global variables
static TaskHandle daqTask;
static int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

float64 ***dataFile, *dataIn, **signals, **processed, **normalized;

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "selectingMultipleDAQtasks.uir", PANEL));
    
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
void signalSorter(int *numCanales, uInt16 muestrasPorCanal, float64 *datosEntrada, float64 **canales);
void signalProcessing (void);
void memoryReservation (void);
void dataNormalization(void);   

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{   
	int modo;
	
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
	/*Inicializacion de la interfaz con los botones que deben estar activos e inactivos segun el modo de operacion*/
	GetCtrlVal (panelHandle, PANEL_MODO, &modo);
			if (!modo)
			{
				SetCtrlAttribute (panelHandle, PANEL_LOADING, ATTR_DIMMED, 1);
				SetCtrlAttribute (panelHandle, PANEL_PROBAR, ATTR_DIMMED, 0);
			}
			else
			{
				SetCtrlAttribute (panelHandle, PANEL_LOADING, ATTR_DIMMED, 0);
				SetCtrlAttribute (panelHandle, PANEL_PROBAR, ATTR_DIMMED, 1);
			}
			
    return 0;
}

//En esta función pueden ir mas casos, esto en caso de que se añadiera JH al programa. 
int CVICALLBACK probarFunctionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int cajon;
	char *tarea = "                    ";
	switch (event)
	{
		case EVENT_COMMIT:
			if (!perfil.running)
			{
				GetCtrlVal (panelHandle, PANEL_CAJON, &cajon);
				switch (cajon)
				{
					case 0:		
						DAQmxLoadTask ("voltageTask", &daqTask); //CArgar el Task asociado al cajón en cuestion.
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonI_PACMC;
						perfil.sampling = sampCajonI_PACMC;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					
					break;
					
					case 1:
						DAQmxLoadTask ("pruebaVoltageTask", &daqTask);
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonII_PACMC;
						perfil.sampling = sampCajonII_PACMC;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					break;
					
					case 2:
						DAQmxLoadTask ("Tasker", &daqTask);
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonIII_PACMC;
						perfil.sampling = sampCajonIII_PACMC;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					break;
					
					case 3:
						DAQmxLoadTask ("voltageTask", &daqTask);
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonIV_PACMC;
						perfil.sampling = sampCajonIV_PACMC;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					break;
					
					case 4:
						DAQmxLoadTask ("voltageTask", &daqTask);
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonI_CMLCMR;
						perfil.sampling = sampCajonI_CMLCMR;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					break;
				
					case 5:
						DAQmxLoadTask ("voltageTask", &daqTask);
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_Name, tarea, 20);
						SetCtrlVal (panelHandle, PANEL_TASKID, tarea);
						perfil.perfilMap = mapaCajonII_CMLCMR;
						perfil.sampling = sampCajonII_CMLCMR;
						//perfil.numChannels = 3;
						DAQmxGetTaskAttribute (daqTask, DAQmx_Task_NumChans, &perfil.numChannels);
						perfil.dataSize = perfil.numChannels * samplesPerChannel;
						dataIn = (float64 *) malloc (perfil.dataSize * sizeof (float64));
						DAQmxStartTask (daqTask);
						perfil.running = 1;
						SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
					break;
			
				}
			}
			break;
			
	}
	return 0;
}
/********************************************************************************************************
Una idea para procesar el perfil y cada una de sus señales seria declarar como char perfilMap[numChannels]
como cadena de 0's y 1's. representando cada elemento de dicha cadena el indice de cada canal y el tipo de 
procesamiento que se llevaria a cabo con cada uno (FFT o Av). En tonces dentro de un for() elejir uno de 
los dos segun sea perfilMap[i]. Por ejemplo: 0 para FFT y 1 para Av. dentro del for, en cada ciclo se puede 
obtener el numero deseado de cada señal. posteriormente, este seria normalizado para que lo procese LAMDA.
*********************************************************************************************************/

int CVICALLBACK timerProcessingProfile_0 (int panel, int control, int event, // Se requiere solo una función de TIMER para todos
		void *callbackData, int eventData1, int eventData2)					 //los cajones, ya que las funciones que participan
{   																		 //estan generalizadas en base al perfil de medicion 
																			 //de cada cajon; mismo que se establece previmente
	switch (event)															 //en la función del boton PRuEBA.
	{
		case EVENT_TIMER_TICK:
			DAQmxReadAnalogF64 (daqTask, samplesPerChannel, 10.0, DAQmx_Val_GroupByScanNumber, dataIn, perfil.dataSize, NULL, 0);
		    //Reservado de memoria para la matriz de señales:
			memoryReservation (); 
			//Reordenamiento del vector de entrada, salida: **signals
			signalSorter(&perfil.numChannels, samplesPerChannel, dataIn, signals);  //pudo ser sin arguments jejeje. asi es mas gral
			//Procesado de cada señal de entrada ya sea como promedio o por FFT, según sea el perfilMap. salida: **processed
			signalProcessing();	
			//Normalizacion de los datos de entrada para que sean procesados por el algoritmo de reconocimiento. salida: **normalized
			dataNormalization();
			PlotStripChart (panelHandle, PANEL_STRIPCHART, dataIn, samplesPerChannel, 0, 0, VAL_DOUBLE);
			break;
	}
	return 0;
}

/******************************************************************************************************
The next fuction takes the acquired dataIn array provided by DAQmxReadAnalogF64(); which has the samples
of each channel in a inconvenient way to treat them. signalSorter(); give us each channel in a better
form to be treated, it means that **canales points to a matrix which number of rows is the number of 
channels and the number of columns is the number of samples per channel and per acquirement. 
*******************************************************************************************************/

void signalSorter(int *numCanales, uInt16 muestrasPorCanal, float64 *datosEntrada, float64 **canales)
{
	uInt32 i, j = 0, k;
	//reordenamiento:
	for (i = 0; i < *numCanales; i++)
	{
		for (j = i, k = 0; k < muestrasPorCanal;  j += *numCanales, k++)
			canales[i][k] = datosEntrada[j];
	}
}


void signalProcessing (void)
{
	double prom, frec, amp, fase, signal[samplesPerChannel] = {0}, exSP, spectrum[samplesPerChannel] = {0} , f0, res;
	//De todas las anteriores solo se usan: prom frec, amp, las demás solo son para llemar los argumentos de SingleToneSignal();
	int i, j;
	for (i = 0; i < perfil.numChannels; i++)
	{
		if(perfil.perfilMap[i] != '0' && perfil.perfilMap[i] != '2')  //TODAS las de CD, o sea; menos la de CA
		{
		   Median(signals[i], samplesPerChannel, &prom);
		   for (j = 0; j < samplesPerChannel; j++)
			   processed[i][j] = prom;
		}
		if (perfil.perfilMap[i] == '0')
		{
			span.centerFrequency = CENTER_FREQ1;
			span.frequencyWidth = WIDTH_FREQ1;
			SingleToneSignal (signals[i], samplesPerChannel, perfil.sampling, &span, TONE_EXPORT_NONE, 
		 				&frec, &amp, &fase, signal, &exSP, spectrum, &f0, &res);
			
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
		if (perfil.perfilMap[i] == '2')
		{   
			span.centerFrequency = CENTER_FREQ2;
			span.frequencyWidth = WIDTH_FREQ2;
			SingleToneSignal (signals[i], samplesPerChannel, perfil.sampling, &span, TONE_EXPORT_NONE, 
		 				&frec, &amp, &fase, signal, &exSP, spectrum, &f0, &res);
			if (amp > ampINF100 && amp < ampSUP100) //Como f0 no se usa, mas que para rellenar la
				f0 = 1; 							//función aterior; pues aqui la usamos.
			else 
				f0 = 0; 
			
			for (j = 0; j < samplesPerChannel; j++)					     
				processed[i][j] = frec*f0;  //en caso de que la señal no cumpla con la amplitud, ésta se lleva a cero.
		}
	SetCtrlVal (panelHandle, PANEL_FREC, frec);
	SetCtrlVal (panelHandle, PANEL_AMPL, amp);
	} //GetKey();
}

void memoryReservation (void)
{
	signals 	= (float64 **) malloc(perfil.numChannels * sizeof(float64 *));
	processed 	= (float64 **) malloc(perfil.numChannels * sizeof(float64 *));
	normalized	= (float64 **) malloc(perfil.numChannels * sizeof(float64 *));
	for ( int i = 0; i < perfil.numChannels; i++)
	{
		signals[i] 	 = (float64 *) malloc(samplesPerChannel * sizeof(float64));
		processed[i] = (float64 *) malloc(samplesPerChannel * sizeof(float64));
		normalized[i]= (float64 *) malloc(samplesPerChannel * sizeof(float64));
	}
}

/***Se requiere generalizar esta funcion, con el fin de que tambien se pueda usar en el entrenamiento
del algoritmo. Se necesita que se le pasen parametros tales como processed[]:    O tambien podriamos 
copiar la informacion obtenida desde el archivo hasta las matrices ke la funcion manipula de mnera global*/

void dataNormalization(void) 
{
/*Mapa de maximos y minimos (según el experto) para la normalizacion. Los indices quedarían organizados como sigue:
  0   	+5	V   {max, min}
  1		-5	V	   "
  2		+12	V	   "
  3		-12 V	   "
  4		+24 V	   "
  5		-24 V	   "
  6		72  V	   "
  7		23  kHz	   "
  8		100 kHz	{max, min}
Esto solo servira si se generalizan estos limites, PERO SI LOS LIMITES FUERAN POR PATA, habría que
hacer un mapa mas complicado que indique los limites por pata y por voltaje esperado en dicha pata*/
float64 mapaMaxsMins [9][2] = {{0.55, 0.0}, {0.0, -0.55},{1.23, 0},{0, -1.25},{2.43, 0},{-1.33, -2.45}, 
					   {7.60, 0.0},{23.5E3, 1.5E3},{100.5E3, 10.5E3}};
	int i, j;
	
	  for (i = 0; i < perfil.numChannels; i++)
	  {   
		  if (perfil.perfilMap[i] == '0')
		  	 for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[7][1])/(mapaMaxsMins[7][0] - mapaMaxsMins[7][1]);
		    
		  if (perfil.perfilMap[i] == '1')
		  	 for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[0][1])/(mapaMaxsMins[0][0] - mapaMaxsMins[0][1]);
		  
		  if (perfil.perfilMap[i] == '2')			  
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[8][1])/(mapaMaxsMins[8][0] - mapaMaxsMins[8][1]);
		  
		  if (perfil.perfilMap[i] == '3')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[1][1])/(mapaMaxsMins[1][0] - mapaMaxsMins[1][1]);
		  
		  if (perfil.perfilMap[i] == '4')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[2][1])/(mapaMaxsMins[2][0] - mapaMaxsMins[2][1]);
		  
		  if (perfil.perfilMap[i] == '5')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[3][1])/(mapaMaxsMins[3][0] - mapaMaxsMins[3][1]);
		  
		  if (perfil.perfilMap[i] == '6')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[4][1])/(mapaMaxsMins[4][0] - mapaMaxsMins[4][1]);
		  
		  if (perfil.perfilMap[i] == '7')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[5][1])/(mapaMaxsMins[5][0] - mapaMaxsMins[5][1]);
		  
		  if (perfil.perfilMap[i] == '8')	
			  for (j = 0; j < samplesPerChannel; j++)
				normalized[i][j] = (processed[i][j] - mapaMaxsMins[6][1])/(mapaMaxsMins[6][0] - mapaMaxsMins[6][1]);
		  
	  }	 //GetKey(); 
}

int CVICALLBACK entrenarFunctionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
		switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
	
}

int CVICALLBACK cargarArchivoFunctionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	uInt64 **numChanValues;
	unsigned int numChannelGroups, *fileNumChannels, numValuesPerChan, i, j, errorCode = 0;
	TDMSChannelHandle **channels;
	TDMSChannelGroupHandle *channelGroups;
	TDMSFileHandle fileHandle;
	char pathFile[MAX_PATHNAME_LEN] = "c:\\";

	switch (event)
	{
		case EVENT_COMMIT:
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
				SetCtrlAttribute (panelHandle, PANEL_TRAIN, ATTR_DIMMED, 0);
				SetCtrlVal (panelHandle, PANEL_LOADEDFILE, pathFile);
			}
			else
			{   
				MessagePopup ("Error message", "Se comprueba la existencia de un error al abrir el archivo especificado.");
			}
			break;
	}
	return 0;
}

