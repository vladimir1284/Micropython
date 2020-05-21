#include "motor_control.h"
#include <Arduino.h>

Motor::Motor(int pinEN, int pin1, int pin2, int ledc_chann)
{
    pinSpeed = pinEN;
    pinM1 = pin1;
    pinM2 = pin2;
    channel = ledc_chann;
}

void Motor::setSpeedFactor(float speedFactor)
{
    sf = speedFactor;
}

void Motor::run()
{
    if (delayNeeded)
    {
        if (millis() - lastStopped > waitTime)
        {
            delayNeeded = false;
        }
    }
}

void Motor::init(float speedFactor)
{
    pinMode(pinM1, OUTPUT);
    pinMode(pinM2, OUTPUT);
    pinMode(pinSpeed, OUTPUT);

    ledcAttachPin(pinSpeed, channel); // assign pin to channel
    // Configure channel
    ledcSetup(channel, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    delayNeeded = false;
    currentState = STOPPED;

    setSpeedFactor(speedFactor);
}

void Motor::moveForward(int speed)
{
    if ((currentState != BACKWARD) && !delayNeeded)
    {
        digitalWrite(pinM1, LOW);
        digitalWrite(pinM2, HIGH);

        ledcWrite(channel, computeSpeed(speed));
        currentState = FORWARD;
    }
    else
    {
        fullStop();
    }
}

void Motor::moveBackward(int speed)
{
    if ((currentState != FORWARD) && !delayNeeded)
    {
        digitalWrite(pinM1, HIGH);
        digitalWrite(pinM2, LOW);

        ledcWrite(channel, computeSpeed(speed));
        currentState = BACKWARD;
    }
    else
    {
        fullStop();
    }
}

void Motor::fullStop()
{
    if (currentState != STOPPED)
    {
        digitalWrite(pinM1, LOW);
        digitalWrite(pinM2, LOW);

        ledcWrite(channel, 0);
        currentState = STOPPED;
        lastStopped = millis();
        delayNeeded = true;
    }
}

int Motor::computeSpeed(int speed)
{
    int output = round(speed * ((MAX_SPEED - MIN_SPEED) * sf) / 9 + MIN_SPEED);
    if (DEBUG)
    {
        Serial.print(output);
    }
    return output;
}