//==============================================================================
//
// Title:		windowing
// Purpose:		A short description of the application.
//
// Created on:	1/10/2019 at 8:30:23 AM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "windowing.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;

//==============================================================================
// Static functions
int SignalSpectrum(double inputArray[], ssize_t numberOfElements, double samplingFrequency, double xOutputArray[], double yOutputArray[]);
void DoCalculations();

//==============================================================================
// Global variables

//==============================================================================
// Global functions
double fs = 1000.0;
//const int N = 64;//pentru semnal scurt
const int N = 1024; //pentru semnal lung

double f1 = 392; // (G4 - Sol major)
double A1 = 10;
double f2 = 440; // (A4 - La major)
double A2 = 3;
double s1[N];
double s2[N];
double notes_signal[N];
double ys[N];
static double window[N];

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (panelHandle = LoadPanel (0, "windowing.uir", PANEL));
	
	//SetAxisTimeFormat(panelHandle, PANEL_RAWDATA_GRAPH, VAL_BOTTOM_XAXIS, VAL_RELATIVE_TIME_FORMAT, "%S");
	SetCtrlAttribute(panelHandle, PANEL_RAWDATA_GRAPH, ATTR_XAXIS_GAIN, N/fs);
	/* display the panel and run the user interface */
	errChk (DisplayPanel (panelHandle));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel (panelHandle);
	return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		QuitUserInterface (0);
	return 0;
}

int CVICALLBACK OnGenerateSignalCB (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	
	
	switch (event)
	{
		case EVENT_COMMIT:
			DoCalculations();
			break;
	}
	return 0;
}

