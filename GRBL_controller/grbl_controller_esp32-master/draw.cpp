#include "config.h"
#include "draw.h"
#include "language.h"
#include "TFT_eSPI_ms/TFT_eSPI.h"
#include "FS.h"
#include "actions.h"
#include "menu_file.h"
#include "SdFat.h"
#include "browser.h"
#include "com.h"
#include "log.h"

#define LABELS9_FONT &FreeSans9pt7b    // Key label font 2
#define LABELS12_FONT &FreeSans12pt7b
#define LABELSB9_FONT &FreeSansBold9pt7b
#define LABELCB12_FONT &FreeSansBold12pt7b
#define LABELM9_FONT &FreeMono9pt7b    // Key label font 2
#define LABELM12_FONT &FreeMono12pt7b
#define LABELMB9_FONT &FreeMonoBold9pt7b
#define LABELMB12_FONT &FreeMonoBold12pt7b

//#define NORMAL_BORDER TFT_WHITE 
//#define SELECTED_BORDER TFT_RED 

// create for touchscreeen
TFT_eSPI tft = TFT_eSPI();

M_Button mButton[_MAX_BTN] ;
M_Page mPages[_P_MAX_PAGES];

extern uint8_t prevPage , currentPage ;
extern boolean updateFullPage ;
extern boolean updatePartPage ;
extern uint8_t justPressedBtn , justReleasedBtn, longPressedBtn , lastBtn ,currentBtn;  
extern boolean waitReleased ;
extern uint32_t beginChangeBtnMillis ;

//extern uint8_t sdStatus ;
extern int8_t dirLevel ;
extern uint16_t sdFileDirCnt ;
extern uint32_t sdFileSize ;
extern uint32_t sdNumberOfCharSent ;

extern char fileNames[4][23] ; // 22 car per line + "\0"
extern uint16_t firstFileToDisplay ;   // 0 = first file in the directory
extern SdFat sd;  // is created in ino file
extern SdBaseFile aDir[DIR_LEVEL_MAX] ;

extern char cmdName[11][17] ;          // contains the names of the commands

extern uint8_t statusPrinting ;
extern float wposXYZA[4] ;
extern float mposXYZA[3] ;
extern char machineStatus[9];
extern float feedSpindle[2] ;  
extern float overwritePercent[3] ; // first is for feedrate, second for rapid (G0...), third is for RPM



extern char lastMsg[80] ;        // last message to display
extern uint16_t lastMsgColor ;            // color of last Msg

extern uint8_t logBuffer[MAX_BUFFER_SIZE] ;
extern uint8_t * pNext ; // position of the next char to be written
extern uint8_t * pFirst ; // position of the first char
extern uint8_t * pGet ; // position of the last char to be read for display

  

//char sdStatusText[][20]  = { "No Sd card" , "Sd card to check" , "Sd card OK" , "Sd card error"} ;  // pour affichage en clair sur le lcd;
char printingStatusText[][20] = { " " , "SD-->Grbl" , "Error SD-->Grbl" , "Pause SD-->Grbl" , "Usb-->Grbl" ,  "CMD" , "Telnet-->Grbl" , "Set cmd"} ; 

//extern int8_t prevMoveX ;
//extern int8_t prevMoveY ;
//extern int8_t prevMoveZ ;
//extern int8_t prevMoveA ;
extern float moveMultiplier ;
// used by nunchuck
extern uint8_t jog_status  ;
extern boolean jogCancelFlag ;
extern boolean jogCmdFlag  ; 

extern boolean statusTelnetIsConnected ;

extern uint8_t logBuffer[MAX_BUFFER_SIZE] ;                              // log data
//extern uint8_t * pNext ; // position of the next char to be written
//extern uint8_t * pFirst ; // position of the first char
extern uint8_t * pGet ; // position of the last char to be read for display
uint8_t * pLastLogLine ; // pointer to the last Log line
boolean endOfLog = true ;
float wposMoveInitXYZA[4] ;

// previous values used to optimise redraw of INFO screen
uint8_t statusPrintingPrev;
uint8_t machineStatus0Prev;   // we keep only the first char for fast testing, so we can't use ir when equal to A or H
extern boolean lastMsgChanged ;
boolean statusTelnetIsConnectedPrev ;
boolean newInfoPage ;

char lineOfText[50] ; // Store 1 line of text for Log screen

extern char grblLastMessage[STR_GRBL_BUF_MAX_SIZE] ;
extern boolean grblLastMessageChanged;

//**************** normal screen definition.
#define B0 3
#define E0 3+74
#define B1 3+80
#define E1 3+80+74
#define B2 3+160
#define E2 3+160+74
#define B3 3+240
#define E3 3+240+74
#define FXB 3
#define FXE 3+80+74
#define FYB0 2
#define FYE0 2 + 56
#define FYB1 2 + 56 + 4
#define FYE1 2 + 56 + 4  + 56 
#define FYB2 2 + 56 + 4  + 56 + 4 
#define FYE2 2 + 56 + 4  + 56 + 4 + 56 
#define FYB3 2 + 56 + 4  + 56 + 4 + 56 + 4
#define FYE3 2 + 56 + 4  + 56 + 4 + 56 + 4 + 56 
 

uint16_t btnDefNormal[12][4] = {{ B0 , E0 , B0 , E0 } ,  // each line contains the Xmin, Xmax, Ymin , Ymax of one button.
                                { B1 , E1 , B0 , E0 } ,
                                { B2 , E2 , B0 , E0 } ,
                                { B3 , E3 , B0 , E0 } ,
                                { B0 , E0 , B1 , E1 } ,
                                { B1 , E1 , B1 , E1 } ,
                                { B2 , E2 , B1 , E1 } ,
                                { B3 , E3 , B1 , E1 } ,
                                { B0 , E0 , B2 , E2 } ,
                                { B1 , E1 , B2 , E2 } ,
                                { B2 , E2 , B2 , E2 } ,
                                { B3 , E3 , B2 , E2 } } ;

uint16_t btnDefFiles[12][4] = {{ FXB , FXE , FYB0 , FYE0 } ,  // each line contains the Xmin, Xmax, Ymin , Ymax of one button.
                                { FXB , FXE , FYB1 , FYE1 } ,
                                { FXB , FXE , FYB2 , FYE2 } ,
                                { FXB , FXE , FYB3 , FYE3 } ,
                                { B2 , E2 , B0 , E0 } ,
                                { B3 , E3 , B0 , E0 } ,
                                { B2 , E2 , B1 , E1 } ,
                                { B3 , E3 , B1 , E1 } ,
                                { B2 , E2 , B2 , E2 } ,
                                { B3 , E3 , B2 , E2 } ,
                                { 0 , 0 , 0 , 0 } ,
                                { 0 , 0 , 0 , 0 }    } ;


// rempli le paramétrage des boutons de chaque page 
void fillMPage (uint8_t _page , uint8_t _btnPos , uint8_t _boutons, uint8_t _actions , void (*_pfNext)(uint8_t) , uint8_t _parameters ) {
  mPages[_page].boutons[_btnPos] =  _boutons ;
  mPages[_page].actions[_btnPos] =  _actions ;
  mPages[_page].pfNext[_btnPos] =  _pfNext ;
  mPages[_page].parameters[_btnPos] =  _parameters ;
}

