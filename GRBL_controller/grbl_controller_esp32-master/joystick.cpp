
#include "config.h"
#include "language.h"
#include "com.h"
#include "joystick.h"
#include "draw.h"


// Calibration joystick values
#define JOYSTICK_X_NEG 350
#define JOYSTICK_X_POS 600
#define JOYSTICK_Y_NEG 350
#define JOYSTICK_Y_POS 600

uint16_t xval, yval, val;
int8_t prevMoveX = 0;
int8_t prevMoveY = 0;
int8_t prevMoveZ = 0;
int8_t prevMoveA = 0;
int8_t jogDistX = 0;
int8_t jogDistY = 0;
int8_t jogDistZ = 0;
int8_t jogDistA = 0;
float moveMultiplier;
uint32_t cntSameMove = 0;
uint32_t startMoveMillis = 0;

uint8_t jog_status = JOG_NO;
boolean jogCancelFlag = false;
boolean jogCmdFlag = false;

extern char machineStatus[9];
//extern char lastMsg[80] ;

/**
 * Initializes the analog inputs (10bits used in this program)
 */
void joystick_init()
{
    pinMode(JOYSTICK_X, ANALOG);
    pinMode(JOYSTICK_Y, ANALOG);
    pinMode(JOYSTICK_BTNS, ANALOG);
}

#define ZBTN_THRESS 100
#define CBTN_THRESS 800

uint8_t buttonZ()
{ // Checks the current state of button Z
    val = analogRead(JOYSTICK_BTNS);
    // #ifdef DEBUG_JOYSTICK
    //     Serial.print("Btn analog value = ");
    //     Serial.println(val);
    // #endif // DEBUG
    return (val < ZBTN_THRESS);
}

uint8_t buttonC()
{ // Checks the current state of button C
    val = analogRead(JOYSTICK_BTNS);
    return (val > ZBTN_THRESS && val < CBTN_THRESS);
}

#define JOYSTICK_READ_DELAY 100 // read joystick every 50 msec (about)
// (if statusPrinting = PRINTING_STOPPED  and) if machineStatus= Idle or Jog, then if delay since previous read exceed 100msec, then read nunchuk data
//  and if buttonZ or buttonC is released while it was pressed bebore, then send a command to cancel JOG (= char 0x85 )
//  If button buttonZ or buttonC is pressed and if joysttick is moved, then send a jog command accordingly
//  sending grbl jogging commands is done in com.cpp because it has to wait for "ok" after sending a command.
// There are 2 type of grbl commands  : one for canceling and some for jogging

void handleJoystick(void)
{
    uint32_t joystickMillis = millis();
    static uint32_t lastjoystickMillis = 0;
    int8_t moveX = 0; //static int8 prevMoveX = 0;
    int8_t moveY = 0; //static int8 prevMoveY = 0;
    int8_t moveZ = 0; //static int8 prevMoveZ = 0;
    int8_t moveA = 0; //static int8 prevMoveZ = 0;
                      //float moveMultiplier ;
                      //Serial.print("handle=");Serial.println( machineStatus[0] , HEX);
    if ((joystickMillis - lastjoystickMillis) > JOYSTICK_READ_DELAY)
    {
        lastjoystickMillis = joystickMillis;
        xval = analogRead(JOYSTICK_X);
        yval = analogRead(JOYSTICK_Y);

        //Serial.print(xval,HEX); Serial.print(",");Serial.print(yval,HEX); Serial.print(",");Serial.println(nunchuk_data[5] & 0x03 , HEX);
        if (buttonC() && buttonZ() == 0)
        {   // si le bouton C est enfoncé mais pas le bouton Z
            //Serial.print( ( int )xval ) ; Serial.print(" ") ; Serial.println( ( int )yval ) ;
#ifdef DEBUG_JOYSTICK
            Serial.print("Xc analog value = ");
            Serial.println(xval);
            Serial.print("Yc analog value = ");
            Serial.println(yval);
#endif // DEBUG
            if (xval < JOYSTICK_X_NEG)
            {
                moveX = -1;
            }
            else if (xval > JOYSTICK_X_POS)
            {
                moveX = 1;
            }
            if (yval < JOYSTICK_Y_NEG)
            {
                moveY = -1;
            }
            else if (yval > JOYSTICK_Y_POS)
            {
                moveY = 1;
            }
        }
        else if (buttonZ() && buttonC() == 0)
        { // si le bouton Z est enfoncé mais pas le bouton C
#ifdef DEBUG_JOYSTICK
            Serial.print("Xz analog value = ");
            Serial.println(xval);
            Serial.print("Yz analog value = ");
            Serial.println(yval);
#endif // DEBUG
#ifdef AA_AXIS
            if (xval < JOYSTICK_X_NEG)
            {
                moveA = -1;
            }
            else if (xval > JOYSTICK_X_POS)
            {
                moveA = 1;
            }
#endif
            if (yval < JOYSTICK_Y_NEG)
            {
                moveZ = -1;
            }
            else if (yval > JOYSTICK_Y_POS)
            {
                moveZ = 1;
            }
        }
        //if ( (machineStatus[0] == 'J' ) && ( ( prevMoveX != moveX) || ( prevMoveY != moveY)  || ( prevMoveZ != moveZ) ) ) { // cancel Jog if jogging and t least one direction change
        if ((machineStatus[0] == 'J' || machineStatus[0] == 'I') && ((prevMoveX != moveX) || (prevMoveY != moveY) || (prevMoveZ != moveZ)) || (prevMoveA != moveA))
        { // cancel Jog if jogging and at least one direction change
            jogCancelFlag = true;
            cntSameMove = 0; // reset the counter
                             //Serial.println("cancel jog") ;
        }
        else
        {
            //jogCancelFlag = false ;
        }
        if (moveX || moveY || moveZ || moveA)
        { // if at least one move is asked
            if (cntSameMove == 0)
            {
                //  moveMultiplier = 0.01 ;
                startMoveMillis = millis();
            }
            cntSameMove++;
            jogCmdFlag = true;
            jogDistX = moveX;
            jogDistY = moveY;
            jogDistZ = moveZ;
            jogDistA = moveA;
#ifdef DEBUG_JOYSTICK
            Serial.print("cnt= ");
            Serial.println(cntSameMove);
            Serial.print("moveX= ");
            Serial.println(moveX);
            Serial.print("moveY= ");
            Serial.println(moveY);
            Serial.print("moveZ= ");
            Serial.println(moveZ);
#endif // DEBUG
        }
        else
        { // no move asked ( moveX || moveY || moveZ || moveA)
            cntSameMove = 0;
            //  moveMultiplier = 0 ; // put the value on 0 to avoid an old move to be execute  ; let the flag to be reset by the com.cpp file after a OK being received
            //jogCmdFlag = false ;
        } // end if ( moveX || moveY || moveZ)
        prevMoveX = moveX;
        prevMoveY = moveY;
        prevMoveZ = moveZ;
        prevMoveA = moveA;
    } //end of test on delay
} // end handleJoysitck
