#include "pruebaDAQ.h"

//**************************************************************************
//* WARNING: This file was automatically generated.  Any changes you make  *
//*          to this file will be lost if you generate the file again.     *
//**************************************************************************
#include <userint.h>
#include <ansi_c.h>
#include <utility.h>			   
#include <analysis.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( (DAQmxError=(functionCall))<0 ) goto Error; else
#define DAQmxNullChk(functionCall) if( (functionCall)==NULL ) { DAQmxError=DAQmxErrorPALMemoryFull; goto Error; } else
#define DAQmxReportErr(statusExpression) \
{ \
	int32 __DAQmxError = (statusExpression); \
	if (DAQmxFailed(__DAQmxError) && !GetBreakOnLibraryErrors()) \
	{ \
		char gErrBuf[512]; \
		DAQmxGetErrorString(__DAQmxError,gErrBuf,512); \
		MessagePopup("DAQmx Error",gErrBuf); \
	} \
}

#define MAX_BUTTONS		5
#define BUTTON_WIDTH	70
#define BUTTON_HEIGHT	30

#define kStart			"__Start"
#define kStop			"Sto__p"
#define kDone			"__Done"
typedef struct {
	TaskHandle	task;
	int32		numSampsPerChan;
	int			startCtrl;
	int			stopCtrl;
	int			chartCtrl;
	int			timerCtrl;
	int			running;
	float64		timeout;
} StartReadWfmLoopStopInfo;
typedef StartReadWfmLoopStopInfo *StartReadWfmLoopStopInfoPtr;

static int CVICALLBACK TimerCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2);
static int CVICALLBACK StartCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2);
static int CVICALLBACK StopCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2);
static int CVICALLBACK DoneCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2);
static int32 BuildStartReadWfmLoopStopUI(TaskHandle taskIn, StartReadWfmLoopStopInfo *info,
	int32 *panel);
static int32 GetChannelNames (TaskHandle task, uInt32 numChannels, char ***channelNames);
static void FreeChannelNames (char **channelNames, uInt32 numChannels);
static void	PositionControlsOnPanel (int panel, int numLegends, int mainCtrl, ...);

//**************************************************************************
//* This generated function demonstrates how to run your DAQmx task.       *
//*                                                                        *
//* Call this function directly after the code that loads or configures    *
//* your DAQmx task.                                                       *
//*                                                                        *
//* This example code is designed as a generic demonstration. Because of   *
//* this, the example code might be more complex than many applications    *
//* require. For example, many tasks can contain multiple channels or can  *
//* read or write a varying number of samples. In these cases the example  *
//* code is general enough to handle all possible values. Code designed    *
//* for a fixed number of channels or samples would be simpler. In many    *
//* cases there is also additional code to build the user interface and    *
//* display the results.                                                   *
//*                                                                        *
//* The key DAQmx code has been marked with "Example Core" comments. You   *
//* can search for "Example Core" to find this code.                       *
//**************************************************************************
int32 RunpruebaVoltageTask(TaskHandle taskin)
{
	int32 DAQmxError = DAQmxSuccess;
	int	panel;
	StartReadWfmLoopStopInfo info;

	info.numSampsPerChan = 100;
	info.timeout = 10;
	DAQmxErrChk(BuildStartReadWfmLoopStopUI(taskin, &info, &panel));

	DisplayPanel(panel);
	RunUserInterface();
	DiscardPanel(panel);

Error:
	return DAQmxError;
}

//**************************************************************************
//* The following callback functions control the execution of the DAQmx    *
//* task.                                                                  *
//**************************************************************************
static int CVICALLBACK TimerCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	StartReadWfmLoopStopInfoPtr	info = (StartReadWfmLoopStopInfoPtr)callbackData;
	int32 DAQmxError = DAQmxSuccess;
	float64 *data = NULL;
	uInt32 numChannels, dataSize;

	if( event==EVENT_TIMER_TICK )
	{
		// Example Core: Allocate a buffer of appropriate size.  If you know
		// the necessary buffer size at compile time you can declare an array
		// of fixed size on the stack instead of allocating one.
		DAQmxErrChk(DAQmxGetTaskAttribute (info->task, DAQmx_Task_NumChans, &numChannels));
		dataSize = numChannels * info->numSampsPerChan;
		DAQmxNullChk(data = (float64 *)malloc (dataSize * sizeof(float64)));

		// Example Core: Read the specified number of samples from each channel.
		DAQmxErrChk(DAQmxReadAnalogF64 (info->task, info->numSampsPerChan, info->timeout,
			DAQmx_Val_GroupByScanNumber, data, dataSize, NULL, 0));

		PlotStripChart (panel, info->chartCtrl, data, dataSize, 0, 0, VAL_DOUBLE);
	}

