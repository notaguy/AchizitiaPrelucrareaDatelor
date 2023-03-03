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

#define  PANEL                            1       /* callback function: panelCB */
#define  PANEL_FILTSPECTRUM_GRAPH         2       /* control type: graph, callback function: (none) */
#define  PANEL_SPECTRUM_GRAPH_2           3       /* control type: graph, callback function: (none) */
#define  PANEL_SPECTRUM_GRAPH             4       /* control type: graph, callback function: (none) */
#define  PANEL_WINDOWED_SIGNAL            5       /* control type: graph, callback function: (none) */
#define  PANEL_FILTDATA_GRAPH             6       /* control type: graph, callback function: (none) */
#define  PANEL_RAWDATA_GRAPH              7       /* control type: graph, callback function: (none) */
#define  PANEL_DATAWINDOW                 8       /* control type: ring, callback function: OnGenerateSignalCB */
#define  PANEL_WINDOWGRAPH                9       /* control type: graph, callback function: (none) */
#define  PANEL_TEXTMSG                    10      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnGenerateSignalCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif