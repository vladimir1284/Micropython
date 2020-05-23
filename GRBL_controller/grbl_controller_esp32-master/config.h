/*      PINOUT

 === ESP32 pins used for the parallel interface TFT ===
 Defined in User_Setups/Setup14_ILI9341_Parallel.h
 LCD_CS   33  // Chip select control pin
 LCD_RS   25  // Data Command control pin - use a pin in the range 0-31
 LCD_RST  -1  // Reset pin

 LCD_WR    4  // Write strobe control pin - use a pin in the range 0-31
 LCD_RD    2

 LCD_D0   21  // Must use pins in the range 0-31 for the data bus
 LCD_D1   26  // so a single register write sets/clears all bits
 LCD_D2   13
 LCD_D3   15
 LCD_D4   17
 LCD_D5   16
 LCD_D6   27
 LCD_D7   22

 === ESP32 pins used for the 4-wires TOUCHSCREEN ===
  Defined in config.h 
 LCD_RD must be set HIGH to make D0-D7 high-z before reading position!!
 YP 33 // must be an analog pin!
 XM 25 // can be a digital pin
 XA 36 // must be an analog pin! (Connected throught a resistor of 5K to XM)
 YM 26 // can be a digital pin
 XP 21 // can be a digital pin

 === ESP32 pins used for the SD card ===
 SD_SS  05 Defined in config.h 
 SD_DI  23
 SD_DO  19
 SD_SCK 18

 === ESP32 pins used for the Arduino UNO ===
  Defined in config.h 
 SERIAL2_RXPIN 14
 SERIAL2_TXPIN 12
*/

#ifndef _config_h
#define _config_h


#include "TouchScreen_ms/TouchScreen_ms.h"
// Pins used for the 4-wires TOUCHSCREEN
#define YP 33 // must be an analog pin!
#define XM 25 // can be a digital pin
#define XA 36 // must be an analog pin!
#define YM 26 // can be a digital pin
#define XP 21 // can be a digital pin

#include "TFT_eSPI_ms/TFT_eSPI.h"

#define ESP32_VERSION "v1.0.t"

// decide if you will use Wifi or not (and how)
#define WIFI ESP32_ACT_AS_AP               // select between NO_WIFI, ESP32_ACT_AS_STATION, ESP32_ACT_AS_AP 

// If wifi is used, set the parameter to identify the access point (= the router when ESP32_ACT_AS_STATION or the ESP32 when ESP32_ACT_AS_AP)
#define MY_SSID "CNC"       // replace by the name of your access point (when act as station) or the name you assign to your device (when act as acces point)

// for ESP32_ACT_AS_STATION , set the password to get access to your access point (router)
// for ESP_ACT_AS_AP, set the password you want to use to protect your ESP32 ( can be empty)
#define MY_PASSWORD "cncpass84" // replace by your password 
// select your language between EN, FR, DE
#define LANGUAGE EN

//#define AA_AXIS    // uncomment if you want that the firmware handles 4 axes instead of 3 ;(The GRBL STM32 firmware has to be compiled/flashed with the same option)
                    

// Here some pins (GPIO) being used
//************************************
// 

#define SD_CHIPSELECT_PIN 5  // pin for SD card selection // to change probably

// pins for Serial to GRBL (it uses Serial2 UART)
#define SERIAL2_RXPIN 14
#define SERIAL2_TXPIN 12

// This is the file name used to store the touch coordinate in the SPIFFS from ESP32 (in es32 flash memory)
// calibration data. Change the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData4"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// the max speed of jogging in X and Y when using the nunchuk or the Move commands on TFT can easily be modified here.
// it seems that 5000 (mm/min) is a good value for RS-CNC32
#define MAX_XY_SPEED_FOR_JOGGING 500 

