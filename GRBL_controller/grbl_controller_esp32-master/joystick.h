
#ifndef JOYSTICK_H
#define JOYSTICK_H

void joystick_init() ;

uint8_t buttonZ() ;

uint8_t buttonC() ;

void handleJoystick (void) ;

// list of status used also when sending
enum { JOG_NO = 0 , JOG_WAIT_END_CANCEL , JOG_WAIT_END_CMD } ;

#endif

