/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ACQUISITIO                       1       /* callback function: OnAcquisitionPanel */
#define  ACQUISITIO_DECORATION_6          2       /* control type: deco, callback function: (none) */
#define  ACQUISITIO_SWITCHPANEL           3       /* control type: binary, callback function: On_switchPanel */
#define  ACQUISITIO_NEXT                  4       /* control type: command, callback function: On_Next_Prev2 */
#define  ACQUISITIO_PREV                  5       /* control type: command, callback function: On_Next_Prev2 */
#define  ACQUISITIO_STOP                  6       /* control type: numeric, callback function: (none) */
#define  ACQUISITIO_START                 7       /* control type: numeric, callback function: (none) */
#define  ACQUISITIO_SEC_FSPECTRU_GRAPH    8       /* control type: graph, callback function: (none) */
#define  ACQUISITIO_SEC_WSPECTRU_GRAPH    9       /* control type: graph, callback function: (none) */
#define  ACQUISITIO_SEC_FILTERED_GRAPH    10      /* control type: graph, callback function: (none) */
#define  ACQUISITIO_SEC_WINDOWED_GRAPH    11      /* control type: graph, callback function: (none) */
#define  ACQUISITIO_IDC_RAW_GRAPH         12      /* control type: graph, callback function: (none) */
#define  ACQUISITIO_TEXTMSG               13      /* control type: textMsg, callback function: (none) */
#define  ACQUISITIO_NPUNCTE               14      /* control type: ring, callback function: (none) */
#define  ACQUISITIO_SPECTRU_BUTTON        15      /* control type: command, callback function: On_Spectru */
#define  ACQUISITIO_FILTRARE_BUTTON       16      /* control type: command, callback function: On_SpectrupeFiltrare */
#define  ACQUISITIO_FILTRU_SWITCH         17      /* control type: binary, callback function: On_tipFiltru */
#define  ACQUISITIO_FEREASTRA_SWITCH      18      /* control type: binary, callback function: On_tipFereastra */
#define  ACQUISITIO_POWERPEAK             19      /* control type: numeric, callback function: (none) */
#define  ACQUISITIO_FREQPEAK              20      /* control type: numeric, callback function: (none) */
#define  ACQUISITIO_DECORATION_3          21      /* control type: deco, callback function: (none) */
#define  ACQUISITIO_ORDIN                 22      /* control type: numeric, callback function: On_ordin */

#define  IDC_DERIV                        2       /* callback function: On_Deriv_Panel */
#define  IDC_DERIV_DERIVATA               2       /* control type: graph, callback function: (none) */
#define  IDC_DERIV_COMMANDBUTTON          3       /* control type: command, callback function: On_Exit_Deriv */

#define  IDC_HISTO                        3       /* callback function: On_Histo_Panel */
#define  IDC_HISTO_HISTOGRAMA             2       /* control type: graph, callback function: (none) */
#define  IDC_HISTO_COMMANDBUTTON          3       /* control type: command, callback function: On_Exit_Histo */

#define  PANEL                            4       /* callback function: OnMainPanelCB */
#define  PANEL_IDC_FILTERED_GRAPH         2       /* control type: graph, callback function: (none) */
#define  PANEL_IDC_RAW_GRAPH              3       /* control type: graph, callback function: On_Load */
#define  PANEL_NEXT                       4       /* control type: command, callback function: On_Next_Prev */
#define  PANEL_PREV                       5       /* control type: command, callback function: On_Next_Prev */
#define  PANEL_STOP                       6       /* control type: numeric, callback function: (none) */
#define  PANEL_START                      7       /* control type: numeric, callback function: (none) */
#define  PANEL_DERIVATABUTTON             8       /* control type: command, callback function: On_Load_Deriv */
#define  PANEL_HISTOGRAMBUTTON            9       /* control type: command, callback function: On_Load_Histo */
#define  PANEL_LOAD_2                     10      /* control type: command, callback function: On_Anvelopa */
#define  PANEL_LOAD                       11      /* control type: command, callback function: On_Load */
#define  PANEL_APLICA                     12      /* control type: command, callback function: On_aplica */
#define  PANEL_TEXTMSG                    13      /* control type: textMsg, callback function: (none) */
#define  PANEL_MAXIM                      14      /* control type: numeric, callback function: (none) */
#define  PANEL_MINIM                      15      /* control type: numeric, callback function: (none) */
#define  PANEL_INDEXMIN                   16      /* control type: numeric, callback function: (none) */
#define  PANEL_INDEXMAX                   17      /* control type: numeric, callback function: (none) */
#define  PANEL_TRECERIPRIN0               18      /* control type: numeric, callback function: (none) */
#define  PANEL_DISPERSIE                  19      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIANA                    20      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIE                      21      /* control type: numeric, callback function: (none) */
#define  PANEL_ALPHA                      22      /* control type: scale, callback function: (none) */
#define  PANEL_DIMENSIUNE                 23      /* control type: ring, callback function: (none) */
#define  PANEL_FILTRU                     24      /* control type: binary, callback function: On_filtru */
#define  PANEL_DECORATION_5               25      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_4               26      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION                 27      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_6               28      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               29      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_3               30      /* control type: deco, callback function: (none) */
#define  PANEL_SWITCHPANEL                31      /* control type: binary, callback function: On_switchPanel */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK On_Anvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_aplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Deriv_Panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Exit_Deriv(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Exit_Histo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_filtru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Histo_Panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Load(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Load_Deriv(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Load_Histo(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Next_Prev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Next_Prev2(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_ordin(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Spectru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_SpectrupeFiltrare(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_switchPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_tipFereastra(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_tipFiltru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAcquisitionPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif