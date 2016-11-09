//==============================================================================
//
// Title:       lamda.h
// Purpose:     A short description of the interface.
//
// Created on:  11/03/2013 at 11:39:29 a.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

#ifndef __lamda_H__
#define __lamda_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
#define nic			 0
#define N_OPERATIONS_TO_SAVE 100
//==============================================================================
// External variables

extern double normalized[][];
extern int Ndescriptors;
extern int Nsubjects;
extern int Nclases;
extern int online;
extern int vacio;
extern char clasesRecs[];
extern char cl[5];
extern FILE *classLogFile;
extern char pathFile[260];
extern double **MADs, *GADs, **clasesROs;
extern char errorMap[][100];
extern int panelHandle;
   
//==============================================================================
// Global functions
int LAMDA_LFF (void);
int LAMDA_LOL (double *normData, int *claseReconocida, size_t normDataVectorSize);
int LAMDA_ROL (double *normData, int *claseReconocida, size_t normDataVectorSize);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __lamda_H__ */
