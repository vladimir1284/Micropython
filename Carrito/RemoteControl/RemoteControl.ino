/* BT controlled remote car
    Using Car Bluetooth apk 
*/

#include "BluetoothSerial.h"
#include "configs.h"
#include "motor_control.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char lastCommand;
unsigned long lastRecieved;

int speed;

bool configuring;
String config_str;
Configurator cfgs = Configurator();

Motor motorR = Motor(pinENA, pinIN1, pinIN2, LEDC_CHANNEL_0);
Motor motorL = Motor(pinENB, pinIN3, pinIN4, LEDC_CHANNEL_1);

void setup()
{
  if (DEBUG)
  {
    Serial.begin(115200);
    Serial.println("The device started, now you can pair it with bluetooth!");
  }
  SerialBT.begin("ESP32test"); //Bluetooth device name

  lastCommand = ' ';

  configuring = false;

  cfgs.init();
  float factor = cfgs.getLfactor();
  Serial.print("Factor L: ");
  Serial.println(factor);
  motorL.init(1);

  factor = cfgs.getRfactor();
  Serial.print("Factor R: ");
  Serial.println(factor);
  motorR.init(1);

  speed = 2;
  lastRecieved = millis();
}

void loop()
{
  if (SerialBT.available())
  {
    lastRecieved = millis();
    char cmd = SerialBT.read();
    if (DEBUG)
    {
      Serial.write(cmd);
    }
    if (configuring)
    {
      switch (cmd)
      {
      case '#': // Stop
        cfgs.setParamater(config_str);
        load_configs();
        configuring = false;
        break;

      default:
        config_str += cmd;
        break;
      }
    }
    else
    {
      switch (cmd)
      {
      case 'S': // Stop
        motorL.fullStop();
        motorR.fullStop();
        break;

      case 'F': // Forward
        motorL.moveForward(speed);
        motorR.moveForward(speed);
        break;

      case 'B': // Backward
        motorL.moveBackward(speed);
        motorR.moveBackward(speed);
        break;

      case 'R': // Rigth
        motorL.moveForward(8);
        motorR.moveBackward(8);
        break;

      case 'L': // left
        motorR.moveForward(8);
        motorL.moveBackward(8);
        break;

      case 'I': // Forward right
        //motorR.moveForward(speed / TURN_FACTOR);
        motorR.fullStop();
        motorL.moveForward(2 * speed);
        break;

      case 'G': // Forward left
        //motorL.moveForward(speed / TURN_FACTOR);
        motorL.fullStop();
        motorR.moveForward(2 * speed);
        break;

      case 'J': // Backward right
        //motorR.moveBackward(speed / TURN_FACTOR);
        motorR.fullStop();
        motorL.moveBackward(2 * speed);
        break;

      case 'H': // Backward left
        //motorL.moveBackward(speed / TURN_FACTOR);
        motorL.fullStop();
        motorR.moveBackward(2 * speed);
        break;

      case '#': // Start Configuration
        configuring = true;
        config_str = "";
        break;

      case '0': // set speed
      case '1': // set speed
      case '2': // set speed
      case '3': // set speed
      case '4': // set speed
      case '5': // set speed
      case '6': // set speed
      case '7': // set speed
      case '8': // set speed
      case '9': // set speed
        speed = String(cmd).toInt();
        break;

      default:
        break;
      }
    }
  }
  else
  {
    if (millis() - lastRecieved > CMD_DELAY)
    {
      // Stop motor if no cmd
      motorL.fullStop();
      motorR.fullStop();
    }
  }

  motorL.run();
  motorR.run();
}

void load_configs()
{
  motorR.setSpeedFactor(cfgs.getRfactor());
  motorL.setSpeedFactor(cfgs.getLfactor());
  if (SerialBT.connected())
  {
    SerialBT.print("Initial speed: ");
    SerialBT.println(cfgs.getInitalSpeed());
    SerialBT.print("Speed factor for the right motor: ");
    SerialBT.println(cfgs.getRfactor());
    SerialBT.print("Speed factor for the left motor: ");
    SerialBT.println(cfgs.getLfactor());
  }
  if (DEBUG)
  {
    SerialBT.print("Initial speed: ");
    SerialBT.println(cfgs.getInitalSpeed());
    Serial.print("Speed factor for the right motor: ");
    Serial.println(cfgs.getRfactor());
    Serial.print("Speed factor for the left motor: ");
    Serial.println(cfgs.getLfactor());
  }
}