#include <cvirte.h>		
#include <userint.h>
#include "1.h"
#include <formatio.h>//filetoarray

#include <analysis.h>//pt functiile de calcul (min, max, dispersie, medie, mediana)
//#include <utility.h>
#include <ansi_c.h> //ca sa aloc, mai folosesc din c

/////////////cerinte///////////// 
//valori min/max+indexul lor, dispersia, medie, mediana
//numarul de treceri prin zero, histograma
//semnal filtrat in timp cu mediere si element de ordin I, sa arat pe fiecare secunda
//derivata
//afisarea anvelopei pe acelasi graph

static int mainPanel;
static int histoPanel;
static int derivPanel;

static int aPanel;

//frecventa de esantionare si nr de esantioane
double sampleRate=0.0;
int npoints=0;

//vectorul pt datele din fisier
int waveInfo[2];
//vectorul pt sirul de numere din fisier
double *waveData=0;

//variabile care ma ajuta sa fac verificarea daca am apasat butonul de Load si de Aplica
int load_activ=0;
int am_apasat=0;

double maxim=0.0;
double minim=0.0;
int maxIndex=0;
int minIndex=0;
double dispersie=0.0;
double medie=0.0;
double mediana=0.0;
int count=0;
	
int start=0;
int stop=0;
int tipfiltru;
double *filtrat=0;//aici imi pun datele filtrate
//pentru filtru mediere
int n; //dimensiunea ferestrei sau nr de esantioane pt mediere 16/32
//pentru filtru ordinI
double alpha=0.0;

//derivata
double *derivata=0;
//vectorul pentru anvelopa
//am rulat in google colab scriptul python si am generat puncte dupa care sa trasez graficul anvelopei
double *anvelopa=0;

//////pt etapa 2////////
int N;//nr de puncte 2048, 4096 etc. 
double freqPeak=0.0;
double powerPeak=0.0;

int tipFereastra;
int filtruTip;
double df; //pasul in domeniul frecventei=1/(dt*nrelem)
int ordinFiltru=4;
double cutoffFreq;


////////functii pt filtre: mediere si ordin 1//////////
double *filtrumediere(int n)
{
	double suma=0.0;
	double *filtrat=(double*)malloc(npoints*sizeof(double));
	for(int i=0;i<n-1;++i)
	{
		suma+=waveData[i];
	}
	for(int i=0;i<n-1;++i)
	{
		filtrat[i]=suma/n;
	}
	for(int i=n;i<npoints;++i)
	{
		suma-=waveData[i-n];
		suma+=waveData[i];
		filtrat[i]=suma/n;
	}
	return filtrat;
}

double *filtruO1(double alpha)
{
	double *filtrat=(double*)malloc(npoints*sizeof(double));
	filtrat[1]=waveData[1];
	for(int i=1;i<npoints;++i)
	{
		filtrat[i]=(1-alpha)*filtrat[i-1]+alpha*waveData[i];
	}
	return filtrat;
}

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainPanel = LoadPanel (0, "1.uir", PANEL)) < 0)
		return -1;
	
	histoPanel=LoadPanel(mainPanel, "1.uir", IDC_HISTO);
	derivPanel=LoadPanel(mainPanel, "1.uir", IDC_DERIV);
	aPanel=LoadPanel(0, "1.uir", ACQUISITIO);
	
	DisplayPanel (mainPanel);
	RunUserInterface ();
	
	if(mainPanel>0)
	{
		DiscardPanel (mainPanel);
	}
	return 0;
}

int CVICALLBACK OnMainPanelCB (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			break;
	}
	return 0;
}

// Constante									   
#define SAMPLE_RATE		0
#define NPOINTS			1
#define ANVELOPA        0

