//**************************************************************************
//* WARNING: This file was automatically generated.  Any changes you make  *
//*          to this file will be lost if you generate the file again.     *
//**************************************************************************
#ifndef DAQTASKPRUEBA_INCLUDE
#define DAQTASKPRUEBA_INCLUDE
#include <NIDAQmx.h>

#ifdef __cplusplus
	extern "C" {
#endif

//**************************************************************************
//* This generated function configures your DAQmx task.                    *
//*                                                                        *
//* Follow these steps to use this generated function:                     *
//*   1) Define a task handle variable in your program.                    *
//*   2) Call the generated function.                                      *
//*   3) Use the returned task handle as a parameter to other DAQmx        *
//*      functions.                                                        *
//*   4) Clear the task handle when you finish.                            *
//*                                                                        *
//*         TaskHandle task = 0;                                           *
//*         CreateDAQTask(&task);                                          *
//*         <use the DAQmx task handle>                                    *
//*         DAQmxClearTask(task);                                          *
//*                                                                        *
//**************************************************************************
int32 CreateDAQTaskPrueba(TaskHandle *taskOut1);

#ifdef __cplusplus
	}
#endif

#endif // ifndef DAQTASKPRUEBA_INCLUDE