void initButtons() {  
mButton[_SETUP].pLabel = __SETUP  ;
mButton[_PRINT].pLabel = __PRINT  ;
mButton[_HOME].pLabel = __HOME  ;
mButton[_UNLOCK].pLabel = __UNLOCK  ;
mButton[_RESET].pLabel = __RESET  ;
mButton[_SD].pLabel = __SD  ;
mButton[_USB_GRBL].pLabel = __USB_GRBL  ;
mButton[_TELNET_GRBL].pLabel = __TELNET_GRBL  ;
mButton[_PAUSE].pLabel = __PAUSE  ;
mButton[_CANCEL].pLabel = __CANCEL  ;
mButton[_INFO].pLabel = __INFO  ;
mButton[_CMD].pLabel = __CMD  ;
mButton[_MOVE].pLabel = __MOVE  ;
mButton[_RESUME].pLabel = __RESUME  ;
mButton[_STOP_PC_GRBL].pLabel = __STOP_PC_GRBL ;
mButton[_XP].pLabel = "X+" ;
mButton[_XM].pLabel = "X-" ;
mButton[_YP].pLabel = "Y+" ;
mButton[_YM].pLabel = "Y-" ;
mButton[_ZP].pLabel = "Z+" ;
mButton[_ZM].pLabel = "Z-" ;
mButton[_AP].pLabel = "A+" ;
mButton[_AM].pLabel = "A-" ;
mButton[_D_AUTO].pLabel = __D_AUTO  ;
mButton[_D0_01].pLabel = "0.01" ;
mButton[_D0_1].pLabel = "0.1" ;
mButton[_D1].pLabel = "1" ;
mButton[_D10].pLabel = "10" ;
mButton[_SET_WCS].pLabel = __SET_WCS  ;
mButton[_SETX].pLabel = __SETX  ;
mButton[_SETY].pLabel = __SETY  ;
mButton[_SETZ].pLabel = __SETZ  ;
mButton[_SETA].pLabel = __SETA  ;
mButton[_SETXYZ].pLabel = __SETXYZ  ;
mButton[_SETXYZA].pLabel = __SETXYZA  ;
mButton[_TOOL].pLabel = __TOOL  ;
mButton[_SET_CHANGE].pLabel = __SET_CHANGE  ;
mButton[_SET_PROBE].pLabel = __SET_PROBE  ;
mButton[_SET_CAL].pLabel = __SET_CAL  ;
mButton[_GO_CHANGE].pLabel = __GO_CHANGE ; 
mButton[_GO_PROBE].pLabel = __GO_PROBE ;
mButton[_BACK].pLabel = __BACK  ;
mButton[_LEFT].pLabel = __LEFT  ;
mButton[_RIGHT].pLabel = __RIGHT  ;
mButton[_UP].pLabel = __UP  ;
mButton[_CMD1].pLabel = &cmdName[0][0] ;
mButton[_CMD2].pLabel = &cmdName[1][0] ;
mButton[_CMD3].pLabel = &cmdName[2][0] ;
mButton[_CMD4].pLabel = &cmdName[3][0] ;
mButton[_CMD5].pLabel = &cmdName[4][0] ;
mButton[_CMD6].pLabel = &cmdName[5][0] ;
mButton[_CMD7].pLabel = &cmdName[6][0] ;
mButton[_CMD8].pLabel = &cmdName[7][0] ;
mButton[_CMD9].pLabel = &cmdName[8][0] ;
mButton[_CMD10].pLabel = &cmdName[9][0] ;
mButton[_CMD11].pLabel = &cmdName[10][0] ;
mButton[_MORE_PAUSE].pLabel = __MORE_PAUSE ;
mButton[_FILE0].pLabel = fileNames[0] ;  // labels are defined during execution in a table
mButton[_FILE1].pLabel = fileNames[1] ;
mButton[_FILE2].pLabel = fileNames[2] ;
mButton[_FILE3].pLabel = fileNames[3] ;
mButton[_MASKED1].pLabel = "" ; // this is a hidden button; so must be empty
mButton[_PG_PREV].pLabel = __PREV ;
mButton[_PG_NEXT].pLabel = __NEXT ;
mButton[_SD_SHOW].pLabel = __SD_SHOW ; 
mButton[_OVERWRITE].pLabel = "" ; // this is a hidden button; so must be empty
mButton[_OVER_SWITCH_TO_FEEDRATE].pLabel = __OVER_SWITCH_TO_FEEDRATE ;
mButton[_OVER_SWITCH_TO_SPINDLE].pLabel = __OVER_SWITCH_TO_SPINDLE ;
mButton[_OVER_10P].pLabel = __OVER_10P ;
mButton[_OVER_10M].pLabel = __OVER_10M ;
mButton[_OVER_1P].pLabel = __OVER_1P ;
mButton[_OVER_1M].pLabel = __OVER_1M ;
mButton[_OVER_100].pLabel = __OVER_100 ;

mPages[_P_INFO].titel = "Info" ;
mPages[_P_INFO].pfBase = fInfoBase ;
fillMPage (_P_INFO , 0 , _MASKED1 , _JUST_PRESSED , fGoToPage , _P_LOG ) ; // this button is masked but clicking on the zone call another screen
fillMPage (_P_INFO , 3 , _OVERWRITE , _JUST_PRESSED , fGoToPage , _P_OVERWRITE ) ; // this button is masked but clicking on the zone call another screen
fillMPage (_P_INFO , 7 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP) ;   // those buttons are changed dynamically based on status (no print, ...)
fillMPage (_P_INFO , 11 , _PRINT , _JUST_PRESSED , fGoToPage , _P_PRINT) ;   // those buttons are changed dynamically based on status (no print, ...)

mPages[_P_SETUP].titel = "Setup" ;
mPages[_P_SETUP].pfBase = fSetupBase ;
fillMPage (_P_SETUP , 4 , _HOME , _JUST_PRESSED , fHome , 0) ;
fillMPage (_P_SETUP , 5 , _UNLOCK , _JUST_PRESSED , fUnlock , 0) ;
fillMPage (_P_SETUP , 6 , _RESET , _JUST_PRESSED , fReset , 0) ;
fillMPage (_P_SETUP , 7 , _CMD , _JUST_PRESSED , fGoToPage , _P_CMD ) ;
fillMPage (_P_SETUP , 8 , _MOVE , _JUST_PRESSED , fGoToPage , _P_MOVE ) ;
fillMPage (_P_SETUP , 9 , _SET_WCS , _JUST_PRESSED , fGoToPage , _P_SETXYZ ) ;
fillMPage (_P_SETUP , 10 , _TOOL , _JUST_PRESSED , fGoToPage , _P_TOOL ) ;
fillMPage (_P_SETUP , 11 , _INFO , _JUST_PRESSED , fGoToPageAndClearMsg ,  _P_INFO) ;

mPages[_P_PRINT].titel = "Print" ;
mPages[_P_PRINT].pfBase = fNoBase ;
fillMPage (_P_PRINT , 4 , _SD , _JUST_PRESSED , fGoToPage , _P_SD) ;
fillMPage (_P_PRINT , 5 , _USB_GRBL , _JUST_PRESSED , fStartUsb , 0) ;
fillMPage (_P_PRINT , 6 , _TELNET_GRBL , _JUST_PRESSED , fStartTelnet , 0) ;
fillMPage (_P_PRINT , 7 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP) ;
fillMPage (_P_PRINT , 10 , _CMD , _JUST_PRESSED , fGoToPage , _P_CMD ) ;
fillMPage (_P_PRINT , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO) ;

mPages[_P_PAUSE].titel = "Paused" ;
mPages[_P_PAUSE].pfBase = fNoBase ;
fillMPage (_P_PAUSE , 4 , _CANCEL , _JUST_PRESSED , fCancel , 0) ;
fillMPage (_P_PAUSE , 5 , _RESUME , _JUST_PRESSED , fResume , 0) ;
fillMPage (_P_PAUSE , 6 , _SD_SHOW , _JUST_PRESSED , fGoToPage , _P_SD_SHOW ) ;
fillMPage (_P_PAUSE , 7 , _MOVE , _JUST_PRESSED , fGoToPage , _P_MOVE) ;
fillMPage (_P_PAUSE , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO) ;

mPages[_P_MOVE].titel = "Move" ;
mPages[_P_MOVE].pfBase = fMoveBase ;
#ifdef AA_AXIS
fillMPage (_P_MOVE , 0 , _XM , _JUST_LONG_PRESSED_RELEASED , fMove , _XM) ;
fillMPage (_P_MOVE , 1 , _YP , _JUST_LONG_PRESSED_RELEASED , fMove , _YP) ;
fillMPage (_P_MOVE , 2 , _XP , _JUST_LONG_PRESSED_RELEASED , fMove , _XP) ;
fillMPage (_P_MOVE , 3 , _ZP , _JUST_LONG_PRESSED_RELEASED , fMove , _ZP) ;
fillMPage (_P_MOVE , 4 , _AM , _JUST_LONG_PRESSED_RELEASED , fMove , _AM) ;
fillMPage (_P_MOVE , 5 , _YM , _JUST_LONG_PRESSED_RELEASED , fMove , _YM) ;
fillMPage (_P_MOVE , 6 , _AP , _JUST_LONG_PRESSED_RELEASED , fMove , _AP) ;
fillMPage (_P_MOVE , 7 , _ZM , _JUST_LONG_PRESSED_RELEASED , fMove , _ZM) ;
#else
fillMPage (_P_MOVE , 1 , _YP , _JUST_LONG_PRESSED_RELEASED , fMove , _YP) ;
fillMPage (_P_MOVE , 3 , _ZP , _JUST_LONG_PRESSED_RELEASED , fMove , _ZP) ;
fillMPage (_P_MOVE , 5 , _YM , _JUST_LONG_PRESSED_RELEASED , fMove , _YM) ;
fillMPage (_P_MOVE , 7 , _ZM , _JUST_LONG_PRESSED_RELEASED , fMove , _ZM) ;
fillMPage (_P_MOVE , 8 , _XM , _JUST_LONG_PRESSED_RELEASED , fMove , _XM) ;
fillMPage (_P_MOVE , 10 , _XP , _JUST_LONG_PRESSED_RELEASED , fMove , _XP) ;
#endif
fillMPage (_P_MOVE , POS_OF_MOVE_D_AUTO , _D_AUTO , _JUST_PRESSED , fDist, _D_AUTO) ;  // -1 because range here is 0...11 
fillMPage (_P_MOVE , 11 , _BACK , _JUST_PRESSED , fGoBack , 0) ;

mPages[_P_SETXYZ].titel = "Set X, Y, Z to 0" ;  
mPages[_P_SETXYZ].pfBase = fSetXYZBase ;
fillMPage (_P_SETXYZ , 4 , _SETX , _JUST_PRESSED , fSetXYZ , _SETX) ;
fillMPage (_P_SETXYZ , 5 , _SETY , _JUST_PRESSED , fSetXYZ , _SETY) ;
fillMPage (_P_SETXYZ , 6 , _SETZ, _JUST_PRESSED , fSetXYZ , _SETZ) ;
#ifdef AA_AXIS
fillMPage (_P_SETXYZ , 7 , _SETA, _JUST_PRESSED , fSetXYZ , _SETA) ;
fillMPage (_P_SETXYZ , 8 , _SETXYZ , _JUST_PRESSED , fSetXYZ , _SETXYZ) ;
fillMPage (_P_SETXYZ , 9 , _SETXYZA , _JUST_PRESSED , fSetXYZ , _SETXYZA) ;
#else
fillMPage (_P_SETXYZ , 8 , _SETXYZ , _JUST_PRESSED , fSetXYZ , _SETXYZ) ;
#endif
fillMPage (_P_SETXYZ , 10 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP ) ;
fillMPage (_P_SETXYZ , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;

mPages[_P_SD].titel = "Select a file on Sd card" ;  // this screen has only 10 buttons instead of 12
mPages[_P_SD].pfBase = fSdBase ;   // cette fonction doit remplir les 4 premiers boutons en fonction des fichiers disponibles
fillMPage (_P_SD , 6 , _PG_PREV , _JUST_PRESSED , fSdMove , _PG_PREV ) ;
fillMPage (_P_SD , 7 , _UP , _JUST_PRESSED , fSdMove , _UP ) ;
fillMPage (_P_SD , 8 , _PG_NEXT , _JUST_PRESSED , fSdMove , _PG_NEXT) ;
fillMPage (_P_SD , 9 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;

mPages[_P_CMD].titel = "Select a command" ;
mPages[_P_CMD].pfBase = fCmdBase ; // 
if (cmdName[0][0] ) fillMPage (_P_CMD , 0 , _CMD1 , _JUST_PRESSED , fCmd , _CMD1) ; // le paramètre contient le n° du bouton
if (cmdName[1][0] ) fillMPage (_P_CMD , 1 , _CMD2 , _JUST_PRESSED , fCmd , _CMD2) ; // le paramètre contient le n° du bouton
if (cmdName[2][0] ) fillMPage (_P_CMD , 2 , _CMD3 , _JUST_PRESSED , fCmd , _CMD3) ; // le paramètre contient le n° du bouton
if (cmdName[3][0] ) fillMPage (_P_CMD , 3 , _CMD4 , _JUST_PRESSED , fCmd , _CMD4) ; // le paramètre contient le n° du bouton
if (cmdName[4][0] ) fillMPage (_P_CMD , 4 , _CMD5 , _JUST_PRESSED , fCmd , _CMD5) ; // le paramètre contient le n° du bouton
if (cmdName[5][0] ) fillMPage (_P_CMD , 5 , _CMD6 , _JUST_PRESSED , fCmd , _CMD6) ; // le paramètre contient le n° du bouton
if (cmdName[6][0] ) fillMPage (_P_CMD , 6 , _CMD7 , _JUST_PRESSED , fCmd , _CMD7) ; // le paramètre contient le n° du bouton
if (cmdName[7][0] ) fillMPage (_P_CMD , 7 , _CMD8 , _JUST_PRESSED , fCmd , _CMD8) ; // le paramètre contient le n° du bouton
if (cmdName[8][0] ) fillMPage (_P_CMD , 8 , _CMD9 , _JUST_PRESSED , fCmd , _CMD9) ; // le paramètre contient le n° du bouton
if (cmdName[9][0] ) fillMPage (_P_CMD , 9 , _CMD10 , _JUST_PRESSED , fCmd , _CMD10) ; // le paramètre contient le n° du bouton
if (cmdName[10][0] ) fillMPage (_P_CMD , 10 , _CMD11 , _JUST_PRESSED , fCmd , _CMD11) ; // le paramètre contient le n° du bouton
fillMPage (_P_CMD , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;

mPages[_P_LOG].titel = "Log" ;
mPages[_P_LOG].pfBase = fLogBase ;
fillMPage (_P_LOG , POS_OF_LOG_PG_PREV , _PG_PREV , _JUST_PRESSED , fLogPrev , 0) ;
fillMPage (_P_LOG , POS_OF_LOG_PG_NEXT , _PG_NEXT , _JUST_PRESSED , fLogNext , 0) ;
fillMPage (_P_LOG , 11 , _BACK , _JUST_PRESSED , fGoBack , 0) ;

mPages[_P_TOOL].titel = "Change tool" ;
mPages[_P_TOOL].pfBase = fToolBase ;
fillMPage (_P_TOOL , 4 , _SETXYZ , _JUST_PRESSED , fSetXYZ , _SETXYZ) ;
fillMPage (_P_TOOL , 5 , _SET_CAL , _JUST_PRESSED , fSetXYZ , _SET_CAL) ;
fillMPage (_P_TOOL , 6 , _GO_CHANGE , _JUST_PRESSED , fSetXYZ , _GO_CHANGE) ;
fillMPage (_P_TOOL , 7 , _GO_PROBE , _JUST_PRESSED , fSetXYZ , _GO_PROBE) ;
fillMPage (_P_TOOL , 8 , _SET_CHANGE , _JUST_PRESSED , fSetXYZ , _SET_CHANGE ) ;
fillMPage (_P_TOOL , 9 , _SET_PROBE , _JUST_PRESSED , fSetXYZ , _SET_PROBE) ;
fillMPage (_P_TOOL , 10 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP ) ;
fillMPage (_P_TOOL , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;

mPages[_P_SD_SHOW].titel = "Show Sd" ;
mPages[_P_SD_SHOW].pfBase = fSdShowBase ;
fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_PREV , _PG_PREV , _JUST_PRESSED , fSdShowPrev , 0) ;
fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_NEXT , _PG_NEXT , _JUST_PRESSED , fSdShowNext , 0) ;
fillMPage (_P_SD_SHOW , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;


mPages[_P_OVERWRITE].titel = "Change tool" ;
mPages[_P_OVERWRITE].pfBase = fOverBase ;
fillMPage (_P_OVERWRITE , POS_OF_OVERWRITE_OVERWRITE , _OVER_SWITCH_TO_SPINDLE , _JUST_PRESSED , fOverSwitch , _OVER_SWITCH_TO_SPINDLE ) ;
fillMPage (_P_OVERWRITE , 4 , _OVER_10M , _JUST_PRESSED , fOverModify , _OVER_10M) ;
fillMPage (_P_OVERWRITE , 5 , _OVER_1M , _JUST_PRESSED , fOverModify , _OVER_1M) ;
fillMPage (_P_OVERWRITE , 6 , _OVER_1P , _JUST_PRESSED , fOverModify , _OVER_1P) ;
fillMPage (_P_OVERWRITE , 7 , _OVER_10P , _JUST_PRESSED , fOverModify , _OVER_10P) ;
fillMPage (_P_OVERWRITE , 10 , _OVER_100 , _JUST_PRESSED , fOverModify , _OVER_100) ;
fillMPage (_P_OVERWRITE , 11 , _INFO , _JUST_PRESSED , fGoToPage , _P_INFO ) ;

}  // end of init



void tftInit() {
  tft.init() ; // Initialise l'écran avec les pins définies dans setup (par exemple)
                // #define TFT_MISO 19
                //#define TFT_MOSI 23
                //#define TFT_SCLK 18
                //#define TFT_CS   15  // Chip select control pin
                //#define TFT_DC    2  // Data Command control pin (to connect to RS pin of LDC probably)
                //#define TFT_RST   4  // Reset pin (could connect to RST pin)
                //#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
                //Cette fonction met les pins en mode input/output; elle envoie aussi les commandes d'initialisation
  // Set the rotation before we calibrate
  tft.setRotation(1); // normally, this is already done in tft.int() but it is not clear how is rotation set (probably 0); so it can be usefull to change it here
  touch_calibrate(); // call screen calibration
  //tft.printCalibration() ;  // print calibration data (print on Serial port the calibration data ; only for debug
  clearScreen();   // clear screen
}


boolean convertPosToXY( uint8_t pos , int32_t *_x, int32_t *_y , uint16_t btnDef[12][4]){
  if (pos > 0 && pos < 13 ) {                // accept only value from 1 to 12
    *_x = btnDef[pos-1][0];
    *_y = btnDef[pos-1][2];
    return true ;
  } else {
    return false ;
  }

} ;

uint8_t convertBtnPosToBtnIdx( uint8_t page , uint8_t btn ) {  // retrieve the type of button based on the position on TFT (in range 1...12)
  return mPages[page].boutons[btn - 1] ;
}

void drawAllButtons(){
    uint8_t i = 0;
    uint8_t btnIdx;
    while ( i < 12 ) {           // pour chacun des 12 boutons possibles
      btnIdx = mPages[currentPage].boutons[i] ;
      //if ( btnIdx && btnIdx != _MASKED1 ) {  // si un n° de bouton est précisé, l'affiche sauf si c'est un bouton masqué (ex: _MASKED1)
      if ( btnIdx && mButton[btnIdx].pLabel[0] != 0 ) {  // si un n° de bouton est précisé, l'affiche sauf si c'est un bouton masqué (dont le label est vide)
        //Serial.print("va afficher le bouton ") ; Serial.println( i) ;
        //Serial.print("bouton code= " ) ; Serial.println( mPages[currentPage].boutons[i] ) ;
        mButtonDraw( i + 1 , btnIdx ) ; // affiche le bouton
      }
      i++ ;
    }
}




void mButtonDraw(uint8_t pos , uint8_t btnIdx) {  // draw a button at position (from 1 to 12) ; btnIdx = n° du bouton à afficher
//  si le texte a moins de 5 char, affiche 1 ligne en size 2
//                         9               1               1
//                         17              2               1 ; le . sert de séparateur pour répartir les noms de fichier sur 2 lignes si possible 
// si le btnIdx est un bouton masqué (ex _MASKED1), alors on n'affiche rien
  int32_t _xl , _yl ;
  int32_t _w = 76 ;
  int32_t _h = 76 ;
  int32_t fill = BUTTON_BACKGROUND ;
  int32_t outline = BUTTON_BORDER_NOT_PRESSED ;
  int32_t text = BUTTON_TEXT ;
  char * pbtnLabel ; 
  boolean isFileName = false ;
  int32_t y1 ;
  char tempText[9] ;              // keep max 8 char + /0
  char * pExtensionChar;
  char * pch;
  uint8_t numbChar = 8 ; 
  char tempLabel[50] ;  
  char fileExtension[14] = "." ;
  char fileNameReduced[21] = "0123456789012345..." ;
  boolean convertPosIsTrue ;
    
  pbtnLabel = mButton[btnIdx].pLabel ;
  if (  currentPage == _P_SD && btnIdx >= _FILE0 && btnIdx <= _FILE3  ) { // if it is a button for a file name
    _w = 74+6+80 ;
    _h = 56 ;
    isFileName = true ;
    if ( *mButton[btnIdx].pLabel == '/' ) {    // when first char is "/", invert the color and skip the first char
      fill = BUTTON_BORDER_NOT_PRESSED ;
      text = BUTTON_BACKGROUND ;
      pbtnLabel++ ;
    }
    // remove extension if any and put it in fileExtension[]
    pExtensionChar = strrchr( pbtnLabel , '.' ); // search for first . from the right
    if ( pExtensionChar ) {
      *pExtensionChar = 0 ; // if any, replace . by 0 to truncate the string
      memccpy( fileExtension +1 , pExtensionChar + 1 , '\0' , 12) ; // copy max 12 char
      fileExtension[12] = 0 ; // put a 0 as last Char for security      
    } else {
      fileExtension[0] = 0 ; // reduce size of extension to 0
    }
    memccpy( fileNameReduced , pbtnLabel , '\0' , 16) ; // copy max 16 char
      
  }  
//  Serial.print("pos="); Serial.print( pos ) ; Serial.print(" btnIdx="); Serial.print( btnIdx ) ;
//  Serial.print("first char of btnName ="); Serial.println( *pbtnLabel ) ;
  if (  currentPage == _P_SD ) {
    convertPosIsTrue =  convertPosToXY( pos , &_xl, &_yl , btnDefFiles ) ;          //  Convert position index to colonne and line (top left corner) 
  } else {
    convertPosIsTrue =  convertPosToXY( pos , &_xl, &_yl , btnDefNormal ) ;          //  Convert position index to colonne and line (top left corner) 
  }
  if ( convertPosIsTrue ) {
    tft.setTextColor(text);
    tft.setTextSize(1);
    //tft.setTextFont(2);
    uint8_t r = min(_w, _h) / 4; // Corner radius
    tft.fillRoundRect( _xl , _yl , _w, _h, r, fill);
    tft.drawRoundRect( _xl, _yl , _w, _h, r, outline);
    uint8_t tempdatum = tft.getTextDatum(); 
    tft.setTextDatum(MC_DATUM);
    
    if (isFileName ) {                // print filename in a special way (larger button)
      tft.setTextDatum(BC_DATUM);
      tft.setFreeFont(LABELS9_FONT);
      tft.drawString( fileNameReduced , _xl + (_w/2), _yl + (_h/2));
      tft.setTextDatum(TC_DATUM);
      tft.drawString( fileExtension , _xl + (_w/2), _yl + (_h/2)); 
      tft.setTextDatum(tempdatum);
      return ;
    }
    memccpy( tempLabel , pbtnLabel , '\0' , 16); // copy max 16 char
    pch = strchr(tempLabel , '*') ; //check for a separator in the name in order to split on 2 lines   
    if  ( pch!=NULL ) {
      numbChar = pch - tempLabel ;
      if (numbChar <= 8 ) {
        tempLabel[numbChar] = 0 ;               // replace * by end of string
        tft.setFreeFont(LABELS9_FONT);
        //tft.setTextFont(2); 
        tft.drawString( &tempLabel[0] , _xl + (_w/2), _yl + (_h/3)); // affiche max 8 char sur la première ligne
        tempLabel[numbChar + 9] = 0 ;           // set a 0 to avoid that second part of name exceed 8 char 
        tft.drawString( &tempLabel[numbChar + 1]  , _xl + (_w/2), _yl + 2 * (_h/3)); // affiche la seconde ligne avec max 8 char
        return;
      }
    }
    // here it is asked to split in 2 lines (or the split was asked after the 8th char)
    // we can still split if there is more than 8 char; if there is less than 4, we use another font 
    tft.setFreeFont(LABELS9_FONT);
    uint8_t txtLength = strlen( pbtnLabel ) ;  // get the length of text to be displayed
    if ( txtLength <= 4 ) {                                               // imprime 1 ligne au milieu en grand
      tft.setFreeFont(LABELS12_FONT); 
      tft.drawString( pbtnLabel , _xl + (_w/2), _yl + (_h/2));  
    } else if ( txtLength <= 8 ) {                                        // imprime 1 ligne au milieu  
        tft.drawString( pbtnLabel , _xl + (_w/2), _yl + (_h/2));  
    } else { 
      if( memccpy( tempLabel , pbtnLabel , '\0' , 16) == NULL ) tempLabel[16] = 0 ; // copy the label in tmp
      pch = strchr(tempLabel , '.') ;
      if  ( pch!=NULL ) {
        numbChar = pch - tempLabel ;
        if (numbChar >= 8 ) numbChar = 8 ;
      }
      uint8_t tempChar =  tempLabel[numbChar] ; // sauvegarde le charactère '.' ou un autre
//      Serial.print("numChar="); Serial.print(numbChar) ;
//      Serial.print("txtLenth="); Serial.print(txtLength) ;
//      Serial.print("tempChar="); Serial.print(tempChar) ; Serial.print(" , "); Serial.print(tempChar , HEX) ;
      tempLabel[numbChar] = 0 ;            // put a 0 instead 
      tft.drawString( tempLabel , _xl + (_w/2), _yl + (_h/3)); // affiche max 7 char sur la première ligne (ou la partie avant .)
      tempLabel[numbChar] = tempChar ;    // restore the char
      if ( (txtLength - numbChar) >= 8 ) {
        tempLabel[numbChar + numbChar + 9 ] = 0 ;     // put a 0 to avoid a too long string 
      }  
      tft.drawString( &tempLabel[numbChar]  , _xl + (_w/2), _yl + 2 * (_h/3)); // affiche la seconde ligne avec max 8 char
    }   
    tft.setTextDatum(tempdatum);
  }  
}

void mButtonBorder(uint8_t pos , uint16_t outline) {  // draw the border of a button at position (from 1 to 12)
  int32_t _xl , _yl ;
  int32_t _w = 76 ;
  int32_t _h = 76 ;
  if (currentPage == _P_SD ) {
    convertPosToXY( pos , &_xl, &_yl , btnDefFiles) ;
    if ( pos <= 4) { // if it is a button for a file name
      _w = 74+6+80 ;
      _h = 56 ;
    }  
  } else {
    convertPosToXY( pos , &_xl, &_yl , btnDefNormal) ;
  }
  uint8_t r = min(_w, _h) / 4; // Corner radius
  tft.drawRoundRect( _xl, _yl , _w, _h, r, outline);
}
 
uint8_t getButton( int16_t x, int16_t y  , uint16_t btnDef[12][4]) {    // convert x y into a button if possible
                                                 // return 1 à 12 suivant le bouton; return 0 if no button
  if ( y > 240 || x > 320 ) return 0 ;
  uint8_t i = 12 ;
  while ( i ) {
    i--;
    if ( x > btnDef[i][0] && x < btnDef[i][1] && y > btnDef[i][2] && y < btnDef[i][3] ) return i+1 ; 
  } 
  return 0 ;
}


//********************************************************************************
//                    look if touch screen has been pressed/released
//                    if pressed, convert the x,y position into a button number (from 1 ...12)
//                    compare to previous state; if previous state is the same, then take new state into account
//                    fill justPressedBtn, justReleasedBtn, currentBtn , longPressedBtn , beginChangeBtnMillis
//*********************************************************************************
#define WAIT_TIME_BETWEEN_TOUCH 50
void updateBtnState( void) {
  int16_t x , y ;
  static uint32_t nextMillis ;
  static uint8_t prevBt0 ; 
  uint32_t touchMillis = millis(); 
  uint8_t bt ;
  uint8_t bt0 = 0; // bt0 is the button nr based on the touched position (0 if not touched, or if touched is not at a valid position, otherwise 1...12)
  boolean touchPressed ;
  justPressedBtn = 0 ;
  justReleasedBtn = 0 ;  
  if ( touchMillis > nextMillis ) {    // s'il n'y a pas assez longtemps depuis la dernière lecture, on fait juste un reset des justPressedBtn et justReleasedBtn
    touchPressed = tft.getTouch( &x,  &y, 600);   // read the touch screen; // later perhaps exit immediately if IrqPin is HIGH (not touched)
                                                // false = key not pressed
    nextMillis = touchMillis + WAIT_TIME_BETWEEN_TOUCH ;
    if ( touchPressed)  {
      if ( currentPage == _P_SD ) {             // conversion depend on current screen.
        bt0 = getButton(x , y , btnDefFiles) ;  // convertit x, y en n° de bouton ; retourne 0 si en dehors de la zone des boutons; sinon retourne 1 à 12
      } else {
        bt0 = getButton(x , y , btnDefNormal) ;  // convertit x, y en n° de bouton ; retourne 0 si en dehors de la zone des boutons; sinon retourne 1 à 12
      }  
    Serial.print("x=") ; Serial.print(x) ; Serial.print( " ," ) ; Serial.print( y) ; Serial.print( " ," ) ; Serial.println(bt0) ;
    } else {
//      Serial.print("!") ; 
    }
    if ( prevBt0 != bt0 ) { // compare bt0 with previous,
        prevBt0 = bt0 ;     // if different, only save the new state but do not handel  
    } else {  // traite uniquement si 2 fois le même bouton consécutivement 
      if ( currentBtn != bt0 ) {    // en cas de changement, active justPressedBtn et justReleasedBtn et reset waitReleased
//      Serial.print(" x=") ; Serial.print(x) ; Serial.print("  y=") ; Serial.print(y) ; Serial.print("  bt=") ; Serial.println(bt0) ; 
         justPressedBtn = bt0 ;
         justReleasedBtn = currentBtn ;
         currentBtn = bt0 ;
         waitReleased = false ;
         beginChangeBtnMillis = touchMillis ;  // save the timestamp when bt has just changed
         longPressedBtn = 0 ;              // reset long press 
      } else {                  // same button pressed
        if ( ( touchMillis - beginChangeBtnMillis ) > 500 ) {
          longPressedBtn = currentBtn ;    // if the same button is pressed for a long time, update longPressedBtn (can be 0)   
        }  
      }
    }
  }
  //if (justPressedBtn){
  //  Serial.print( "just pressed") ;   Serial.println( justPressedBtn) ;    
  //}
  //if (justReleasedBtn){
  //  Serial.print( "just released") ;   Serial.println( justReleasedBtn) ;    
  //}
}

void drawUpdatedBtn( ) {   // update the color of the buttons on a page (based on currentPage, justPressedBtn , justReleasedBtn, longPressedBtn)
  if ( justReleasedBtn && mPages[currentPage].boutons[justReleasedBtn - 1] && 
                      (  mButton[mPages[currentPage].boutons[justReleasedBtn - 1]].pLabel[0] != 0 ) ) {  // si justReleased contient un bouton et que ce bouton est affiché sur la page
    mButtonBorder( justReleasedBtn , BUTTON_BORDER_NOT_PRESSED ) ; // affiche le bord du bouton dans sa couleur normale
  }
  if ( justPressedBtn && mPages[currentPage].boutons[justPressedBtn - 1] && ( mButton[mPages[currentPage].boutons[justPressedBtn - 1]].pLabel[0] != 0 ) ) {  // si justPressed contient un bouton et que ce bouton est affiché sur la page
    mButtonBorder( justPressedBtn , BUTTON_BORDER_PRESSED ) ; // affiche le bord du bouton dans la couleur de sélection
  }
}

void executeMainActionBtn( ) {   // find and execute main action for ONE button (if any)
  uint8_t actionBtn = 0 ;
  if ( ( justPressedBtn ) && ( (mPages[currentPage].actions[justPressedBtn - 1] == _JUST_PRESSED ) \
                            || (mPages[currentPage].actions[justPressedBtn - 1 ] == _JUST_LONG_PRESSED ) \
                            || (mPages[currentPage].actions[justPressedBtn - 1 ] == _JUST_LONG_PRESSED_RELEASED )) ) {
    actionBtn = justPressedBtn  ;
  } else if ( ( longPressedBtn ) && ( (mPages[currentPage].actions[longPressedBtn - 1 ] == _LONG_PRESSED ) \
                                   || (mPages[currentPage].actions[longPressedBtn - 1 ] == _JUST_LONG_PRESSED ) \
                                   || (mPages[currentPage].actions[longPressedBtn - 1 ] == _JUST_LONG_PRESSED_RELEASED )) ) {
    actionBtn = longPressedBtn ;
  } else if ( ( justReleasedBtn ) && ( ( mPages[currentPage].actions[justReleasedBtn - 1 ] == _JUST_RELEASED ) \
                                    || ( mPages[currentPage].actions[justReleasedBtn - 1 ] == _JUST_LONG_PRESSED_RELEASED ) ) ) {
    actionBtn =  justReleasedBtn ;
  }
  if ( actionBtn) {
    (*mPages[currentPage].pfNext[actionBtn - 1 ])(mPages[currentPage].parameters[actionBtn - 1 ]) ;// execute the action with the predefined parameter
  }
}


// Basis functions
void blankTft(char * titel, uint16_t x , uint16_t y) {    // blank screen and display one titel
  clearScreen() ;
  //tft.fillScreen( SCREEN_BACKGROUND ) ;
  // currently titel is not used so folowwing lines can be skipped; uncomment if titel would be used
  //tft.setTextFont( 2 ); // use Font2 = 16 pixel X 7 probably
  //tft.setTextColor(TFT_GREEN ,  TFT_BLACK) ; // when oly 1 parameter, background = fond);
  //tft.setTextSize(1) ;           // char is 2 X magnified => 
  //tft.setTextDatum( TL_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  //tft.drawString( titel , x , y ) ;     // affiche un texte
  //tft.setCursor( x , y + 30 , 2) ; // x, y, font
}

void clearScreen() {
  tft.fillScreen( SCREEN_BACKGROUND ) ;
}

void printTft(char * text ) {     // print a text on screen
  tft.print( text ) ;
}

// affichage d'une nouvelle page = fonction drawPage(pageIdx)
//          remplir la page d'une couleur
//          exécuter le pointeur correspondant aux paramètres de la page
//          12 fois:
//                 si le n° du bouton existe
//                    afficher le bouton
void drawFullPage() {
  //uint32_t fullPageMillis = millis();
  clearScreen() ;
  mPages[currentPage].pfBase();   // exécute la fonction de base prévue pour la page
  //Serial.println("drawFullPage: fin de l'appel de la page de base") ;
  //Serial.print("drawFullPage takes");Serial.println(millis() - fullPageMillis); // most of time it takes about 100msec to redraw the full screen
  drawAllButtons();
}

void drawPartPage() {          // update only the data on screen (not the button)
// si la page courante est celle d'info, met à jour certaines données (positions, statut...)
// si la page courante est celle de Move, met à jour certaines données (positions, statut...)
// etc...
  if ( currentPage == _P_INFO) {
    drawDataOnInfoPage() ;
  } else if ( currentPage == _P_MOVE) {
    drawWposOnMovePage() ;
  } else if (currentPage == _P_SETUP ){
    drawDataOnSetupPage() ;
  } else if (currentPage == _P_SETXYZ ){
    drawDataOnSetXYZPage() ;
  } else if (currentPage == _P_TOOL ){
    drawDataOnToolPage() ;
  } else if (currentPage == _P_OVERWRITE ){
    drawDataOnOverwritePage() ;
  }   
}

//********************************  Fonctions appelées quand on entre dans un écran (appelée via drawFullPage)
//                                  Elles changent seulement les paramètres des boutons.
//************************************************************************************************************                                  
void fInfoBase(void) { 
  updateButtonsInfoPage() ; // met à jour le set up de la page en fonction du statut d'impression
  statusPrintingPrev = 0xFF ;  // fill with a dummy value to force a redraw
  machineStatus0Prev = 0 ;  // fill with a dummy value to force a redraw
  lastMsgChanged = true ;   // force a redraw of Last Msg
  grblLastMessageChanged = true ;
  statusTelnetIsConnectedPrev = ! statusTelnetIsConnected ;
  newInfoPage = true ; 
  drawDataOnInfoPage() ;    // affiche les données sur la page info  
}

void updateButtonsInfoPage (void) { // met à jour le set up de la page en fonction du statut d'impression
  switch ( statusPrinting ) {
    case PRINTING_STOPPED :
      fillMPage (_P_INFO , 7 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP) ;
      fillMPage (_P_INFO , 11 , _PRINT , _JUST_PRESSED , fGoToPage , _P_PRINT) ;
      break ;
    case PRINTING_FROM_SD :
      fillMPage (_P_INFO , 7 , _PAUSE , _JUST_PRESSED , fPause , 0 ) ;
      fillMPage (_P_INFO , 11 , _CANCEL , _JUST_PRESSED , fCancel , 0 ) ;
      break ;
    case PRINTING_ERROR :                                              // to do; not clear what we should do
      fillMPage (_P_INFO , 7 , _SETUP , _JUST_PRESSED , fGoToPage , _P_SETUP) ;
      fillMPage (_P_INFO , 11 , _PRINT , _JUST_PRESSED , fGoToPage , _P_PRINT) ;
      break ;
    case PRINTING_PAUSED :
      fillMPage (_P_INFO , 7 , _RESUME , _JUST_PRESSED , fResume , 0 ) ;
      fillMPage (_P_INFO , 11 , _MORE_PAUSE , _JUST_PRESSED , fGoToPage , _P_PAUSE) ;
      break ;
    case PRINTING_FROM_USB :
      fillMPage (_P_INFO , 7 , _STOP_PC_GRBL , _JUST_PRESSED , fStopPc , 0 ) ;
      fillMPage (_P_INFO , 11 , _NO_BUTTON , _NO_ACTION , fGoToPage , _P_INFO ) ;
      break ;
    case PRINTING_FROM_TELNET :
      fillMPage (_P_INFO , 7 , _STOP_PC_GRBL , _JUST_PRESSED , fStopPc , 0 ) ;
      fillMPage (_P_INFO , 11 ,_NO_BUTTON , _NO_ACTION , fGoToPage, _P_INFO ) ;
      break ;
    case PRINTING_CMD :
      fillMPage (_P_INFO , 7 , _PAUSE , _JUST_PRESSED , fPause , 0 ) ;
      fillMPage (_P_INFO , 11 , _CANCEL , _JUST_PRESSED , fCancel , 0 ) ;
      break ;
    case PRINTING_STRING :
      fillMPage (_P_INFO , 7 , _NO_BUTTON , _NO_ACTION , fPause , 0 ) ;
      fillMPage (_P_INFO , 11 , _CANCEL , _JUST_PRESSED , fCancel , 0 ) ;
      break ;  
  }
}


void drawDataOnInfoPage() { // to do : affiche les données sur la page d'info
//            USB<-->Grbl                  Idle                (or Run, Alarm, ... grbl status)  
//                                                 So, printing status (blanco, ,SD-->Grbl  xxx%, USB<-->Grbl , Pause,  Cmd)  = printing status
//                                                 and GRBL status (or Run, Alarm, ... grbl status)
//            Last message                   (ex : card inserted, card removed, card error, Error: 2 
//            Wifi icon
//              Wpos          Mpos
//            X xxxxxpos      xxxxxpos                 
//            Y yyyyypos      yyyyypos       
//            Z zzzzzpos      zzzzzpos  
//            F 100           S 10000
   
  if ( statusPrintingPrev != statusPrinting ) {
    tft.setFreeFont (LABELS12_FONT) ;
    tft.setTextSize(1) ;           // char is 2 X magnified => 
    tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
    
    tft.setTextDatum( TL_DATUM ) ; // align Left
    tft.setTextPadding (200) ; 
    tft.drawString ( &printingStatusText[statusPrinting][0] , 5 , 0 )  ;
    statusPrintingPrev = statusPrinting ;
  }
  
  if ( statusPrinting == PRINTING_FROM_SD ) {
      tft.setFreeFont (LABELS12_FONT) ;
      tft.setTextSize(1) ;           // char is 2 X magnified => 
      tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
      tft.setTextDatum( TR_DATUM ) ;
      tft.setTextPadding (40) ;
      tft.drawNumber( ( (100 * sdNumberOfCharSent) / sdFileSize), 170 , 0 ) ;
      tft.setTextPadding (1) ;
      tft.drawString( "%" , 190 , 0 ) ;
      // Serial.print( sdNumberOfCharSent ); Serial.print(  " / ") ; Serial.println( sdFileSize ); 
  }

  if ( machineStatus0Prev != machineStatus[0] || machineStatus[0] == 'A' || machineStatus[0] == 'H') {
      drawMachineStatus() ;
      machineStatus0Prev = machineStatus[0] ;
  }

  if ( lastMsgChanged ) {  
      drawLastMsg() ;  
      lastMsgChanged = false ;
  }

  if ( statusTelnetIsConnectedPrev != statusTelnetIsConnected) {    
      tft.setTextFont( 4);                              // font 4has been modified to have a wifi Icon for 0X7F
      tft.setTextPadding (10) ;
      tft.setTextSize(1) ;  
      if ( statusTelnetIsConnected ) {
        tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ;    // display in green when connected
      } else {
        tft.setTextColor(SCREEN_ALERT_TEXT ,  SCREEN_BACKGROUND ) ;      // display in red when not connected
      } 
      tft.setTextDatum( TL_DATUM ) ; 
      tft.drawChar (  0x7F , 2 , 62 ) ;  // char 0x7E in font 4 has been redesigned to get the Wifi logo
      statusTelnetIsConnectedPrev = statusTelnetIsConnected ;
  }    

  if ( grblLastMessageChanged ) {    
     
      tft.setTextSize(1) ;
      tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ;
      tft.setTextDatum( TL_DATUM ) ; // align Left
      tft.setTextPadding (290) ; 
      tft.setFreeFont(LABELS9_FONT);
      if ( strlen( grblLastMessage) > 30 ) {
        tft.drawString ( " " , 30 , 62) ; // print space first in the larger font to clear the pixels
        tft.setTextFont( 1 ); 
      }
      tft.drawString ( &grblLastMessage[0] , 30 , 62) ;
      grblLastMessageChanged = false ;
  }
  
  tft.setTextDatum( TR_DATUM ) ; 
  uint16_t line = 90 ;
  if (newInfoPage ) {
    tft.setTextColor( SCREEN_HEADER_TEXT ,  SCREEN_BACKGROUND ) ; 
    tft.setTextFont( 2 );
    tft.setTextSize(1) ;           
    tft.setTextPadding (0) ;      // expect to clear 70 pixel when drawing text or 
    tft.drawString( __WPOS , 70 , line ) ;     // affiche un texte
    tft.drawString( __MPOS , 190 , line ) ;     // affiche un texte
    newInfoPage = false ;
  }
  //tft.setTextSize(2) ;
  //tft.setTextFont( 2 );
  tft.setFreeFont (LABELS12_FONT) ;
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ;
  tft.setTextPadding (120) ;      // expect to clear 120 pixel when drawing text or float
  uint8_t c1 = 120, c2 = c1 + 120 ;
#ifdef AA_AXIS
  uint8_t lineSpacing1 = 24 ;
#else 
  uint8_t lineSpacing1 = 34 ; // we have more space for 3 axis
#endif   
  line += 20           ; tft.drawFloat( wposXYZA[0] , 2 , c1 , line ); tft.drawFloat( mposXYZA[0] , 2 , c2 , line ); 
  line += lineSpacing1 ; tft.drawFloat( wposXYZA[1] , 2 , c1 , line ); tft.drawFloat( mposXYZA[1] , 2 , c2 , line );
  line += lineSpacing1 ; tft.drawFloat( wposXYZA[2] , 2 , c1 , line ); tft.drawFloat( mposXYZA[2] , 2 , c2 , line  ); 
#ifdef AA_AXIS
  line += lineSpacing1 ; tft.drawFloat( wposXYZA[3] , 2 , c1 , line ); tft.drawFloat( mposXYZA[3] , 2 , c2 , line  );
#endif  
  tft.setTextFont( 2 );
  tft.setTextSize(1) ;
  tft.setTextPadding (0) ;
  tft.setTextDatum( TL_DATUM ) ;
  tft.setTextColor(SCREEN_HEADER_TEXT ,  SCREEN_BACKGROUND) ; 
  line += lineSpacing1 ; tft.drawString(__FEED , 90 , line)  ; tft.drawString(__RPM , 205 , line) ;
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND) ;
  tft.setTextPadding (80) ;
  tft.setTextDatum( TR_DATUM ) ;
  tft.drawNumber( (long) feedSpindle[0] , 80 , line) ; // here we could add the Feed rate and the spindle rpm
  tft.drawNumber( (long) feedSpindle[1] , 200 , line) ; // here we could add the Feed rate and the spindle rpm

}

void fNoBase(void) {
}

void fSetupBase(void) {
  tft.setFreeFont (LABELS9_FONT) ;
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when oly 1 parameter, background = fond);
  tft.setTextDatum( TL_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  uint8_t line = 2 ;
  uint8_t col = 1 ;
  char ipBuffer[20] ;
  if ( getWifiIp( ipBuffer ) ) { 
    tft.drawString( "IP:" , col , line );
    tft.drawString( ipBuffer , col + 20 , line ); // affiche la valeur avec 3 décimales 
  } else {
    tft.drawString( "No WiFi" , col , line ); 
  }
  tft.drawString( ESP32_VERSION,  col + 140 , line ) ;
}

void drawDataOnSetupPage() {  
  drawMachineStatus() ;       // draw machine status in the upper right corner
  drawLastMsg() ;
}

void fMoveBase(void) {
  fillMPage (_P_MOVE , POS_OF_MOVE_D_AUTO , _D_AUTO , _JUST_LONG_PRESSED , fDist , _D_AUTO) ;  // reset the button for autochange of speed
  wposMoveInitXYZA[0] = wposXYZA[0];             // save the position when entering (so we calculate distance between current pos and init pos on this screen)
  wposMoveInitXYZA[1] = wposXYZA[1];
  wposMoveInitXYZA[2] = wposXYZA[2];
  wposMoveInitXYZA[3] = wposXYZA[3];
  drawWposOnMovePage() ;
  // multiplier = 0.01 ; // à voir si cela sera encore utilisé (si on met à jour le bouton distance au fur et à mesure, on peut l'utiliser pour calculer 
  // movePosition = 0 ;  à utiliser si on l'affiche 
  // afficher la position des axes
}


void fSetXYZBase(void) {                 
  drawWposOnSetXYZPage() ;
}

void drawMachineStatus() {
      tft.setFreeFont (LABELS12_FONT) ;
      tft.setTextSize(1) ;           // char is 2 X magnified => 
      tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND) ; // when only 1 parameter, background = fond);
      tft.setTextDatum( TR_DATUM ) ;
      tft.setTextPadding (120) ;      // expect to clear 70 pixel when drawing text or 
      tft.drawString( &machineStatus[0] , 315  , 0 ) ; // affiche le status GRBL (Idle,....)
}

void drawLastMsg() {  // affiche le lastMsg en ligne 32  
      tft.setTextSize(1) ;
      tft.setTextColor(lastMsgColor ,  SCREEN_BACKGROUND ) ; // color is defined in lastMsgColor ; previously SCREEN_ALERT_TEXT
      tft.setTextDatum( TL_DATUM ) ; // align Left
      tft.setTextPadding (320) ; 
      tft.setFreeFont(LABELS9_FONT); 
      if ( strlen( lastMsg) > 30 ) {
        tft.drawString ( " " , 2 , 32) ; // print space first in the larger font to clear the pixels
        tft.setTextFont( 1 );           
      }
      tft.drawString ( &lastMsg[0] , 2 , 32) ;
}
      
void drawDataOnSetXYZPage() {
  drawWposOnSetXYZPage() ;
}

void drawWposOnSetXYZPage() {
  drawMachineStatus() ;
  drawLastMsg() ;
  tft.setTextFont( 2 ); // use Font2 = 16 pixel X 7 probably
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when oly 1 parameter, background = fond);
  tft.setTextDatum( TR_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  tft.setTextPadding (80) ;      // expect to clear 70 pixel when drawing text or   
  uint8_t line = 60 ; // was 60 ;
  uint8_t col = 60 ;
  tft.drawString( __WPOS , col , line);
  tft.drawFloat( wposXYZA[0] , 2 , col , line ); // affiche la valeur avec 3 décimales 
  tft.drawFloat( wposXYZA[1] , 2 , col + 80 , line );
  tft.drawFloat( wposXYZA[2] , 2 , col + 160 , line );
#ifdef AA_AXIS  
  tft.drawFloat( wposXYZA[3] , 2 , col + 240 , line );
#endif  
  //tft.drawString( __WPOS , col + 240 , line);
}


void fSdBase(void) {                // cette fonction doit vérifier que la carte est accessible et actualiser les noms des fichiers disponibles sur la carte sd
  if ( ! sdStart() ) {           // try to read the SD card; en cas d'erreur, rempli last message et retourne false
    currentPage = _P_INFO ;      // in case of error, go back to Info
    dirLevel = -1 ;              // force a reload of sd data next time we goes to SD card menu
    fInfoBase () ;               // prepare info page (affiche les data et redétermine les boutons à afficher)
  } else {           // if SD seems OK
    //Serial.println("fSdBase is ok") ;
    sdFileDirCnt = fileCnt() ;     // count the number of files in working dir
    if (sdFileDirCnt == 0) {
      firstFileToDisplay == 0 ;
    } else if ( firstFileToDisplay == 0 ) {
        firstFileToDisplay = 1 ;
    } else if ( (firstFileToDisplay + 4) > sdFileDirCnt ) {
      if (  sdFileDirCnt <= 4 ) {
        firstFileToDisplay = 1 ;     // reset firstFileToDisplay 0 if less than 5 files
      } else {
        firstFileToDisplay = sdFileDirCnt - 3 ;  // keep always 4 files on screen
      }  
    }
    //Serial.print("Nbr de fichiers") ; Serial.println(sdFileDirCnt) ;
    //Serial.print("firstFileToDisplay") ; Serial.println(firstFileToDisplay) ;
    //tft.setTextSize(2) ;
    tft.setFreeFont (LABELS9_FONT) ;
    tft.setTextSize(1) ;
    tft.setTextDatum( TL_DATUM ) ;
    tft.setCursor(180 , 20 ) ; // x, y, font
    tft.print( firstFileToDisplay ) ;
    tft.print( " / " ) ;
    tft.print( sdFileDirCnt ) ;  
    tft.setTextDatum( TR_DATUM ) ;
    char dirName[23] ;
    if ( ! aDir[dirLevel].getName( dirName , 16 ) ) { 
      fillMsg (__DIR_NAME_NOT_FOUND );
      currentPage = _P_INFO ;      // in case of error, go back to Info
      dirLevel = -1 ;              // force a reload of sd data next time we goes to SD card menu
      fInfoBase () ;               // fill info page with basis data (affiche les data et redétermine les boutons à afficher sans les afficher)
      return ;
    }
    tft.drawString( dirName , 310 , 40 );
    if ( ! updateFilesBtn() ) {             // met à jour les boutons à afficher; conserve le premier fichier affiché si possible ; retourne false en cas d'erreur
      //Serial.println( "updateFilesBtn retuns false"); 
      fillMsg( __BUG_UPDATE_FILE_BTN  );
      currentPage = _P_INFO ;      // in case of error, go back to Info
      dirLevel = -1 ;              // force a reload of sd data next time we goes to SD card menu
      fInfoBase () ;               // fill info page with basis data (affiche les data et redétermine les boutons à afficher sans les afficher)
    }
  }
}

void fCmdBase(void) {            //  En principe il n'y a rien à faire; 
}

void fLogBase(void) { // fonction pour l'affichage de l'écran Log 
  //Serial.print("justPressedBtn= ") ; Serial.println( justPressedBtn ) ; // to debug
  if (justPressedBtn == 1 ) { // when we go on this function directly from a previous screen using the MASKED button (btn 1 of 12)
                              // then we have to set up pGet in order to view the last lines
                              // this test is needed because the function can be called also when we have the flag updateFullPage = true
    uint8_t count = 0;
    pGet = pNext ;
    if (pGet != pFirst) {
      if (pGet == logBuffer) pGet = logBuffer + MAX_BUFFER_SIZE ;
      pGet--;         // Set Pget on last written Car
    }
    getPrevLogLine();
    pLastLogLine = pGet ;
    endOfLog = true ;
    if (pGet != pFirst) {
      fillMPage (_P_LOG , POS_OF_LOG_PG_PREV , _PG_PREV , _JUST_PRESSED , fLogPrev , 0) ; // activate PREV btn 
    } else {
      fillMPage (_P_LOG , POS_OF_LOG_PG_PREV , _NO_BUTTON , _NO_ACTION , fLogPrev , 0) ;  // deactivate PREV btn
    }
    fillMPage (_P_LOG , POS_OF_LOG_PG_NEXT , _NO_BUTTON , _NO_ACTION , fLogNext , 0) ; // deactivate the NEXT button
    //Serial.println("begin fLogBase");
    //Serial.print("pFirst at begin ") ; Serial.println( pFirst - logBuffer) ; // to debug
    //Serial.print("pget at begin ") ; Serial.println( pGet - logBuffer) ; // to debug
    //Serial.print("pNext at begin ") ; Serial.println( pNext - logBuffer) ; // to debug
    //Serial.print("pNext char (hex) = ") ;  Serial.println( *pNext, HEX ) ; // to debug
    while ( (count < ( N_LOG_LINE_MAX - 1) ) && ( getPrevLogLine()>=0 ) ) count++ ; // move back max 6 line before
    //Serial.print("nbr line to display"); Serial.println(count) ; // to debug
  }
  drawDataOnLogPage() ; // display from current position  
}

void fToolBase(void) {                 //  En principe il n'y a rien à faire;
  //drawWposOnSetXYZPage() ;
}

void fSdShowBase(void) { // fonction pour l'affichage de l'écran Sd show 
  //Serial.print("justPressedBtn= ") ; Serial.println( justPressedBtn ) ; // to debug
  if (justPressedBtn == 7 ) { // when we go on this function directly from a previous screen using a (masked) button (btn 1 of 12)
                              // then we have to set up some data in order to view the file lines
                              // Otherwise this set up is done in the function fSdShowPrev and fSdShowNext
            setShowBuffer() ;
  }
  drawDataOnSdShowPage() ; // display from current position  
}



void fOverBase(void) {                 //  En principe il n'y a rien à faire;
  drawDataOnOverwritePage() ;
}

void drawWposOnMovePage() {
#ifdef AA_AXIS
  //  X-    Y+      X+    Z+
  //  X-    Y+      X+    Z+
  //  X-    Y+      X+    Z+
  //  A-    Y-      A+    Z-
  //  A-    Y-      A+    Z-
  //  A-    Y-      A+    Z-
  //W 0,00 MO,00    Auto Back
  //  0,00  O,00    Auto Back
  //  0,00  O,00    Auto Back
  //  0,00  O,00    Auto Back

  tft.setTextFont( 2 ); // use Font2 = 16 pixel X 7 probably
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
  tft.setTextDatum( TR_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  tft.setTextPadding (75) ;      // expect to clear 70 pixel when drawing text or 
  
  uint8_t line = 160 ;
  uint8_t col1 = 75 ;
  uint8_t col2 = 75 + 80 ;
  tft.drawString( __WPOS , col1  , line );
  tft.drawString( __MOVE , col2  , line  );
  line +=16 ;
  //tft.drawString( "  X  " , col  , line + 80 );
  tft.drawFloat( wposXYZA[0] , 2 , col1 , line ); // affiche la valeur avec 2 décimales 
  tft.drawFloat( wposXYZA[0] - wposMoveInitXYZA[0] , 2 , col2 , line  ); // affiche la valeur avec 2 décimales 
  line +=16 ;
  //tft.drawString( "  Y  " , col + 160  , line + 80  );
  tft.drawFloat( wposXYZA[1] , 2 , col1 , line );
  tft.drawFloat( wposXYZA[1] - wposMoveInitXYZA[1] , 2 , col2 , line );
  line +=16 ;
  //tft.drawString( "  Z  " , col + 160  , line   );
  tft.drawFloat( wposXYZA[2] , 2 , col1 , line );
  tft.drawFloat( wposXYZA[2] - wposMoveInitXYZA[2] , 2 , col2 , line ) ;
  line +=16 ;
  tft.drawFloat( wposXYZA[3] , 2 , col1 , line );
  tft.drawFloat( wposXYZA[3] - wposMoveInitXYZA[3] , 2 , col2 , line ) ;
  
#else
  //        Y+      x     Z+
  // Wpos   Y+      0.0   Z+
  // Move   Y+      0.0   Z+
  //   x    Y-      y     Z-
  //   0.0  Y-      0.0   Z-
  //   0.0  Y-      0.0   Z-
  //   X-  Auto     X+    Back
  //   X-  Auto     X+    Back
  //   X-  Auto     X+    Back
  tft.setTextFont( 2 ); // use Font2 = 16 pixel X 7 probably
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
  tft.setTextDatum( TR_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  tft.setTextPadding (65) ;      // expect to clear 70 pixel when drawing text or 
  
  uint8_t line = 10 ;
  uint8_t col = 70 ;
  tft.drawString( __WPOS , col  , line + 20);
  tft.drawString( __MOVE , col  , line + 40 );
  
  tft.drawString( "  X  " , col  , line + 80 );
  tft.drawFloat( wposXYZA[0] , 2 , col , line + 100 ); // affiche la valeur avec 2 décimales 
  tft.drawFloat( wposXYZA[0] - wposMoveInitXYZA[0] , 2 , col , line + 120 ); // affiche la valeur avec 2 décimales 

  tft.drawString( "  Y  " , col + 160  , line + 80  );
  tft.drawFloat( wposXYZA[1] , 2 , col + 160 , line +100 );
  tft.drawFloat( wposXYZA[1] - wposMoveInitXYZA[1] , 2 , col + 160 , line + 120 );

  tft.drawString( "  Z  " , col + 160  , line   );
  tft.drawFloat( wposXYZA[2] , 2 , col + 160 , line + 20 );
  tft.drawFloat( wposXYZA[2] - wposMoveInitXYZA[2] , 2 , col + 160 , line + 40 ) ;

#endif
}


void drawDataOnLogPage() {
  // tft has already been cleared before we call this function 
  //Serial.println("begin drawing a page");
  //Serial.print("pFirst at begin ") ; Serial.println( pFirst - logBuffer) ; // to debug
  //Serial.print("pget at begin ") ; Serial.println( pGet - logBuffer) ; // to debug
  //Serial.print("pNext at begin ") ; Serial.println( pNext - logBuffer) ; // to debug
  //tft.setFreeFont (LABELS9_FONT) ;
  tft.setTextFont ( 1 ) ;
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
  tft.setTextDatum( TL_DATUM ) ; // align rigth ( option la plus pratique pour les float ou le statut GRBL)
  tft.setTextPadding (0) ;
  
  uint8_t line = 2 ;
  uint8_t col = 0 ;
  uint8_t count = N_LOG_LINE_MAX ;
  int16_t nCar = 0 ;
  while (count ) {
    printOneLogLine(col , line );
    count--;
    line += 10 ; // goes to next line on screen (was20 if we use setTextFont(1)
    if (getNextLogLine() < 0 ) count = 0 ; // stop if next line is not complete
  }
}

void printOneLogLine(uint8_t col , uint8_t line ) {
  lineOfText[49] = 0 ; //make sure there is an 0 at the end
  cpyLineToArray( lineOfText , pGet , 48) ; // copy one line
  tft.drawString( lineOfText , col , line) ;
}

void drawDataOnToolPage() {
  drawMachineStatus() ;       // draw machine status in the upper right corner
  drawLastMsg() ;  
}

char sdShowBuffer[1000] = "1234567890\nA1234567890\nB1234567890\nC1234567890\nD1234567890\nE1234567890\nF12345\nG1234567890\nH1234567890\nI1234567890\n"  ; // buffer containing the data to display
int16_t sdShowFirst = 11 ;     // index in buffer of first char to display in the buffer
int16_t sdShowMax = 11;       // index in buffer of the first char that has not been written in the buffer = Number of char written in sdShowBuffer

extern uint32_t sdShowBeginPos ;      // position in the SD file of the first char stored in the sdShowBuffer array   
extern uint32_t sdMillPos  ;      // position in the SD file of the next char to be sent to GRBL   
uint32_t sdShowNextPos ;         // End of the the data having been displayed ( in fact first char that is not displayed); used by Next
boolean sdShowPrevVisible ;      // true when the prev button must be displayed
boolean sdShowNextVisible ;      // true when the next button must be displayed

int16_t bufferLineLength(int16_t firstCharIdx , int16_t &nCharInFile ) {   //return the number of char to be displayed on TFT       
                                                                //-1=end of buffer, 0=only \n , >1 means there are some char; \n is not counted
                                                                // update nCharInFile with the number of char found in the buffer
  int16_t i = firstCharIdx ;
  int16_t nCharOnTft = 0;
  char c ; 
  nCharInFile = 0 ;
  if ( firstCharIdx >= sdShowMax ) return -1 ; // we are alredy at the end of the buffer 
  while ( i < sdShowMax ) {
    c = sdShowBuffer[i++] ;
    if (  c == '\n' ) { 
      break ;
    } else if ( c!= '\r') nCharOnTft++ ; // 
  }
  nCharInFile = i - firstCharIdx ;
  //Serial.print("For line beginning at ") ; Serial.println(firstCharIdx ); // to debug
  //Serial.print("nChar= ") ; Serial.println(i - firstCharIdx ); // to debug
  return  nCharOnTft ;
}

int16_t prevBufferLineLength( int16_t beginSearchAt ,  int16_t &nCharInFile ) {   //return the number of char to be displayed on TFT in the previous line      
                                                                //-1=No previous line, 0=only \n , >1 means there are some char; \n is not counted
                                                                // update nCharInFile with the number of char found in the buffer
    int16_t i = beginSearchAt ;
    int16_t nCharOnTft = 0;
    char c ;
    nCharInFile = 0 ;
    if (beginSearchAt == 0 ) return -1 ; // we are already at the beginning
    do {
      c = sdShowBuffer[--i] ;
      if ( c == '\n' ) break ; // exit first do on first \n
    } while (  i >= 0 ) ;
    if ( i == 0 ) return -1 ; // we are already at the beginning
    do {
      c = sdShowBuffer[--i] ;
      if ( c == '\n' ) {
        i++ ;             // go one char ahead to point on the first char
        break ; // exit because we found a \n
      }
      if ( c != '\r' ) nCharOnTft++ ; // count the number of char on Tft (skip \r)
      } while (  i > 0 ) ;
    nCharInFile = beginSearchAt - i ;
    return nCharOnTft ;
}
    
#define NCHAR_PER_LINE 23  
uint8_t convertNCharToNLines ( int16_t nChar ) { // return the number of lines needed on Tft to display one line in the show buffer
  if ( nChar < 0) return 0 ;
  return (nChar / NCHAR_PER_LINE ) + 1 ;
}

void drawDataOnSdShowPage() { // this function assume that sdShowBuffer contains the data to be displayed
                              // and that sdShowFirst, sdShowMax, sdShowBeginPos , sdMillPos have been filled
                              // after drawing, it fill the tabel to make PREV and NEXT visible or not
                              // it also save the position in file of the first char not being displayed (so for a Next we can save from there in the buffer)
  tft.setFreeFont (LABELS9_FONT) ;
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
  tft.setTextDatum( TL_DATUM ) ; 
  tft.setTextPadding (0) ;
  
  uint16_t line = 20 ;
  uint8_t col = 0 ;
  int16_t sdShowCurrent  = sdShowFirst ;   // next char to display (during the loop)
  int16_t nChar ; // number of char in buffer for the current line
  uint8_t nLines ; // number of lines on TFT needed to display the current line from buffer
  int16_t nCharInFile ; //number of char on one line in the SD file
  if ( sdShowFirst >= sdShowMax ) return ; //we are at the end of file, so nothing to display 
  do {
    //if ( sdShowCurrent >= (sdShowFirst + sdShowMax) ) break ;   // exit if we reach the end of the buffer
    //Serial.print(">> do : sdShowCurrent = ") ; Serial.println(sdShowCurrent); // to debug
    nChar = bufferLineLength( sdShowCurrent , nCharInFile ) ; // détermine le nbr de char de la ligne à afficher (sans le \n)
    if (nChar < 0 ) break;                      // exit if we reach the end of the buffer
    nLines = convertNCharToNLines ( nChar ) ;   // calcule le nbr de lignes du TFT pour afficher une ligne du buffer
    if ( ( line + (nLines * 20)) > 240 ) break ; // exit if we reach the end of the tft or if there are no enough free tft lines for next SD line
    // when it is possible to print the full line on TFT, do it (even on several tft lines
    tft.setCursor( 0 , line ) ;
    if ( (sdShowBeginPos  + sdShowCurrent) >= sdMillPos) tft.setTextColor(SCREEN_TO_SEND_TEXT ,  SCREEN_BACKGROUND ) ;
    tft.print(">") ; // display > on each new line
    //Serial.println( "put >" ); // to debug
    tft.setCursor( 15 , line ) ;
    uint16_t i = 0 ;
    uint8_t nCharOnTftLine = 0 ;
    while ( i <= nChar ) {                     // display each tft line
       if ( nCharOnTftLine < NCHAR_PER_LINE  ) {
        tft.print(sdShowBuffer[sdShowCurrent + i]) ;
        //Serial.print( sdShowBuffer[sdShowCurrent + i] ); // to debug
        nCharOnTftLine++ ;
        i++;                                  // move to next char
       } else {
        tft.setCursor( 230 , line ) ;
        tft.print( "+" ) ; // put a + at the end of the line on tft when sd line continue
        line += 20 ;                          // move to next line
        nCharOnTftLine = 0 ;                       // reset counter of char per line
        tft.setCursor( 15 , line ) ;
       }
    } // end while
    sdShowCurrent += nCharInFile   ;            
    line += 20 ;
    //Serial.print("line = ") ; Serial.println(line); // to debug
       
  } while (true ) ;  // end do
  if ( (sdShowBeginPos + sdShowFirst) > 0 ) { // adjust the setup for Prev btn
    fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_PREV , _PG_PREV , _JUST_PRESSED , fSdShowPrev , 0) ;
  } else {
    fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_PREV , _NO_BUTTON , _NO_ACTION , fSdShowPrev , 0) ;
  }
  if ( (sdShowBeginPos + sdShowCurrent ) < sdFileSize ) { // adjust the setup for Next btn
    fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_NEXT , _PG_NEXT , _JUST_PRESSED , fSdShowNext , 0) ;
  } else {
    fillMPage (_P_SD_SHOW , POS_OF_SD_SHOW_PG_NEXT , _NO_BUTTON , _NO_ACTION  , fSdShowNext , 0) ;
  }
  //Serial.print("sdShowBeginPos + sdShowCurrent=") ; Serial.println(sdShowBeginPos + sdShowCurrent); // to debug
  //Serial.print("sdFileSize=") ; Serial.println(sdFileSize); // to debug
  sdShowNextPos = sdShowBeginPos + sdShowCurrent ;              // adjust position in SD file of next char to be displayed if we press Next
  //Serial.print("sdShowNextPos = ") ; Serial.println(sdShowNextPos); // to debug
}

