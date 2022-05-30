from micropython import const
from shifter595 import DigitalOutputs595

DATA_PIN = const(0)
SAVE_PIN = const(4)
CLK_PIN  = const(16)

NOUTs = const(8)

dOuts595 = DigitalOutputs595(DATA_PIN,SAVE_PIN,CLK_PIN,NOUTs)

class Dout:
    def __init__(
        self, 
        description: str,
        address: int,
        highLevel: bool,
        dOut595: DigitalOutputs595):
        
        self._description = description
        self._address = address
        self._highLevel = highLevel
        self._dOut595 = dOut595

        self.value = 0
        self.turnOff()


    def __str__(self):
        return self._description

    def turnOn(self):
        self.value = 1
        self._dOut595.digitalWrite(self._address, self._highLevel)

    def turnOff(self):
        self.value = 0
        self._dOut595.digitalWrite(self._address, not(self._highLevel))

# Init digital Outputs
buzzer = Dout(
    "Buzzer",
    0,
    1,
    dOuts595
    )

ssr10A = Dout(
    "SSR 10A two pins available",
    1,
    1,
    dOuts595
    )

bomba = Dout(
    "SSR 10A AC line on/off (only 1 wire)",
    2,
    1,
    dOuts595
    )

lamp_hall = Dout(
    "SSR 2A AC line on/off (only 1 wire)",
    3,
    1,
    dOuts595
    )

ssr2A = Dout(
    "SSR 2A AC line on/off (only 1 wire)",
    4,
    1,
    dOuts595
    )

siren = Dout(
    "10A relay",
    5,
    0,
    dOuts595
    )

relay10A = Dout(
    "10A relay",
    6,
    0,
    dOuts595
    )