Error:
	if (data)
		free (data);
	if (DAQmxFailed(DAQmxError))
	{
		DAQmxReportErr(DAQmxError);
		StopCB_StartReadWfmLoopStop (panel, info->stopCtrl, EVENT_COMMIT, callbackData, 0, 0);
	}
	return 0;
}

static int CVICALLBACK StartCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	StartReadWfmLoopStopInfoPtr info = (StartReadWfmLoopStopInfoPtr)callbackData;
	int32 DAQmxError = DAQmxSuccess;

	if( event==EVENT_COMMIT && !info->running )
	{
		// Example Core: Start the task.
		DAQmxErrChk(DAQmxStartTask(info->task));

		SetCtrlAttribute (panel, info->timerCtrl, ATTR_ENABLED, 1);
		info->running = 1;
	}

Error:
	DAQmxReportErr(DAQmxError);
	return 0;
}

static int CVICALLBACK StopCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	StartReadWfmLoopStopInfoPtr info = (StartReadWfmLoopStopInfoPtr)callbackData;
	int32 DAQmxError = DAQmxSuccess;

	if( event==EVENT_COMMIT && info->running )
	{
		info->running = 0;
		SetCtrlAttribute (panel, info->timerCtrl, ATTR_ENABLED, 0);

		// Example Core: Stop the task.
		DAQmxErrChk(DAQmxStopTask(info->task));
	}

Error:
	DAQmxReportErr(DAQmxError);
	return 0;
}

static int CVICALLBACK DoneCB_StartReadWfmLoopStop (int panel, int control, int event,
	void *callbackData, int eventData1, int eventData2)
{
	StartReadWfmLoopStopInfoPtr	info = (StartReadWfmLoopStopInfoPtr)callbackData;

	if( event==EVENT_COMMIT )
	{
		StopCB_StartReadWfmLoopStop (panel, info->stopCtrl, EVENT_COMMIT, callbackData, 0, 0);
		QuitUserInterface (0);
	}

	return 0;
}

//**************************************************************************
//* The following functions build and manage the user interface.           *
//**************************************************************************
static int32 BuildStartReadWfmLoopStopUI(TaskHandle taskIn, StartReadWfmLoopStopInfo *info,
	int32 *panel)
{
	int32 DAQmxError = DAQmxSuccess;
	float64	max, min, highestMax = 0, lowestMin = 0;
	int32 panelTemp, doneCtrl, chartCtrl, timerCtrl, startCtrl, stopCtrl;
	uInt32 numChannels;
	char **channelNames = NULL;
	unsigned int i;

	DAQmxErrChk(DAQmxGetTaskAttribute (taskIn, DAQmx_Task_NumChans, &numChannels));

	panelTemp = NewPanel (0, "DAQmx Example", 0, 0, 10, 10);
	SetPanelAttribute (panelTemp, ATTR_CONFORM_TO_SYSTEM, TRUE);
	SetPanelAttribute (panelTemp, ATTR_CAN_MAXIMIZE, FALSE);
	SetPanelAttribute (panelTemp, ATTR_SIZABLE, FALSE);

	chartCtrl = NewCtrl (panelTemp, CTRL_STRIP_CHART_LS, "", 10, 10);
	SetCtrlAttribute (panelTemp, chartCtrl, ATTR_NUM_TRACES, numChannels);

	startCtrl = NewCtrl (panelTemp, CTRL_SQUARE_COMMAND_BUTTON_LS, kStart, 10, 10);

	stopCtrl = NewCtrl (panelTemp, CTRL_SQUARE_COMMAND_BUTTON_LS, kStop, 10, 10);

	doneCtrl = NewCtrl (panelTemp, CTRL_SQUARE_COMMAND_BUTTON_LS, kDone, 10, 10);
	SetPanelAttribute (panelTemp, ATTR_CLOSE_CTRL, doneCtrl);

	timerCtrl = NewCtrl (panelTemp, CTRL_TIMER, "", 0, 0);

	// Obtain maximum and minimum values across all channels.
	DAQmxErrChk(GetChannelNames (taskIn, numChannels, &channelNames));
	for (i = 0; i < numChannels; i++)
	{
		DAQmxErrChk(DAQmxGetChanAttribute (taskIn, channelNames[i], DAQmx_AI_Min, &min));
		if (i == 0 || min < lowestMin)
			lowestMin = min;
		DAQmxErrChk(DAQmxGetChanAttribute (taskIn, channelNames[i], DAQmx_AI_Max, &max));
		if (i == 0 || max > highestMax)
			highestMax = max;
	}
	SetAxisScalingMode (panelTemp, chartCtrl, VAL_LEFT_YAXIS, VAL_MANUAL, lowestMin, highestMax);

	info->task = taskIn;
	info->startCtrl = startCtrl;
	info->stopCtrl = stopCtrl;
	info->chartCtrl = chartCtrl;
	info->timerCtrl = timerCtrl;
	info->running = 0;

	SetCtrlAttribute (panelTemp, timerCtrl, ATTR_INTERVAL, 0.0);
	SetCtrlAttribute (panelTemp, timerCtrl, ATTR_ENABLED, 0);

	InstallCtrlCallback (panelTemp, startCtrl, StartCB_StartReadWfmLoopStop, info);
	InstallCtrlCallback (panelTemp, doneCtrl, DoneCB_StartReadWfmLoopStop, info);
	InstallCtrlCallback (panelTemp, stopCtrl, StopCB_StartReadWfmLoopStop, info);
	InstallCtrlCallback (panelTemp, timerCtrl, TimerCB_StartReadWfmLoopStop, info);

	PositionControlsOnPanel (panelTemp, 0, chartCtrl, startCtrl, stopCtrl, doneCtrl, 0);

	*panel = panelTemp;

Error:
	if (channelNames)
		FreeChannelNames (channelNames, numChannels);
	return DAQmxError;
}