void drawDataOnOverwritePage() {                                // to do : text has still to be coded here
  tft.setFreeFont (LABELS12_FONT) ;
  tft.setTextSize(1) ;           // char is 2 X magnified => 
  tft.setTextColor( SCREEN_HEADER_TEXT ,  SCREEN_BACKGROUND ) ; // when only 1 parameter, background = fond);
  tft.setTextDatum( TL_DATUM ) ; // align left 
  tft.setTextPadding (239) ;      // expect to clear 230 pixel when drawing text or 
  uint8_t line = 15 ;
  uint8_t col = 2 ;
  if ( mPages[_P_OVERWRITE].boutons[POS_OF_OVERWRITE_OVERWRITE] == _OVER_SWITCH_TO_SPINDLE ) {
    tft.drawString( __CHANGING_FEEDRATE1 , col  , line );
    tft.setTextDatum( TR_DATUM ) ; // align rigth 
    tft.drawString( __CHANGING_FEEDRATE2 , col  + 230 , line + 30 );  
  } else {
    tft.drawString( __CHANGING_SPINDLE1 , col  , line );
    tft.setTextDatum( TR_DATUM ) ; // align rigth 
    tft.drawString( __CHANGING_SPINDLE2 , col + 230 , line + 30 );
  }

  tft.setTextFont( 2 );
  tft.setTextSize(1) ;
  tft.setTextPadding (0) ;
  tft.setTextDatum( TL_DATUM ) ;
  tft.setTextColor(SCREEN_HEADER_TEXT ,  SCREEN_BACKGROUND) ; 
  line = 180 ;
  tft.drawString(__FEED , 5 , line )  ;
  tft.drawString(__RPM , 5 , line + 20 ) ;
  tft.setTextColor(SCREEN_NORMAL_TEXT ,  SCREEN_BACKGROUND) ;
  tft.setTextPadding (70) ;
  tft.setTextDatum( TR_DATUM ) ;
  tft.drawNumber( (long) feedSpindle[0] , 110 , line) ; 
  tft.drawNumber( (long) feedSpindle[1] , 110 , line + 20) ; 
  tft.setTextPadding (30) ;
  tft.drawNumber( (long) overwritePercent[0] , 140 , line) ;
  tft.drawNumber( (long) overwritePercent[2] , 140 , line + 20) ;
  tft.setTextPadding (0) ;
  tft.drawString(  "%" , 155 , line )  ;
  tft.drawString(  "%" , 155 , line + 20 )  ;
}


