//==============================================================================
//
// Title:       fileDriving.h
// Purpose:     A short description of the interface.
//
// Created on:  11/03/2013 at 12:07:08 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

#ifndef fileDriving_H__
#define fileDriving_H__

#ifdef __cplusplus
    extern "C" {
#endif


//==============================================================================
// External variables
//==============================================================================

int matrixClasesConfig (char *path, int *clases, int *descriptores);
int storeKnoledgeFile (char *path, double **clusterMatrix, int Ncls, int Ndtrs);
int loadFile(char *path, double **matrizClases, int Ncl, int Nds);
char **loadStates (char *path, int *);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __fileDriving_H__ */
