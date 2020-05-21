#ifndef configs_h
#define configs_h

#include <Arduino.h>
#include "EEPROM.h"

#define DEBUG  true

#define CMD_DELAY  1000 // ms to stop motors if no cmd is received

// Motor L
#define pinENA  21 // D21
#define pinIN1  19 // D19
#define pinIN2  18 // D18
#define SPEED_FACTOR_L 1

// Motor R
#define pinIN3   5 // D5
#define pinIN4  17 // TX2
#define pinENB  16 // RX2
#define SPEED_FACTOR_R 1
 
#define waitTime  200   //espera entre fases
#define TURN_FACTOR 2

// ------------------- PWM ---------------------------
#define MAX_SPEED 5000   // PWM 2 ^ 13 - 1
#define MIN_SPEED 2000   
// use firsts channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0     0
#define LEDC_CHANNEL_1     1
// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13
// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// ------------ EEPROM -----------------
#define ADDRESS_INITAL_SPEED 0
#define ADDRESS_L_FACTOR     4
#define ADDRESS_R_FACTOR     8

class Configurator
{

public:
    Configurator();

    void init(),
        setParamater(String parameter_str);

    float getLfactor(),
          getRfactor();

    int getInitalSpeed();
};

#endif