//pt butonul LOAD si panoul Raw Data
int CVICALLBACK On_Load (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			//incarc din fisier si afisez datele pe graph
			FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			waveData=(double *) calloc(npoints, sizeof(double));
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			DeleteGraphPlot(panel,PANEL_IDC_RAW_GRAPH ,-1, VAL_IMMEDIATE_DRAW); 
			PlotY(panel,PANEL_IDC_RAW_GRAPH , waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			///afisez si pe graficul din al doilea panou///
			DeleteGraphPlot(aPanel, ACQUISITIO_IDC_RAW_GRAPH,-1, VAL_IMMEDIATE_DRAW); 
			PlotY(aPanel,ACQUISITIO_IDC_RAW_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			
			load_activ=1;
			
			//min&max 
			MaxMin1D(waveData,npoints,&maxim,&maxIndex,&minim,&minIndex);
			SetCtrlVal(panel,PANEL_MAXIM, maxim);
			SetCtrlVal(panel,PANEL_MINIM, minim);
			
			//indexii
			SetCtrlVal(panel,PANEL_INDEXMAX, (double)maxIndex);
			SetCtrlVal(panel,PANEL_INDEXMIN, (double)minIndex);
			
			//media
			Mean(waveData,npoints,&medie);
			SetCtrlVal(panel,PANEL_MEDIE,medie);
			//mediana
			Median(waveData,npoints,&mediana);
			SetCtrlVal(panel,PANEL_MEDIANA,mediana);
			//disperia
			StdDev(waveData,npoints,&medie,&dispersie);
			SetCtrlVal(panel,PANEL_DISPERSIE, dispersie);
			
			//treceri prin 0
			for(int i=1;i<npoints-1;i++)
			{
				if((waveData[i]>0 && waveData[i+1]<0) || (waveData[i-1]<0 && waveData[i]>0))
				{
					count++;
				}
			}
			SetCtrlVal(panel,PANEL_TRECERIPRIN0,(double)count);
			
			//salvare imagine originala
			char *file = (char*)malloc(40*sizeof(char));
			int img; 
			sprintf(file, "%s", "rawData.png");//numele cu care salvez imaginea
			GetCtrlDisplayBitmap(mainPanel, PANEL_IDC_RAW_GRAPH, 1, &img);
			SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
			break;
	}
	return 0;
}

int CVICALLBACK On_Load_Histo (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(histoPanel);
			break;
	}
	return 0;
}

int CVICALLBACK On_Load_Deriv (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DisplayPanel(derivPanel);
			break;
	}
	return 0;
}

int CVICALLBACK On_Exit_Deriv (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(derivPanel);
			break;
	}
	return 0;
}


int CVICALLBACK On_Exit_Histo (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel(histoPanel);
			break;
	}
	return 0;
}

int CVICALLBACK On_Histo_Panel (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	int intervale=100;
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			if(load_activ)
			{
				ssize_t vectfrecvente[intervale];
				double vectorx[intervale];
				Histogram(waveData, npoints, minim, maxim, vectfrecvente, vectorx, intervale);
				
				DeleteGraphPlot(mainPanel, IDC_HISTO_HISTOGRAMA,-1, VAL_IMMEDIATE_DRAW); 
				PlotXY(panel, IDC_HISTO_HISTOGRAMA, vectorx, vectfrecvente,intervale,VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			}
			
			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
	}
	return 0;
}

int CVICALLBACK On_Deriv_Panel (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			if (npoints == 0)
				return -1;
			
			derivata = (double *) calloc(npoints, sizeof(double));
			DifferenceEx (waveData, npoints, 1.0, waveData + 1, 1, waveData + npoints - 1, 1, DIFF_SECOND_ORDER_CENTRAL, derivata); 
			
			PlotY(panel, IDC_DERIV_DERIVATA, derivata, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN); 
			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			break;
			
	}
	return 0;
}