static int32 GetChannelNames (TaskHandle task, uInt32 numChannels, char ***channelNames)
{
	int32 DAQmxError = DAQmxSuccess;
	uInt32 channelIndex;

	*channelNames = (char **)calloc (numChannels, sizeof (char *));
	if (!(*channelNames))
		goto Error;
	for (channelIndex = 0; channelIndex < numChannels; channelIndex++)
	{
		DAQmxNullChk((*channelNames)[channelIndex] = (char *)malloc (255 + 1));  // Assume channel names are no longer than 255 characters.
		DAQmxErrChk(DAQmxGetNthTaskChannel(task, channelIndex+1, (*channelNames)[channelIndex], 256));
	}

Error:
	return DAQmxError;
}

static void FreeChannelNames (char **channelNames, uInt32 numChannels)
{
	uInt32 channelIndex;

	if (channelNames) {
		for (channelIndex = 0; channelIndex < numChannels; channelIndex++)
			if (channelNames[channelIndex])
				free (channelNames[channelIndex]);
		free (channelNames);
	}
}

static void PositionControlsOnPanel (int panel, int numLegends, int mainCtrl, ...)
{
	va_list	parmInfo;
	int32 i, buttonList[MAX_BUTTONS]={0}, button, numButtons = 0, mainCtrlHeight, mainCtrlWidth;
	int32 panelHeight, panelWidth;

	// Build button array.
	va_start (parmInfo, mainCtrl);
	while (numButtons < MAX_BUTTONS) {	// Enforce maximum number of buttons.
		button = va_arg (parmInfo, int);
		if (button == 0)				// Button list is 0-terminated.
			break;
		buttonList[numButtons] = button;
		numButtons++;
	}
	va_end (parmInfo);

	SetCtrlAttribute (panel, mainCtrl, ATTR_LEFT, 50);
	SetCtrlAttribute (panel, mainCtrl, ATTR_TOP, 50);
	GetCtrlAttribute (panel, mainCtrl, ATTR_WIDTH, &mainCtrlWidth);
	GetCtrlAttribute (panel, mainCtrl, ATTR_HEIGHT, &mainCtrlHeight);
	if (numLegends > 0) {
		SetCtrlAttribute (panel, mainCtrl, ATTR_LEGEND_LEFT, VAL_LEFT_ANCHOR);
		SetCtrlAttribute (panel, mainCtrl, ATTR_LEGEND_TOP, 50 + mainCtrlHeight + 3);
	}
	panelHeight = mainCtrlHeight + 100 + (numLegends * 18);
	panelWidth = mainCtrlWidth + BUTTON_WIDTH + 150;

	for (i = 0; i < numButtons; i++)
	{	// Make all buttons the same size, and distribute them vertically.
		SetCtrlAttribute (panel, buttonList[i], ATTR_WIDTH, BUTTON_WIDTH);
		SetCtrlAttribute (panel, buttonList[i], ATTR_HEIGHT, BUTTON_HEIGHT);
		SetCtrlAttribute (panel, buttonList[i], ATTR_TOP, i * BUTTON_HEIGHT +
			(i + 1) * ((panelHeight - numButtons * BUTTON_HEIGHT) / (numButtons + 1)));
		SetCtrlAttribute (panel, buttonList[i], ATTR_LEFT, mainCtrlWidth + 100);
	}

	SetPanelAttribute (panel, ATTR_WIDTH, panelWidth);
	SetPanelAttribute (panel, ATTR_HEIGHT, panelHeight);
	SetPanelAttribute (panel, ATTR_LEFT, VAL_AUTO_CENTER);
	SetPanelAttribute (panel, ATTR_TOP, VAL_AUTO_CENTER);
}