void DoCalculations()
{

//%% Genereaza semnal cu doua note muzicale  
	for( int i = 0; i < N; i++)
	{
		s1[i] = A1 * sin(2 * PI * f1 / fs * i);
		s2[i] = A2 * sin(2 * PI * f2 / fs * i);	
	}
	//PlotY(panel, PANEL_RAWDATA_GRAPH, s1, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	//PlotY(panel, PANEL_RAWDATA_GRAPH, s2, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
	Add1D(s1, s2, N, notes_signal);

//%% Plot signal
	DeleteGraphPlot(panelHandle, PANEL_RAWDATA_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	PlotY(panelHandle, PANEL_RAWDATA_GRAPH, notes_signal, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
	
//%% Plot fft pentru semnalul considerat	
	double powerSpectrum[N/2];
	double frequencyArray[N/2];
	SignalSpectrum(notes_signal, N, fs, frequencyArray, powerSpectrum);
	DeleteGraphPlot(panelHandle, PANEL_SPECTRUM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	PlotXY (panelHandle, PANEL_SPECTRUM_GRAPH, frequencyArray, powerSpectrum, N/2, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,1, VAL_DK_GREEN);
	
	
//generare fereastra
	int windowing;
	WindowConst constants;
	
 	GetCtrlVal (panelHandle, PANEL_DATAWINDOW,  &windowing);	
    /* Generate and Display Window Data */ 
	/////////////////////////////////////////////////////////
    LinEv1D (window, N, 0, 1, window);//////////////////////
	///////////////////////////////////////////////////////
    ScaledWindow (window, N, windowing, &constants);
	DeleteGraphPlot(panelHandle, PANEL_WINDOWGRAPH, -1, VAL_IMMEDIATE_DRAW);
    PlotY (panelHandle, PANEL_WINDOWGRAPH, window, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
	
//%% Se aplica o fereastra pentru a limita efectul sinc
	double sw[N];
	Mul1D (notes_signal, window, N, sw);
	DeleteGraphPlot(panelHandle, PANEL_WINDOWED_SIGNAL, -1, VAL_IMMEDIATE_DRAW);
	PlotY (panelHandle, PANEL_WINDOWED_SIGNAL, sw, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);

//%% Plot fft dupa ferestruire	
	SignalSpectrum(sw, N, fs, frequencyArray, powerSpectrum);
	DeleteGraphPlot(panelHandle, PANEL_SPECTRUM_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
	PlotXY (panelHandle, PANEL_SPECTRUM_GRAPH_2, frequencyArray, powerSpectrum, N/2, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,1, VAL_DK_GREEN);

//%%aplicare filtre
//%% Plot fft dupa ferestruire	
//double f1 = 392; // (G4 - Sol major)
//double A1 = 10;
//double f2 = 440; // (A4 - La major)
//double A2 = 3;

	double filteredSignal[N];
	Bssl_BPF(sw, N, fs, 415.0, 500.0, 2, filteredSignal);
	Bssl_BSF(,,,,);
	//Bssl_LPF(sw, N, fs, 415.0, 2, filteredSignal);
	//Bssl_HPF(sw, N, fs, 415.0, 3, filteredSignal);
	int xAxisScalingMode = 0.0;
	double xmin = 0.0;
	double xmax = 0.0;
	int yAxisScalingMode = 0;
	double ymin = 0.0;
	double ymax = 0.0;
	GetAxisRange (panelHandle, PANEL_WINDOWED_SIGNAL, &xAxisScalingMode, &xmin, &xmax, &yAxisScalingMode, &ymin, &ymax);
	SetAxisRange (panelHandle, PANEL_FILTDATA_GRAPH, VAL_AUTOSCALE, xmin, xmax, VAL_MANUAL, ymin, ymax);
	PlotY (panelHandle, PANEL_FILTDATA_GRAPH, filteredSignal, N, VAL_DOUBLE, VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);

//%% Plot 	
	SignalSpectrum(filteredSignal, N, fs, frequencyArray, powerSpectrum);
	DeleteGraphPlot(panelHandle, PANEL_FILTSPECTRUM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	GetAxisRange (panelHandle, PANEL_SPECTRUM_GRAPH_2, &xAxisScalingMode, &xmin, &xmax, &yAxisScalingMode, &ymin, &ymax);
	SetAxisRange (panelHandle, PANEL_FILTSPECTRUM_GRAPH, VAL_AUTOSCALE, xmin, xmax, VAL_MANUAL, ymin, ymax);
	PlotXY (panelHandle, PANEL_FILTSPECTRUM_GRAPH, frequencyArray, powerSpectrum, N/2, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,1, VAL_DK_GREEN);
	
}



/********************************************************************************
	functie de a spectrului de putere a unui semnal 
********************************************************************************/
int SignalSpectrum(double inputArray[], ssize_t numberOfElements, double samplingFrequency, double xOutputArray[], double yOutputArray[])
{
	int err = 0;
	double WfR[numberOfElements];
	double WfI[numberOfElements];
	double p[numberOfElements];
	double ps[numberOfElements];
	double frequency_array[numberOfElements];

	for(int i = 0; i < numberOfElements; i++)
	{ 
		WfR[i] = inputArray[i]; //partea reala
		WfI[i] = 0; //partea imaginara o in cazul dat
	}
	FFT(WfR, WfI, numberOfElements);
	
	for (int i = 0; i < numberOfElements - 1; i++)
	{ 
		//spectrul dupa formula
		p[i] = WfR[i] * WfR[i] + WfI[i] * WfI[i];
		ps[i] = p[i] / pow((double)numberOfElements,2.0);
	}
	//Spectrum (wfm3, 1024);//Functie CVI- se poate incerca ca alternativa
	double delta_t = 1.0 / samplingFrequency; //pas in timp
	double delta_f = 1.0 / (numberOfElements * delta_t); //pas in frecventa
	
	//construiesc axa pentru frecventa
	frequency_array[0] = 0.0;
	frequency_array[numberOfElements - 1] = 1.0 / (2 * delta_t);
	for (int i = 0; i < numberOfElements / 2; i++)
	{
		frequency_array[i] = i * delta_f;
		frequency_array[numberOfElements - 1 - i] = -1 * delta_f;
	}

	Copy1D (frequency_array, numberOfElements/2, xOutputArray);
	Copy1D (ps, numberOfElements/2, yOutputArray);

	return err;
}
