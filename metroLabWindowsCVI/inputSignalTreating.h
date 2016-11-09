//==============================================================================
//
// Title:       inputSignalTreating.h
// Purpose:     A short description of the interface.
//
// Created on:  11/04/2013 at 01:57:02 p.m. by utm.
// Copyright:   Microsoft. All Rights Reserved.
//
//==============================================================================

#ifndef __inputSignalTreating_H__
#define __inputSignalTreating_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types
struct profileInfo{
	int numChannels;
	int dataSize;
	int  running;
	char  *perfilMap;
	double sampling;
} perfil;
//==============================================================================
// External variables
extern int panelHandle;
extern double **normalized, **mapaMaxsMins;
//==============================================================================
// Global functions

int taskSelectingAndSignalMapping (int cajon);
int ProcessingProfile (void);
void stopTask(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __inputSignalTreating_H__ */