// note: this project allows to define up to 11 GRBL set of commands (makros) that can be called from setup screen.
// Those are defined by the user on a sd card and loaded on request into the ESP32 flash memory system (SPIFFS)
// So, once uploaded, they can be called without SD card.
// To be recognise as command, file name must be like Cmd3_xxxxxxxx.yyyy where
//       Cmd _ is fixed text (take care to the case)
//       3 is a digit from 1 up to 9 or the letter "A" or "B"; it defines the position of the button
//       xxxxxxxx will be the name of the button; first char must be a letter; min 1, max 16 characters, no space
//       yyy is the file name extension; it will be discarded
// The files to be used for upload must be placed in the root directory of SD card.
// To upload one of them, use the "Mill" + "SD->GRBL" menus and then select the file.
// After reset, there will be a button named xxxxxxxxx in the "Setup" + "CMD" menu
// If you upload a file having the same button position(digit 1...9, A or B) as an existing button, the new file will replace the button name and content of the previous button. 
// to delete a button create and execute a file having a name like Cmd3_delete where 3 is the button position (digit 1...9, A or B) to delete. 

// select color between (or define your own) 
// TFT_BLACK       0x0000      /*   0,   0,   0 */
// TFT_NAVY        0x000F      /*   0,   0, 128 */
// TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
// TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
// TFT_MAROON      0x7800      /* 128,   0,   0 */
// TFT_PURPLE      0x780F      /* 128,   0, 128 */
// TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
// TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
// TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
// TFT_BLUE        0x001F      /*   0,   0, 255 */
// TFT_GREEN       0x07E0      /*   0, 255,   0 */
// TFT_CYAN        0x07FF      /*   0, 255, 255 */
// TFT_RED         0xF800      /* 255,   0,   0 */
// TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
// TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
// TFT_WHITE       0xFFFF      /* 255, 255, 255 */
// TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
// TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
// TFT_PINK        0xFC9F 

//#define BUTTON_BORDER_NOT_PRESSED TFT_WHITE
//#define BUTTON_BORDER_PRESSED     TFT_RED
//#define BUTTON_BACKGROUND         TFT_BLUE
//#define BUTTON_TEXT               TFT_WHITE
//#define SCREEN_BACKGROUND         TFT_BLACK
//#define SCREEN_NORMAL_TEXT        TFT_GREEN
//#define SCREEN_ALERT_TEXT         TFT_RED
//#define SCREEN_HEADER_TEXT        TFT_WHITE

#define BUTTON_BORDER_NOT_PRESSED TFT_BLACK
#define BUTTON_BORDER_PRESSED TFT_RED
#define BUTTON_BACKGROUND TFT_GREEN
#define BUTTON_TEXT TFT_BLACK
#define SCREEN_BACKGROUND TFT_DARKGREY
#define SCREEN_NORMAL_TEXT TFT_GREEN
#define SCREEN_ALERT_TEXT TFT_RED
#define SCREEN_HEADER_TEXT TFT_WHITE
#define SCREEN_TO_SEND_TEXT TFT_WHITE   // used when looking at Sd file content for lines still to be sent to GRBL

// ************************************ Commands for change tools
// This suppose that we can use G28 and G30 and that milling use G54 WCS.
// The GRBL commands used by those buttons are defined here
// _SET_CHANGE_STRING : Set the current position as the position where the tool must be changed; 
//                      This is saved by GRBL so it remains after power off; G28 can be use to move directly to this position (! if homing has been done)
// _SET_PROBE_STRING : Set the current position as the position where probing must be performed; 
//                      This is saved by GRBL so it remains after power off; G30 can be use to move directly to this position (! if homing has been done)
//                      The idea is to put a micro switch at this position (so there is no need to use a clip)
// _CAL_STRING : = calibration ; to be execute only once for a given work piece (WCS); Set XYZ has to be set before running this command
//               Program will perform a Z probe in order to know the Z offset for current WCS; This offset is then saved. 
//               This offset will be saved in flash memory; so it will still be known after a reset.
// _GO_CHANGE_STRING : Go to the change tool position in order to change tool.
// _GO_PROBE_STRING : Perform a probe with new tool and then change offset based on the offset saved during calibration;
//
// To perform those task we define set of GRBL commands with some extensions:
// $G asks GRBL for the modal parameters (G21 = metric,G90 = absolute,...); ESP32 program save them in order to restore them later on
//        Values are displayed on Log screen
// $# ask GRBL for all offsets (G54,...G28, G92, ...); Values are displayed on Log screen
// %M restore 2 modal parameters (G20/G21 and G90/G91)
// %z ask the firmware to save the current Z Mpos;
// %Z replace %Z by the saved value %z; this occurs when the string is sent to GRBL.
// %X replace %X by the saved value of G30 X offset
// %Y replace %Y by the saved value of G30 Y offset
//                  wait that all commands are execute (G4P0.0\n), then ask for offset and modal parameters that are automatically saved( $#$G\n ),
//                  then stop spindle ( M5\n ), then move Z up for safety ( G53G21G90G00Z-2\n ) ,
//                  then go to Probe position ( G30 keeping Z-2\n ), then probe at high speed ( G38.2Z-100F50\n ),
//                  then change modal to relative and mm ( G21G91\n ), then move Z up 2mm ( G0Z2\n ) ,
//                  then probe again at low speed ( G38.2Z-3F10\n ) , then wait that command is executed ( G4P0.5\n ) ,
//                  then save the Z WCS position ( %z )in ESP32, then rise Z up ( G53G21G90G0Z-2\n ) ,
//                  then goto predefined change tool position ( G28\n ) and restore 2 modal parameters ( M\n )
#define _CAL_STRING "G4P0.0\n $G\n M5\n G53 G21 G90 G00 Z-2\n G30\n G21 G91\n G38.2 Z-70 F100\n G00 Z2\n G38.2 Z-3 F10\n G4P0.5\n %z G53 G21 G90 G00 Z-2\n G28\n %M\n"
  
