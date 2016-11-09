//==============================================================================
//
// Title:       fileDriving.c
// Purpose:     A short description of the implementation.
//
// Created on:  11/03/2013 at 12:07:08 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================


//==============================================================================
// Include files


// The rest of the include file lies here
#include "toolbox.h"
#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>

//#include "fileDriving.h"
#include "lamda.h"
#include "fileDriving.h"

#define MAX_STATE_STRING_LENGTH 50

/// HIFN  Esta función jala (por referencia) los parametro del archivo de clases, mismos que estan contenidos en la cabecera del mismo. Dichos parametros son unicamente el numero de descriptores y el numero clases y el numero de descriptores
/// HIPAR path/Es una cadena que contiene la ruta del archivo que se va a accesar.
/// HIPAR clases/Con este apuntador se regresa por referencia el numero de clases que se encuentran en el archivo accesado.
/// HIPAR descriptores/Con este apuntador se regresa por referencia el numero de descriptores mas uno (por el renglon de elementos) que se encuentran en el archivo accesado.
/// HIRET Regresa el codigo de error (>= 0) en caso de haberlo; -1 si no hay errores.
int matrixClasesConfig (char *path, int *clases, int *descriptores)
{	 //05100000053
	 // 3      8   11
	FILE *archivo;
	char cabecera[12] = {0}, r[4] = {0}, c[9] = {0};
	int i;
	
	archivo = fopen(path, "r");
	
	if (archivo == NULL || path == "")											//Error Handling   ;)
		return 0;
		
	fgets(cabecera, 12, archivo);  // 7 caracteres que incluyen 6 digitos mas el fin de cadena
	if (strlen(cabecera) != 11)    // Verificar integridad de la cabecera, en caso negativo 
		return 2;				  // terminar funcion y regresar error.

	for (i = 0; i < 3; i++)
		r[i] = cabecera[i];
	r[i] = '\0';
	for (i = 0; i < 8; i++)
		c[i] = cabecera[i + 3];
	c[i] = '\0';
	*descriptores = atoi(r);
	*clases       = atoi(c);
	
	fclose(archivo);
	
	return -1;					  // -1 = there weren't errors
}

/// HIFN   This function almacena la matriz con clases nuevas mas las ya existentes reforzadas en el archivo cuya ruta sea path.
/// HIPAR  path/Es una cadena que contiene la ruta del archivo que se va a accesar.
/// HIPAR  clusterMatrix/Apunta la matriz que contiene el conocimiento existente mas el nuevo (en caso de haberlo). Además de una columna de ceros que solo cumple con el formato establecido.
/// HIPAR  Ncls/Número de clases que contiene la matriz de conocimiento.
/// HIPAR  Ndtrs/Número de descriptores que contiene la matriz de conocimiento.
/// HIRET  Regresa el codigo de error (>= 0) en caso de haberlo; -1 si no hay errores.
int storeKnoledgeFile (char *path, double **clusterMatrix, int Ncls, int Ndtrs)
{								   //[clases][descripteurs]
	FILE *archivo;
	char bufferA[13] = {0}, *bufferB, bufF[9]= "0000" ,bufG[9]= "000",bufH[9]= "00000" ,bufI[9]= "000000" ,bufJ[9]= "0000000" , bufA[9]= "00", bufB[9] = "0", bufC[4] = "", bufD[13] = "", bufE[9] = "";
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
			strcat (bufJ, bufC);
			strcpy (bufE, bufJ);
		break;
		case 2:
			strcat (bufI, bufC);
			strcpy (bufE, bufI);
		break;
		case 3:
			strcat (bufH, bufC);
			strcpy (bufE, bufH);
		break;
		case 4:
			strcat (bufF, bufC);
			strcpy (bufE, bufF);
			break;
		case 5:
			strcat (bufG, bufC);
			strcpy (bufE, bufG);
			break;
		case 6:
			strcat (bufA, bufC);
			strcpy (bufE, bufA);
			break;
		case 7:
			strcat (bufB, bufC);
			strcpy (bufE, bufB);
			break;
		case 8:
			strcpy (bufE, bufC);
			break;
	}
	
	strcat (bufD, bufE);
	fputs (bufD, archivo);
	fputc (10, archivo);
	//SetCtrlVal (panelHandle, PANEL_CLASES_STORED, bufD);
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

/// HIFN  Esta funcion sirve para cargar el contenido del archivo de las cases en la matriz de clases. Dicho archivo debe estar en formato CSV (Coma Separated Values).
/// HIPAR path/Es una cadena que contiene la ruta del archivo que se va a accesar.
/// HIPAR matrizClases/Apunta a la matriz donde será cargado el contenido del archivo accesado.
/// HIPAR Ncl/Indica a la función el numero de clases que contiene el archivo que se va accesar.
/// HIPAR Nds/Indica a la función el numero de descriptores que contiene el archivo que se va accesar (considerando claseNIC y sin considerar la columna de ceros).
/// HIRET Regresa el codigo de error (>= 0) en caso de haberlo; -1 si no hay errores.
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
/// HIFN  This fuction returns a char matrix containing at each row
//        strings that label registered states.
/// HIPAR path/ It is the path in which is the states file.
/// HIPAR point/ It is the number of states got by the function from states file. 
char **loadStates (char *path, int *point)
{
	FILE *archivo;
	int i = 0;
	char **stateMatrix;
	
	stateMatrix = (char **) malloc (Nclases * sizeof(char *));
	for (i = 0; i < Nclases; i++)
		stateMatrix[i] = (char *) malloc (MAX_STATE_STRING_LENGTH * sizeof(char));
	
	archivo = fopen(path, "r");
	
	for (i = 0; !feof(archivo) && i < Nclases; i++)
		fgets (stateMatrix[i], MAX_STATE_STRING_LENGTH, archivo);
	
	stateMatrix = (char **) realloc (stateMatrix, i * sizeof(char *));
	
	fclose (archivo);
	*point = i;
	return  stateMatrix;
}