// ******************************** touch calibrate ********************************************
#define DEBUG_CALIBRATION
void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  bool repeatCal = REPEAT_CAL;  // parameter in the config.h file (true when calibration is requested)
  if (checkCalibrateOnSD()) {
    repeatCal = true ; // force a recalibration if a file "calibrate.txt" exist on SD card
  }
  // check file system exists
  if (!SPIFFS.begin()) {
    #ifdef DEBUG_CALIBRATION
    Serial.println("Formating file system");
    #endif
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (repeatCal)
    {
      // Delete if we want to re-calibrate
      #ifdef DEBUG_CALIBRATION
      Serial.println("Remove file system");
      #endif
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    { 
      fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        #ifdef DEBUG_CALIBRATION
        Serial.println("File system opened for reading");
        #endif
        if (f.readBytes((char *)calData, 14) == 14)
          #ifdef DEBUG_CALIBRATION
          Serial.println("Cal Data has 14 bytes");
          #endif
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !repeatCal) {
    // calibration data valid
    #ifdef DEBUG_CALIBRATION
    Serial.println("Use calData");
    #endif
    tft.setTouch(calData);
    tft.printCalibration();
  } else {
    // data not valid so recalibrate
    #ifdef DEBUG_CALIBRATION
    Serial.println("Perform calibration");
    #endif
    clearScreen();
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(SCREEN_HEADER_TEXT ,  SCREEN_BACKGROUND);
    tft.println(__TOUCH_CORNER );
    tft.setTextFont(1);
    tft.println();

    if (repeatCal) {
      tft.setTextColor(SCREEN_ALERT_TEXT ,  SCREEN_BACKGROUND);
      tft.println(__SET_REPEAT_CAL );
    }

    tft.calibrateTouch(calData, SCREEN_ALERT_TEXT, SCREEN_BACKGROUND , 15);  
    //tft.getTouchCalibration(calData , 15) ; // added by ms for touch_ms_V1 instead of previous
    //tft.setTouch(calData);  // added by ms ; not needed with original because data are already loaded with original version
    tft.setTextColor(SCREEN_NORMAL_TEXT , SCREEN_BACKGROUND );
    tft.println(__CAL_COMPLETED );

    // store data
    fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      #ifdef DEBUG_CALIBRATION
      Serial.println("Write calibration in file system");
      #endif
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

boolean checkCalibrateOnSD(void){
      // return true if a file calibrate.txt exist on the SD card; if so it means that new calibration is requested
      SdBaseFile calibrateFile ;  
      if ( ! sd.begin(SD_CHIPSELECT_PIN , SD_SCK_MHZ(5)) ) {  
          Serial.println( __CARD_MOUNT_FAILED  ) ;
          return false;       
      }
      //if ( ! SD.exists( "/" ) ) { // check if root exist
      if ( ! sd.exists( "/" ) ) { // check if root exist   
          Serial.println( __ROOT_NOT_FOUND  ) ;
          return false;  
      }
      if ( ! sd.chdir( "/" ) ) {
          Serial.println( __CHDIR_ERROR  ) ;
          return false;  
      }
      if ( ! calibrateFile.open("/calibrate.txt" ) ) { // try to open calibrate.txt 
          Serial.println("failed to open calibrate.txt" ) ;
          return false;  
      }
      Serial.println("calibrate.txt exist on SD" ) ;      
      calibrateFile.close() ;
      return true ;
}




void fillMsg( char * msg, uint16_t color) {
  memccpy ( lastMsg , msg , '\0' , 79);
  lastMsgColor = color ;
  lastMsgChanged = true ;
}