#define _GO_CHANGE_STRING "G4P0.0\n $G\n $#\n M5\n G53 G21 G90 G00 Z-2\n G28\n"
#define _GO_PROBE_STRING "G4P0.0\n $#\n $G\n M5\n G4P0.5\n G53 G21 G90 G00 Z-2\n G53 G21 G90 G00 X%X Y%Y\n G30\n G21 G91\n G38.2 Z-70 F100\n G00 Z2\n G38.2 Z-3 F10\n G10 L20 P1 Z%Z\n G53 G21 G90 G00 Z-2\n %M\n"
#define _SET_CHANGE_STRING "G28.1\n G4P0.0\n $#\n $G\n" 
#define _SET_PROBE_STRING "G30.1\n G4P0.0\n $#\n $G\n" 

// here a few other GCODE being used; it set offset for W54 (=P1)
#define _SETX_STRING "G10 L20 P1 X0\n"
#define _SETY_STRING "G10 L20 P1 Y0\n"
#define _SETZ_STRING "G10 L20 P1 Z0\n"
#define _SETA_STRING "G10 L20 P1 Z0\n"
#define _SETXYZ_STRING "G10 L20 P1 X0 Y0 Z0\n"
#define _SETXYZA_STRING "G10 L20 P1 X0 Y0 Z0 A0\n"
// *************************************     normally do not change here below ****************************
//       debugging
//#define DEBUG_TO_PC
#ifdef DEBUG_TO_PC
  #define COPY_GRBL_TO_PC
  #define DEBUG_TO_PC_MENU
#endif


#define DIR_LEVEL_MAX 6

#define MAX_FILES 4

#define N_LOG_LINE_MAX 24 // was 12 when we used a bigger font

#define NO_WIFI 0               // code used to identify the wifi mode
#define ESP32_ACT_AS_STATION 1
#define ESP32_ACT_AS_AP 2

//        commands available in menu ; this part is normally not used anymore because cmd are defined in SPIFFS
//#define CMD1_GRBL_CODE "*X"
//#define CMD2_GRBL_CODE "G01 X02"
//#define CMD3_GRBL_CODE "G01 X03"
//#define CMD4_GRBL_CODE "G01 X04"
//#define CMD5_GRBL_CODE "G01 X05"
//#define CMD6_GRBL_CODE "G01 X06"
//#define CMD7_GRBL_CODE "G01 X07"

//#define CMD1_NAME " Unlock GRBL"
//#define CMD2_NAME "G01 X02"
//#define CMD3_NAME "G01 X03"
//#define CMD4_NAME "G01 X04"
//#define CMD5_NAME "G01 X05"
//#define CMD6_NAME "G01 X06"
//#define CMD7_NAME "G01 X07"


#endif


