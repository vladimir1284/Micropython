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

int speed;

bool configuring;
String config_str;
Configurator cfgs = Configurator();

Motor motorL = Motor(pinENA, pinIN1, pinIN2, LEDC_CHANNEL_0);
Motor motorR = Motor(pinENB, pinIN3, pinIN4, LEDC_CHANNEL_1);

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

  motorL.init(cfgs.getLfactor());
  motorR.init(cfgs.getRfactor());

  speed = 5;
}

void loop()
{
  if (SerialBT.available())
  {
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
        motorL.moveForward(speed);
        motorR.fullStop();
        break;

      case 'L': // left
        motorR.moveForward(speed);
        motorL.fullStop();
        break;

      case 'I': // Forward right
        motorR.moveForward(speed / TURN_FACTOR);
        motorL.moveForward(speed);
        break;

      case 'G': // Forward left
        motorL.moveForward(speed / TURN_FACTOR);
        motorR.moveForward(speed);
        break;

      case 'J': // Backward right
        motorR.moveBackward(speed / TURN_FACTOR);
        motorL.moveBackward(speed);
        break;

      case 'H': // Backward left
        motorL.moveBackward(speed / TURN_FACTOR);
        motorR.moveBackward(speed);
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