//aici cand aleg filtrul apelez functiile 
int CVICALLBACK On_filtru (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	int dim;
	double alpha;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainPanel, PANEL_FILTRU, &tipfiltru);
			
			if(tipfiltru==0)//mediere
			{
				SetCtrlAttribute(mainPanel,PANEL_ALPHA, ATTR_DIMMED, 1);//dezactivez alpha
				SetCtrlAttribute(mainPanel,PANEL_DIMENSIUNE, ATTR_DIMMED, 0);
				GetCtrlVal(mainPanel,PANEL_DIMENSIUNE, &dim);
				
				filtrat=filtrumediere(dim);
				DeleteGraphPlot(mainPanel,PANEL_IDC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(mainPanel, PANEL_IDC_FILTERED_GRAPH, filtrat, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
				
			}
			else //ordinI
			{
				SetCtrlAttribute(mainPanel,PANEL_DIMENSIUNE, ATTR_DIMMED, 1);//dezactivez dimensiunea 
				SetCtrlAttribute(mainPanel,PANEL_ALPHA, ATTR_DIMMED, 0);
				GetCtrlVal(mainPanel,PANEL_ALPHA, &alpha);
				
				filtrat=filtruO1(alpha);
				DeleteGraphPlot(mainPanel,PANEL_IDC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(mainPanel, PANEL_IDC_FILTERED_GRAPH, filtrat, npoints, VAL_DOUBLE,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			}
			
			break;
	}
	return 0;
}

int CVICALLBACK On_aplica (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			On_filtru(mainPanel, PANEL_FILTRU, EVENT_COMMIT, 0,0,0);
			DeleteGraphPlot(panel, PANEL_IDC_RAW_GRAPH,-1, VAL_IMMEDIATE_DRAW); 
			PlotY(panel,PANEL_IDC_RAW_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			
			am_apasat=1;
			break;
	}
	return 0;
}

//am facut o functie pentu ambele butoane de next si prev
int CVICALLBACK On_Next_Prev (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	double *signal;
	double *filt;
	n=npoints/6;
	switch (event)
	{
		case EVENT_COMMIT:
			if(am_apasat)
			{
				GetCtrlVal(mainPanel,PANEL_START,&start);
				GetCtrlVal(mainPanel,PANEL_STOP,&stop);
				
				signal=(double*)calloc(n,sizeof(double));
				filt=(double*)calloc(n,sizeof(double));
				
				switch(control)
				{
					case PANEL_NEXT:
						switch(event)
						{
							case EVENT_COMMIT:
								if(stop<6)
								{
									++start;
									++stop;
									SetCtrlVal(mainPanel,PANEL_STOP,stop);
									SetCtrlVal(mainPanel,PANEL_START,start);
									for(int i=0;i<n;++i)
									{
										filt[i]=filtrat[start*n+i];
									}
									
									for(int i=0;i<n;++i)
									{
										signal[i]=waveData[start*n+i];
									}
									DeleteGraphPlot (mainPanel,PANEL_IDC_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainPanel, PANEL_IDC_RAW_GRAPH, signal, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
									
									DeleteGraphPlot (mainPanel,PANEL_IDC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainPanel, PANEL_IDC_FILTERED_GRAPH, filt, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
								
									/////////////////////////////////////////////////////////
									//min&max 
									MaxMin1D(filt,n,&maxim,&maxIndex,&minim,&minIndex);
									SetCtrlVal(mainPanel,PANEL_MAXIM, maxim);
									SetCtrlVal(mainPanel,PANEL_MINIM, minim);
									
									//indexii
									SetCtrlVal(mainPanel,PANEL_INDEXMAX, (double)maxIndex);
									SetCtrlVal(mainPanel,PANEL_INDEXMIN, (double)minIndex);
									
									//media
									Mean(filt,n,&medie);
									SetCtrlVal(mainPanel,PANEL_MEDIE,medie);
									//mediana
									Median(filt,n,&mediana);
									SetCtrlVal(mainPanel,PANEL_MEDIANA,mediana);
									//disperia
									StdDev(filt,n,&medie,&dispersie);
									SetCtrlVal(mainPanel,PANEL_DISPERSIE, dispersie);
									
									//treceri prin 0
									for(int i=1;i<n-1;i++)
									{
										if((filt[i]>0 && filt[i+1]<0) || (filt[i-1]<0 && filt[i]>0))
										{
											count++;
										}
									}
									SetCtrlVal(mainPanel,PANEL_TRECERIPRIN0,(double)count);
									
								}
								break;
						}
								
						break;
					case PANEL_PREV:
						switch(event)
						{
							case EVENT_COMMIT:
								if(stop>1)
								{
									--stop;
									--start;
									SetCtrlVal(mainPanel,PANEL_STOP,stop);
									SetCtrlVal(mainPanel,PANEL_START,start);
									for(int i=0;i<n;++i)
									{
										filt[i]=filtrat[start*npoints/6+i];
									}
									
									for(int i=0;i<n;++i)
									{
										signal[i]=waveData[start*n+i];
									}
									DeleteGraphPlot (mainPanel,PANEL_IDC_RAW_GRAPH, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainPanel, PANEL_IDC_RAW_GRAPH, signal, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
									
									DeleteGraphPlot (mainPanel, PANEL_IDC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainPanel, PANEL_IDC_FILTERED_GRAPH, filt, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
								
									/////////////////////////////////////////////
									//min&max 
									MaxMin1D(filt,n,&maxim,&maxIndex,&minim,&minIndex);
									SetCtrlVal(mainPanel,PANEL_MAXIM, maxim);
									SetCtrlVal(mainPanel,PANEL_MINIM, minim);
									
									//indexii
									SetCtrlVal(mainPanel,PANEL_INDEXMAX, (double)maxIndex);
									SetCtrlVal(mainPanel,PANEL_INDEXMIN, (double)minIndex);
									
									//media
									Mean(filt,n,&medie);
									SetCtrlVal(mainPanel,PANEL_MEDIE,medie);
									//mediana
									Median(filt,n,&mediana);
									SetCtrlVal(mainPanel,PANEL_MEDIANA,mediana);
									//disperia
									StdDev(filt,n,&medie,&dispersie);
									SetCtrlVal(mainPanel,PANEL_DISPERSIE, dispersie);
									
									//treceri prin 0
									for(int i=1;i<n-1;i++)
									{
										if((filt[i]>0 && filt[i+1]<0) || (filt[i-1]<0 && filt[i]>0))
										{
											count++;
										}
									}
									SetCtrlVal(mainPanel,PANEL_TRECERIPRIN0,(double)count);
								}
							break;	
						}
						break;
				}
			}
			if(tipfiltru==0)
			{
				char *file = (char*)malloc(40*sizeof(char));
				int img; 
				sprintf(file, "%s", "filtruMedian.png");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(mainPanel, PANEL_IDC_FILTERED_GRAPH, 1, &img);
				SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
			}
			else
			{
				char *file = (char*)malloc(40*sizeof(char));
				int img; 
				sprintf(file, "%s", "filtruO1.png");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(mainPanel, PANEL_IDC_FILTERED_GRAPH, 1, &img);
				SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
			}
			break;
	}
	return 0;
}

int CVICALLBACK On_Anvelopa (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			anvelopa=(double *) calloc(npoints, sizeof(double));
			FileToArray("anvelopa.txt", anvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			DeleteGraphPlot(panel,PANEL_IDC_RAW_GRAPH ,-1, VAL_IMMEDIATE_DRAW); 
			PlotY(mainPanel,PANEL_IDC_RAW_GRAPH , anvelopa, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);
			break;
	}
	return 0;
}
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //////////////////////////////////////////////////////////////////ETAPA 2//////////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CVICALLBACK OnAcquisitionPanel (int panel, int event, void *callbackData,
									int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK On_switchPanel (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel==mainPanel)
			{
				SetCtrlVal(aPanel,ACQUISITIO_SWITCHPANEL, 1);
				DisplayPanel(aPanel);
				HidePanel(panel);
				MessagePopup("Atentie!", "Nu uita sa incarci wave-ul");
			}
			else if(panel==aPanel)
			{
				SetCtrlVal(mainPanel,PANEL_SWITCHPANEL, 0);
				DisplayPanel(mainPanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

int CVICALLBACK On_Spectru (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	double *convertedSpectrum=(double *) calloc (sampleRate, sizeof(double));
	double *autospectrum=(double *) calloc (sampleRate, sizeof(double));
	double dt=1/sampleRate;
	
	WindowConst winConst;
	char unit[32]="V";
	
	double *ferestruit=(double *) calloc (sampleRate, sizeof(double));
	double *window;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(aPanel,ACQUISITIO_NPUNCTE, &N);
			GetCtrlVal(aPanel,ACQUISITIO_STOP, &stop);
			GetCtrlVal(aPanel,ACQUISITIO_START, &start);
			
			//mai intai aplic ferestrele
			window=waveData+(int)(start*N);
			LinEv1D(window, N, 1, 0, ferestruit); //functia din exemplul dat, 
			switch(tipFereastra)
			{
				case 0: //Hamming
				{
					HamWin(ferestruit,sampleRate);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotY(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, ferestruit, (stop-start) * N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN );
			        
					//spectrul pentru semnalul ferestruit
					ScaledWindowEx(ferestruit, N, RECTANGLE_, 0, &winConst);
					//calculez partea pozitiva a spectrului scalat de putere
					AutoPowerSpectrum(window, N, dt, autospectrum, &df);
					//calculam valoarea maxima din spectrul de putere si frecventa estimata pentru spectrul de putere maxim
					PowerFrequencyEstimate(autospectrum, N/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_POWERPEAK, powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_FREQPEAK, freqPeak);
			
					SpectrumUnitConversion(autospectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotWaveform(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, convertedSpectrum, N, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);
					
					break;
				}
				case 1: //FlatTop
				{
					FlatTopWin(ferestruit, sampleRate);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotY(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, ferestruit, (stop-start) * N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN );
					
					//spectrul pentru semnalul ferestruit
					ScaledWindowEx(ferestruit, N, RECTANGLE_, 0, &winConst);
					//calculez partea pozitiva a spectrului scalat de putere
					AutoPowerSpectrum(window, N, dt, autospectrum, &df);
					//calculam valoarea maxima din spectrul de putere si frecventa estimata pentru spectrul de putere maxim
					PowerFrequencyEstimate(autospectrum, N/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_POWERPEAK, powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_FREQPEAK, freqPeak);
			
					SpectrumUnitConversion(autospectrum, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotWaveform(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, convertedSpectrum, N, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);
					
					break;
				}
			}
			if(tipFereastra==0)
			{
				char *file = (char*)malloc(40*sizeof(char));
				int fereastra; 
				int spectru;
				sprintf(file, "%s", "Hamming.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, 1, &fereastra);
				SaveBitmapToJPEGFile(fereastra, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(fereastra);
				sprintf(file, "%s", "Hamming_Spectru.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, 1, &spectru);
				SaveBitmapToJPEGFile(spectru, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(spectru);
			}
			else
			{
				char *file = (char*)malloc(40*sizeof(char));
				int fereastra; 
				int spectru;
				sprintf(file, "%s", "FlatTop.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_WINDOWED_GRAPH, 1, &fereastra);
				SaveBitmapToJPEGFile(fereastra, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(fereastra);
				sprintf(file, "%s", "FlatTop_Spectru.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_WSPECTRU_GRAPH, 1, &spectru);
				SaveBitmapToJPEGFile(spectru, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(spectru);
			}
			
			break;	
			
	}
	return 0;
}

int CVICALLBACK On_Next_Prev2 (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
				GetCtrlVal(aPanel,ACQUISITIO_START,&start);
				GetCtrlVal(aPanel,ACQUISITIO_STOP,&stop);
				switch(control)
				{
					case ACQUISITIO_NEXT:
					{
						int aux=(int)npoints/N;
						switch(event)
						{
							case EVENT_COMMIT:
								if(start>=0 && stop<aux)
								{
									++start;
									++stop;
									SetCtrlVal(aPanel,ACQUISITIO_STOP,stop);
									SetCtrlVal(aPanel,ACQUISITIO_START,start);	
								}
								break;
						}		
						break;
					}
					case ACQUISITIO_PREV:
					{
						int aux=(int)npoints/N;
						switch(event)
						{
							case EVENT_COMMIT:
								if(start>0 && stop<=aux)
								{
									--stop;
									--start;
									SetCtrlVal(aPanel,ACQUISITIO_STOP,stop);
									SetCtrlVal(aPanel,ACQUISITIO_START,start);
								}
							break;	
						}
						break;
					}
				}
	}
	return 0;
}

int CVICALLBACK On_tipFereastra (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(aPanel, ACQUISITIO_FEREASTRA_SWITCH, &tipFereastra);
			break;
	}
	return 0;
}

int CVICALLBACK On_tipFiltru (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(aPanel, ACQUISITIO_FILTRU_SWITCH, &filtruTip);
			break;
	}
	return 0;
}

int CVICALLBACK On_ordin (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(aPanel, ACQUISITIO_ORDIN, &ordinFiltru);
			break;
	}
	return 0;
}

int CVICALLBACK On_SpectrupeFiltrare (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	double *convertedSpectrumFiltru=(double *) calloc (sampleRate, sizeof(double));
	double *autospectrumFiltru=(double *) calloc (sampleRate, sizeof(double));
	double dt=1/sampleRate;
	
	WindowConst winConst;
	char unit[32]="V";
	
	double *ferestruit=(double *) calloc (sampleRate, sizeof(double));
	double *window;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//imi iau numarul de puncte si secunda pt care trebuie sa reprezint
			GetCtrlVal(aPanel,ACQUISITIO_NPUNCTE, &N);
			GetCtrlVal(aPanel,ACQUISITIO_STOP, &stop);
			GetCtrlVal(aPanel,ACQUISITIO_START, &start);
			
			//mai intai aplic ferestrele
			window=waveData+(int)(start*N);
			LinEv1D(window, N, 1, 0, ferestruit); //functia din exemplul dat, 
			switch(tipFereastra)
			{
				case 0: //Hamming
				{
					HamWin(ferestruit,sampleRate);
					break;
				}
				case 1: //FlatTop
				{
					FlatTopWin(ferestruit, sampleRate);
					break;
				}
			}
			
			double *vectorBessel=(double *) calloc (sampleRate, sizeof(double));
			double *vectorButterworth=(double *) calloc (sampleRate, sizeof(double));
			switch(filtruTip)
			{
				case 0: //Bessel
				{
					Bssl_LPF(ferestruit, sampleRate, 0.5*sampleRate, sampleRate/6, ordinFiltru, vectorBessel);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotY(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, vectorBessel, (stop-start)*N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
					
					//spectrul pentru semnalul ferestruit
					ScaledWindowEx(waveData, N, RECTANGLE_, 0, &winConst);
					//calculez partea pozitiva a spectrului scalat de putere
					AutoPowerSpectrum(window, N, dt, autospectrumFiltru, &df);
					//calculam valoarea maxima din spectrul de putere si frecventa estimata pentru spectrul de putere maxim
					PowerFrequencyEstimate(autospectrumFiltru, N/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_POWERPEAK, powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_FREQPEAK, freqPeak);
			
					SpectrumUnitConversion(autospectrumFiltru, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumFiltru, unit);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotWaveform(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, convertedSpectrumFiltru, N, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);
					
					break;
				}
				
				case 1: //Butterworth
				{
					Bw_LPF(ferestruit, sampleRate, 0.5*sampleRate, sampleRate/6, ordinFiltru, vectorButterworth);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotY(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, vectorButterworth, (stop-start)*N, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
					
					//spectrul pentru semnalul ferestruit
					ScaledWindowEx(waveData, N, RECTANGLE_, 0, &winConst);
					//calculez partea pozitiva a spectrului scalat de putere
					AutoPowerSpectrum(window, N, dt, autospectrumFiltru, &df);
					//calculam valoarea maxima din spectrul de putere si frecventa estimata pentru spectrul de putere maxim
					PowerFrequencyEstimate(autospectrumFiltru, N/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_POWERPEAK, powerPeak);
					SetCtrlVal(aPanel, ACQUISITIO_FREQPEAK, freqPeak);
			
					SpectrumUnitConversion(autospectrumFiltru, N/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumFiltru, unit);
					DeleteGraphPlot(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotWaveform(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, convertedSpectrumFiltru, N, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_MAGENTA);
					
					break;
				}
			}
			if(filtruTip==0)
			{
				char *file = (char*)malloc(40*sizeof(char));
				int fereastra; 
				int spectru;
				sprintf(file, "%s", "Bessel.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, 1, &fereastra);
				SaveBitmapToJPEGFile(fereastra, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(fereastra);
				sprintf(file, "%s", "Bessel_Spectru.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, 1, &spectru);
				SaveBitmapToJPEGFile(spectru, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(spectru);
			}
			else
			{
				char *file = (char*)malloc(40*sizeof(char));
				int fereastra; 
				int spectru;
				sprintf(file, "%s", "Butterworth.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_FILTERED_GRAPH, 1, &fereastra);
				SaveBitmapToJPEGFile(fereastra, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(fereastra);
				sprintf(file, "%s", "Butterworth_Spectru.jpeg");//numele cu care salvez imaginea
				GetCtrlDisplayBitmap(aPanel, ACQUISITIO_SEC_FSPECTRU_GRAPH, 1, &spectru);
				SaveBitmapToJPEGFile(spectru, file, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(spectru);
			}
		break;	
	}
	return 0;
}






