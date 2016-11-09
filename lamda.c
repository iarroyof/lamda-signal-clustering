//==============================================================================
//
// Title:       lamda.c
// Purpose:     Este archivo fuente contiene las funciones que implementan el 
//				algoritmo de reconocimiento de patrones (LAMDA) en sus dife--
//				rentes modalidades, mismas que son: Learning Fron File (LFF),
//				Learning On-Line (LOL) y Recognicing On-line and Learning (ROL).
//
// Created on:  11/03/2013 at 11:39:29 a.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================


//==============================================================================
// Include files
#include "toolbox.h"
#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
// The rest of the include file lies here

//#include "lamda.h"
#include "fileDriving.h"
#include "lamda.h"
#include "User Interface Application.h"



/// HIFN  LAMDA_ Learn Fron File  (aprendizaje desde un archivo, no desde la tarjeta de adquisicion).
/// HIRET Function returns error code to mapping in errorMap[][].
int LAMDA_LFF (void)														 
{																			
	int maxI = 0, minI = 0, i = 0, j = 0, k = 0, err = -1;
	double max = 0, min = 0, L = 0.6, **dataIn;
	FILE *clasesFile, *classLogFile;
	char pathFile2[260] = {0}, bufer[6] = {0}, *clasesCadena;    ;
	
	
	if (FileSelectPopup ("c:\\Users\\utm\\Documents", "*.csv", "", "Seleccione el archivo de CLASES existentes", 
							VAL_LOAD_BUTTON, 0, 1, 1, 0, pathFile))
		{
			if (err = matrixClasesConfig (pathFile, &Nclases, &Ndescriptors) >= 0)
			{
				MessagePopup ("Error de acceso", errorMap[err]);
				return 0;
			}

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
		
		switch (k)
		{
																					//Reservamos memoria para todas las matrices que seran
		  case 1:																	//usadas; dados los parametros proporcionados por
			dataIn = (double **) malloc ((Nsubjects + 1) * sizeof(double *));		//matrixClasesConfig().   
			for (j = 0; j < Nsubjects + 1; j++)
				dataIn[j] = (double *) malloc (Ndescriptors * sizeof(double));        
			clasesCadena  = (char *) malloc(5 * sizeof(char));							//Array para guardar las clases reconocidas  
 			clasesCadena[0] = 0;																		//durante el aprendizaje.
			clasesROs = (double **) malloc((Nclases + 1) * sizeof(double *));		 
			for (i = 0; i < Nclases + 1; i++)										
				clasesROs[i] = (double *) malloc(Ndescriptors * sizeof(double));
		
			MADs = (double **) malloc(Nclases * sizeof(double *));
			for (i = 0; i < Nclases; i++)
				MADs[i] = (double *) malloc((Ndescriptors - 1) * sizeof(double));
		
			GADs = (double *) malloc(Nclases * sizeof(double));
			
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
				GetCtrlVal (panelHandle, PANEL_LAMBDA, &L);
				for (j = 0; j < vacio; j++)
				{
					MaxMin1D (MADs[j], Ndescriptors - 1, &max, &maxI, &min, &minI);
					GADs[j] = L * min + (1 - L) * max;      			//Conectivo hibrido L. C.
				}
			 
				MaxMin1D (GADs, vacio, &max, &maxI, &min, &minI);  	// Decision...
			
				if (maxI == nic) 									//crear nueva clase:
				{
					clasesROs[vacio][0] = 2; 						// ahora sera uno el numero de elementos de esta nueva clase.
					for (j = 1; j < Ndescriptors; j++)
						clasesROs[vacio][j] = 0.5 + ((dataIn[i][j] - 0.5)/ 2);   
					clasesROs = (double **) realloc (clasesROs, (vacio + 2) * sizeof(double *));  //Creando espacio para nueva clase
					clasesROs[vacio + 1] = 0;													  //en todos los vectores:
					clasesROs[vacio + 1] = (double *) realloc (clasesROs[vacio + 1], Ndescriptors * sizeof(double));
					for (j = 0; j < Ndescriptors; j++)
						clasesROs[vacio + 1][j] = 0;	
					
					MADs = (double **) realloc (MADs, (vacio + 1) * sizeof(double *));  
					MADs[vacio] = 0;
					MADs[vacio] = (double *) realloc (MADs[vacio], (Ndescriptors - 1) * sizeof(double));
					for (j = 0; j < Ndescriptors-1; j++)
						MADs[vacio][j] = 0;
					
					GADs = (double *) realloc (GADs, (vacio + 1)* sizeof(double));  
					GADs[vacio] = 0;
					vacio++;
				}
				else  //reforzar aprendizaje de una clase existente:
				{
					clasesROs[maxI][0]++;   					//Se aumenta el numero de elementos que han entrenado a la clase.
					for (j = 1; j < Ndescriptors; j++)			//Entrenamos la clase maxI con el nuevo elemento.
						clasesROs[maxI][j] = clasesROs[maxI][j] + ((dataIn[i][j] - clasesROs[maxI][j])/clasesROs[maxI][0]); 
																//clasesROs[maxI][0] = numero actual de elementos en la clase.
				}
			sprintf(bufer, "%d, ", maxI);
			clasesCadena = (char *) realloc (clasesCadena, strlen(clasesCadena) + (strlen(bufer) + 1) * sizeof(char));    //Alargamos un elemento la cadena de clases
			strcat(clasesCadena, bufer);
			}
			for (j = 0; j < Ndescriptors; j++)						//Agregamos la columna de ceros al final para que corresponda
				clasesROs[vacio][j] = 0;							//el formato del archivo al guardarlo y al abrirlo nuevamente.

		classLogFile = fopen ("c:\\Users\\utm\\Documents\\cadenaClases.csv", "w");
		fputs (clasesCadena, classLogFile);
		fclose(classLogFile);
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
			MessagePopup ("Error generico", errorMap[5]);
		    break;
		}
	return 4;													//Operación exitosa: errorMap[4]
}
/// HIFN  LAMDA_ Learning On Line (aprendizaje en tiempo real). 
/// HIPAR normData/Vector de datos de entrada normalizados.
/// HIPAR claseReconocida/Regresa por referencia la clase reconocida por el algoritmo.
/// HIPAR normDataVectorSize/El numero de elementos que contiene el vector de entrada de datos normalizados.
/// HIRET Function returns error code to mapping in errorMap[][].
int LAMDA_LOL (double *normData, int *claseReconocida, size_t normDataVectorSize)	
{
	int maxI = 0, minI = 0, j = 0, k = 0;
	double max = 0, min = 0, L = 0.6;
	FILE *clasesFile;
	
		
		if (normDataVectorSize != Ndescriptors - 1)									    	//Verificamos si las dimenciones corresponden, en
			return 5; 														//caso negativo se aborta la operación.
		
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
						MADs[j][k] = pow(clasesROs[j][k + 1], normData[k]) * pow((1 - clasesROs[j][k + 1]), 1 - normData[k]);	
													 //es k+1 para descartar el renglon ke no sirve, del de la cantidad de elementos
			// obtenemos las GADs					 //de la clase.
				GetCtrlVal (panelHandle, PANEL_LAMBDA, &L);
				for (j = 0; j < vacio; j++)
				{
					MaxMin1D (MADs[j], Ndescriptors - 1, &max, &maxI, &min, &minI);
					GADs[j] = L * min + (1 - L) * max;      			//Conectivo hibrido L. C.
				}
			 
				MaxMin1D (GADs, vacio, &max, &maxI, &min, &minI);  	// Decision...
				
				if (maxI == nic) 									//crear nueva clase:
				{
					clasesROs[vacio][0] = 2; 						// ahora sera uno el numero de elementos de esta nueva clase.
					for (j = 1; j < Ndescriptors; j++)
						clasesROs[vacio][j] = 0.5 + ((normData[j - 1] - 0.5)/ 2);   // + 0.5 para evitar sobreponderacion de la nueva lcase
					clasesROs = (double **) realloc (clasesROs, (vacio + 2) * sizeof(double *));  //Creando espacio para nueva clase
					clasesROs[vacio + 1] = 0;													  //en todos los vectores:
					clasesROs[vacio + 1] = (double *) realloc (clasesROs[vacio + 1], Ndescriptors * sizeof(double));
					for (j = 0; j < Ndescriptors; j++)
						clasesROs[vacio + 1][j] = 0;	
					
					MADs = (double **) realloc (MADs, (vacio + 1) * sizeof(double *));  
					MADs[vacio] = 0;
					MADs[vacio] = (double *) realloc (MADs[vacio], (Ndescriptors - 1) * sizeof(double));
					for (j = 0; j < Ndescriptors - 1; j++)
						MADs[vacio][j] = 0;
					
					GADs = (double *) realloc (GADs, (vacio + 1)* sizeof(double));  
					GADs[vacio] = 0;
					vacio++;
				}
				else  //reforzar aprendizaje de una clase existente:
				{
					clasesROs[maxI][0]++;   					//Se aumenta el numero de elementos que han entrenado a la clase.
					k = clasesROs[maxI][0];
					for (j = 1; j < Ndescriptors; j++)			//Entrenamos la clase maxI con el nuevo elemento.
						clasesROs[maxI][j] = clasesROs[maxI][j] + ((normData[j - 1] - clasesROs[maxI][j])/ k); 
																	//clasesROs[maxI][0] = numero actual de elementos en la clase.
				}
			*claseReconocida = maxI;
			//---------------------------------------------------------------------------------------------------------------------------
			for (j = 0; j < Ndescriptors; j++)						//Agregamos la columna de ceros al final para que corresponda
				clasesROs[vacio][j] = 0;							//el formato del archivo al guardarlo y al abrirlo nuevamente.
				
	return 4;													//Operación exitosa: errorMap[4]
}
/// HIFN  LAMDA_ Recognition On Line (reconocimiento de patrones On line).
/// HIPAR normData/Vector de datos de entrada normalizados.
/// HIPAR claseReconocida/Regresa por referencia la clase reconocida por el algoritmo.
/// HIPAR normDataVectorSize/El numero de elementos que contiene el vector de entrada de datos normalizados.
/// HIRET Function returns error code to mapping in errorMap[][].
int LAMDA_ROL (double *normData, int *claseReconocida, size_t normDataVectorSize)  
{
	int maxI = 0, minI = 0, j = 0, k = 0;
	double max = 0, min = 0, L = 0.6;
	FILE *clasesFile;
	
	
	if (normDataVectorSize != Ndescriptors - 1)							//Verificamos si las dimenciones corresponden, en
		return 5; 														//caso negativo se regresa el error y se aborta la operación.
	
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
			MADs[j][k] = pow(clasesROs[j][k + 1], normData[k]) * pow((1 - clasesROs[j][k + 1]), 1 - normData[k]);	
									 //es k+1 para descartar el renglon ke no sirve, del de la cantidad de elementos
	// obtenemos las GADs			 //de la clase.
	GetCtrlVal (panelHandle, PANEL_LAMBDA, &L);
	for (j = 0; j < vacio; j++)
	{
		MaxMin1D (MADs[j], Ndescriptors - 1, &max, &maxI, &min, &minI);
		GADs[j] = L * min + (1 - L) * max;     			//Conectivo hibrido L. C.
	}
		 
	MaxMin1D (GADs, vacio, &max, &maxI, &min, &minI);  	// Decision...
	*claseReconocida = maxI;
		//---------------------------------------------------------------------------------------------------------------------------
				
	return 4;													//Operación exitosa: errorMap[4]
	
}



