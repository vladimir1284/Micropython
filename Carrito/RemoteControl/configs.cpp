#include "configs.h"

Configurator::Configurator()
{
}

void Configurator::init()
{
    if (!EEPROM.begin(64))
    {
        if (DEBUG)
        {
            Serial.println("Failed to initialise EEPROM");
            Serial.println("Restarting...");
        }
        delay(1000);
        ESP.restart();
    }
}

void Configurator::setParamater(String parameter_str)
{
    char param = parameter_str.charAt(0);
    switch (param)
    {
    case 'L':
        EEPROM.writeFloat(ADDRESS_L_FACTOR,parameter_str.substring(1).toFloat());
        break;
    case 'R':
        EEPROM.writeFloat(ADDRESS_R_FACTOR,parameter_str.substring(1).toFloat());
        break;    
    case 'I':
        EEPROM.writeInt(ADDRESS_INITAL_SPEED,parameter_str.substring(1).toInt());
        break;
    default:
        if (DEBUG){
            Serial.print("Unknown parameter ");
            Serial.println(param);
        }
        break;
    }
}

float Configurator::getLfactor(){
    return EEPROM.readFloat(ADDRESS_L_FACTOR);
}
 
float Configurator::getRfactor(){
    return EEPROM.readFloat(ADDRESS_R_FACTOR);
}
 
int Configurator::getInitalSpeed(){
    return EEPROM.readInt(ADDRESS_INITAL_SPEED);
}