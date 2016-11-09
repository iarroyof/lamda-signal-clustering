//==============================================================================
//
// Title:       pruebaLAMDA
// Purpose:     A short description of the application.
//
// Created on:  24/02/2013 at 12:56:56 a.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "pruebaLAMDA.h"
#include "toolbox.h"

//==============================================================================
// Constants
#define nic			 0
//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables
int Ndescriptors = 0;
int Nsubjects	 = 1;
int Nclases		 = 0;
double **MADs, *GADs, **clasesROs;
char errorMap[4][100] = {"No se pudo abrir el archivo de Clases Eexistentes", 
						 "No se pudo abrir el archivo de Sujetos a Observar", 
						 "No se pudo actualizar el archivo de clases Existentes",
						 "Cabecera de archivo incorrecta"};
//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "pruebaLAMDA.uir", PANEL));
    
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
int matrixClasesConfig (char *path, int *sujetos, int *descriptores);
int loadFile(char *path, double **matrizClases, int Ncl, int Nds);
int storeKnoledgeFile (char *path, double **clusterMatrix, int Ncls, int Ndtrs);
int LAMDA (char recLearn);
/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int CVICALLBACK diagFunctionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK aprenderFunctionButton (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{

	if (event == EVENT_COMMIT)
	{
		LAMDA ('r');
	}
	return 0;
}
/*Esta funcion sirve para cargar el contenido del archivo de las cases en la matriz de clases. Dicho archivo debe estar en
  formato CSV (Coma Separated Values)*/
int loadFile(char *path, double **matrizClases, int Ncl, int Nds)
{
	//(((Ncl * 13)+13) + Ncl + 1)->(((numeroDeClases{Ncl} * NoDeDigitosDeUnDouble{13}) + NoDeDigitosDeUnDouble{13[claseNIC]}) + (numeroDeComas{Ncl-1}) + TerminadorNulo{1})
	FILE *archivo;
	char *numero, **nums, num[14], c = 0, listo = 0;   
	int i = 0, j = 0, k = 0;
													  		//mas el termination character
	numero = (char *) malloc ((((Ncl * 13)+13) + Ncl + 1) * sizeof(char));
	
	nums = (char **) malloc ((Nds + 1) * sizeof(char *));
	for (i = 0; i < Nds + 1; i++)
		nums[i] = (char *) malloc((((Ncl * 13)+13) + Ncl + 1) * sizeof(char));
	
	archivo = fopen(path, "r");
	
	if (archivo == NULL)											//Error Handling   ;)
		return 0;
	
	fgets(numero, 13, archivo);     						//nos brincamos la cabecera pues ya no se usa

	for (i = 0; i < Nds; i++)     						//Revisamos si no se encuentra el fin de archivo
	{
		fgets(numero, ((Ncl * 13)+13) + Ncl + 1, archivo); 
		
		for (j = 0; ; j++)
		{
			if((numero[j] != 10) && (numero[j] != 0))		//jalamos cada renglon siguiente del archivo
				nums[i][j] = numero[j]; 					// y los guardamos en cada renglon de nums[][].
			else
				break;
		}
		nums[i][j] = '\0';									// se delimita cada renglon con el terminador nulo
	} 
	int m = 0;	
	for (i = 0; i < Nds; i++)								//Hacemos esto para todos los descriptores + el num de elem 
	{														//de cada clase 
		k = 0;
		for (j = 0; j < strlen (nums[i]) + 1; j++)			//Hacemos esto mientras no alcancemos la longitud de cadena
		{
			if (nums[i][j] != 44 && nums[i][j] != 0)
			{
				num[m] = nums[i][j];						//Obtenemos cada caracter y lo guardamos en num, mientras
				m++;										//no se aparezca una coma o un terminador nulo.
			}
			else
			{
				num[m] = '\0';							   //Una vez que aparece una coma o un terminador nulo terminamos 
				matrizClases[k][i] = atof(num);			   //la cadena que se esta construyendo, la convertimos en double
				k++;									   //y la guardamos en la matriz de clases que se regresa por 
				m = 0;									   //referencia
			}
		}
	
	}													   
	fclose(archivo);
	return -1;
}

// Esta función jala (por referencia) los parametro del archivo de clases, mismos que estan contenidos en la cabecera 
// del mismo. Dichos parametros son unicamente el numero de descriptores y el numero clases y el numero de descriptores
int matrixClasesConfig (char *path, int *clases, int *descriptores)
{
	FILE *archivo;
	char cabecera[7] = {0}, r[4] = {0}, c[4] = {0};
	int i;
	
	archivo = fopen(path, "r");
	
	if (archivo == NULL || path == "")											//Error Handling   ;)
		return 0;
		
	fgets(cabecera, 7, archivo);  // 7 caracteres que incluyen 6 digitos mas el fin de cadena
	if (strlen(cabecera) != 6)    // Verificar integridad de la cabecera, en caso negativo 
		return 2;				  // terminar funcion y regresar error.

	for (i = 0; i < 3; i++)
		r[i] = cabecera[i];
	r[i] = '\0';
	for (i = 0; i < 3; i++)
		c[i] = cabecera[i + 3];
	c[i] = '\0';
	*descriptores = atoi(r);
	*clases       = atoi(c);
	
	fclose(archivo);
	
	return -1;					  // -1 = there wasn't errors
}

int storeKnoledgeFile (char *path, double **clusterMatrix, int Ncls, int Ndtrs)
{								   //[clases][descripteurs]
	FILE *archivo;
	char bufferA[13] = {0}, *bufferB, bufA[7]= "00", bufB[7] = "0", bufC[4] = "", bufD[13] = "", bufE[7] = "";
	int i = 0, j = 0;
	
	bufferB = (char *) malloc ((((Ncls * 13)+13) + Ncls + 1) * sizeof(char));
	
	archivo = fopen (path, "w");
	if (archivo == NULL || path == "")											//Error Handling   ;)
		return 2;
	//Vamos primero a colocar la cabezera (primero el numero de descriptores):
	sprintf (bufC, "%d", Ndtrs);
	int L = strlen (bufC);
	
	switch (L)
	{
		case 1:
			strcat (bufA, bufC);
			strcpy (bufD, bufA);
			break;
		case 2:
			strcat (bufB, bufC);
			strcpy (bufD, bufB);
			break;
		case 3:
			strcpy (bufD, bufC);
			break;
	}
	
	sprintf (bufC, "%d", Ncls);
	sprintf (bufA, "00");
	sprintf (bufB, "0");
	L = strlen (bufC);
	switch (L)
	{
		case 1:
			strcat (bufA, bufC);
			strcpy (bufE, bufA);
			break;
		case 2:
			strcat (bufB, bufC);
			strcpy (bufE, bufB);
			break;
		case 3:
			strcpy (bufE, bufC);
			break;
	}
	
	strcat (bufD, bufE);
	fputs (bufD, archivo);
	fputc (10, archivo);
	SetCtrlVal (panelHandle, PANEL_CLASES_STORED, bufD);
	//Ahora imprimimos datos al archivo.
	
	bufferB[0] = '\0';
	for (i = 0; i < Ndtrs; i++)									//File writing is performed by this loop until reaching the number  
	{															//of descriptors (Ndtrs).
		for(j = 0, bufferB[0] = '\0'; j <= Ncls; j++)			//clusterMatrix[][] contains the descriptor values of each class.
		{														//Each value is printed as %f string inside bufferA. After that,  
			sprintf (bufferA, "%f,", clusterMatrix[j][i]);		//bufferA is concatenated with the previous bufferA inside bufferB.
			strcat (bufferB, bufferA);							//All this values are comma separated ("%f,"). This loop is repeated
		}														//until reaching the number of classes (Ncls) since that 
																//matrixCluster[][] is treated row by row in this loop.
		bufferB[strlen (bufferB)-1] = 0;       					//Null Terminator instead last ',' in the string
		fputs (bufferB, archivo);			   					//Prints the complete string in the file
		fputc (10, archivo);				   					//Prints ENTER at the end of the printed string
	}
	
	fclose (archivo);
	return -1;
}

int LAMDA (char recLearn)
{
	int vacio = 0, maxI = 0, minI = 0, i = 0, j = 0, k = 0, *clasesCadena, err = -1;
	double max = 0, min = 0, L = 0.6, **dataIn;
	FILE *clasesFile;
	char pathFile[260] = {0}, pathFile2[260] = {0}, bufer[6] = {0};
	
	
	if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile))
		{
			if (err = matrixClasesConfig (pathFile, &Nclases, &Ndescriptors) >= 0)
			{
				MessagePopup ("Error de acceso", errorMap[err]);
				return 0;
			}
			SetCtrlVal (panelHandle, PANEL_CLASES_READED, Nclases);
			if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de OBJETOS a observar", 
						 VAL_LOAD_BUTTON, 0, 1, 1, 0,pathFile2))
			{
				if (err = matrixClasesConfig (pathFile2, &Nsubjects, &j) >= 0)
				{
					MessagePopup ("Error de acceso", errorMap[err]);
					return 0;
				}
			}
			else
				return 0;
		}
		else
			return 0;
		

		
		if (j == Ndescriptors)									    		//Verificamos si las dimenciones corresponden, en
			k = 1;															//caso negativo se aborta la operación.
		else k = 0;
		int n = j;
		dataIn = (double **) malloc ((Nsubjects + 1) * sizeof(double *));
		for (j = 0; j < Nsubjects + 1; j++)
			dataIn[j] = (double *) malloc (Ndescriptors * sizeof(double));
		
		switch (k)
		{
		
		  case 1:																	//Reservamos memoria para todas las matrices que seran  
			clasesROs = (double **) malloc((Nclases + 1) * sizeof(double *));		//usadas; dados los parametros proporcionados por 
			for (i = 0; i < Nclases + 1; i++)										//matrixClasesConfig().
				clasesROs[i] = (double *) malloc(Ndescriptors * sizeof(double));
		
			MADs = (double **) malloc(Nclases * sizeof(double *));
			for (i = 0; i < Nclases; i++)
				MADs[i] = (double *) malloc(Ndescriptors * sizeof(double));
		
			GADs = (double *) malloc(Nclases * sizeof(double));
			
			clasesCadena  = (int *) malloc(sizeof(int));							//Array para guardar las clases reconocidas  
 																					//durante el aprendizaje.
			if (err = loadFile(pathFile, clasesROs, Nclases, Ndescriptors) >= 0)	//Cargamos el archivo de clases existentes.
			{
				MessagePopup ("Error de acceso", errorMap[err]);
				return 0;
			}
			if (err = loadFile(pathFile2, dataIn, Nsubjects, Ndescriptors) >= 0)	//Cargamos el archivo de señales de entrada
			{
				MessagePopup ("Error de acceso", errorMap[err]);
				return 0;
			}
			
			for (i = 0; i < Nsubjects; i++)			// <---Esto solo se usaria al aprender desde un archivo con un numero fijo de sujetos de
			{										// entrada. De otro modo, Nsubjects seria indefinido, durante el tiempo que dure la prueba.
				clasesCadena = (int *) realloc (clasesCadena, (i + 1)* sizeof(int));    //Alargamos un elemento la cadena de clases
																//reconocidas.
				for (j = 1; k < Nclases + 1; j++)  	//Escaneamos la matriz de clases existentes para saber cuantas hay
				{
					if (clasesROs[j][0] == 0) 		//Solo revisamos el elemento 0 si hubiese mas clases, deberian estar contiguas
					{								//Aunque esto ya no es necesario porque matrixClasesConfig() ha obtenido sus parametros
						vacio = j;					//vacio es el indice de la ultima clase; vacio + 1 es el indice de la primer  
						break;						//clase vacia.
					} 
				} 
				//Obtenemos las MADs
				for (j = 0; j < vacio; j++)
					for (k = 0; k < Ndescriptors - 1; k++)
						MADs[j][k] = pow(clasesROs[j][k + 1], dataIn[i][k+1]) * pow((1 - clasesROs[j][k + 1]), 1 - dataIn[i][k+1]);	
													 //es k+1 para descartar el renglon ke no sirve, del de la cantidad de elementos
			// obtenemos las GADs					 //de la clase.
				GetCtrlVal (panelHandle, PANEL_ALPHA, &L);
				for (j = 0; j < vacio - 1; j++)
				{
					MaxMin1D (MADs[j], Ndescriptors - 1, &max, &maxI, &min, &minI);
					GADs[j] = L * max + (1 - L) * min;     			//Conectivo hibrido L. C.
				}
			 
				MaxMin1D (GADs, vacio, &max, &maxI, &min, &minI);  	// Decision...
			
				if (maxI == nic) 									//crear nueva clase:
				{
					clasesROs[vacio][0] = 1; 						// ahora sera uno el numero de elementos de esta nueva clase.
					for (j = 1; j < Ndescriptors; j++)
						clasesROs[vacio][j] = 0.5 + ((dataIn[i][j - 1] - 0.5)/ 2.5);   // + 0.5 para evitar sobreponderacion de la nueva lcase
					clasesROs = (double **) realloc (clasesROs, (vacio + 2) * sizeof(double *));  //Creando espacio para nueva clase
					clasesROs[vacio + 1] = 0;													  //en todos los vectores:
					clasesROs[vacio + 1] = (double *) realloc (clasesROs[vacio + 1], Ndescriptors * sizeof(double));
					for (j = 0; j < Ndescriptors; j++)
						clasesROs[vacio + 1][j] = 0;	
					
					MADs = (double **) realloc (MADs, (vacio + 1) * sizeof(double *));  
					MADs[vacio] = 0;
					MADs[vacio] = (double *) realloc (MADs[vacio], Ndescriptors * sizeof(double));
					for (j = 0; j < Ndescriptors; j++)
						MADs[vacio][j] = 0;
					
					GADs = (double *) realloc (GADs, (vacio + 1)* sizeof(double));  
					GADs[vacio] = 0;
					vacio++;
				}
				else  //reforzar aprendizaje de una clase existente:
				{
					if (clasesROs[maxI][0] < 100)  // Si el numero de elementos en la clase es mayor a 100 ya no hay reforzamiento
					{							   // significativo, de modo que no tiene caso actualizar a RO.
						clasesROs[maxI][0]++;   					//Se aumenta el numero de elementos que han entrenado a la clase.
						for (j = 1; j < Ndescriptors; j++)			//Entrenamos la clase maxI con el nuevo elemento.
							clasesROs[maxI][j] = clasesROs[maxI][j] + ((dataIn[i][j - 1] - clasesROs[maxI][j])/ (clasesROs[maxI][0] + 1)); 
					}												//clasesROs[maxI][0] = numero actual de elementos en la clase.
				}
				clasesCadena[i] = maxI;
			}
			for (j = 0; j < Ndescriptors; j++)						//Agregamos la columna de ceros al final para que corresponda
				clasesROs[vacio][j] = 0;							//el formato del archivo al guardarlo y al abrirlo nuevamente.
			pathFile2[0] = 0;										//Uso de pathFile[] Para no crear otra variable jjeje
			for (j = 0; j < i; j++)
			{														//Este ciclo forma en una cadena el registro de clases recono-
				sprintf (bufer,"%d, ",clasesCadena[j]);				//cidas durante el aprendizaje: 
				strcat (pathFile2, bufer);							//int *clasesCadena ->> char *pathFile2 para ser mostrado
			}
			SetCtrlVal (panelHandle, PANEL_CADENACLASES, pathFile2);//Solo muestra la cadena de clases reconocidas en el StringCtrl
			
			storeKnoledgeFile (pathFile, clasesROs, vacio, Ndescriptors); //This function almacena la matriz con clases nuevas y las
		    															  //ya existentes reforzadas en el mismo archivo pathFile.
			//Procedemos a liberar toda la memoria reservada para no generar replicas de cualquier tipo o datos indeseables en en-
			//trenamientos posteriores:
			for (i = 0; i < vacio + 1; i++)
				free(clasesROs[i]);
			free (clasesROs);
			for (i = 0; i < vacio; i++)
				free(MADs[i]);
			free (MADs);
			free (GADs);
			free (clasesCadena);
				
			break;
		  case 0:
			MessagePopup ("Error generico", "La dimension del espacio de entrada no es la requerida");
		    break;
		}
	return 0;
}

