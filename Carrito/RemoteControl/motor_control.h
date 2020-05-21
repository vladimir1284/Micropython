#include "configs.h"

enum MotorStates
{
    FORWARD,
    BACKWARD,
    STOPPED
};

class Motor
{

public:
    Motor(int pinEN, int pin1, int pin2, int ledc_chann);

    void run(), // Main method to be refreshed in every loop
        init(float speedFactor),
        moveBackward(int speed),
        fullStop(),
        setSpeedFactor(float speedFactor),
        moveForward(int speed);

private:
    int pinSpeed,
        pinM1,
        pinM2,
        channel;
    
    float sf;

    MotorStates currentState;
    bool delayNeeded;
    unsigned long lastStopped;

    int computeSpeed(int speed);
};