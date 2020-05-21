/*
  ESP32 eeprom_extra example with EEPROM library

  This simple example demonstrates using other EEPROM library resources

  Created for arduino-esp32 on 25 Dec, 2017
  by Elochukwu Ifediora (fedy0)
*/

#include "EEPROM.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\nTesting EEPROM Library\n");
  if (!EEPROM.begin(50)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  int address = 0;

  EEPROM.writeInt(address, -2147483648);            // -2^31
  int int_size = sizeof(int);
  address += int_size;
  Serial.print("Entero = ");
  Serial.print(int_size);
  Serial.println("bytes");

  // L motor factor
  EEPROM.writeFloat(address, 1.0);
  int float_size = sizeof(float);
  address += int_size;
  Serial.print("Float = ");
  Serial.print(float_size);
  Serial.println("bytes");
  
  // R motor factor
  EEPROM.writeFloat(address, 0.7);

  // See also the general purpose writeBytes() and readBytes() for BLOB in EEPROM library
  EEPROM.commit();
  address = 0;
  
  Serial.println(EEPROM.readInt(address));
  address += sizeof(int);

  Serial.println(EEPROM.readFloat(address), 4);
  address += sizeof(float);

  Serial.println(EEPROM.readFloat(address), 4);
}

void loop() {
  // put your main code here, to run repeatedly:

}
