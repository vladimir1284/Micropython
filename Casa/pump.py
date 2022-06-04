# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init

import time
import uasyncio as asyncio
from micropython import const
from machine import Pin
from HCSR04 import HCSR04
from settings import Settings

try:
    from typing import Optional
    from ulogging import RootLogger
    from DigitalOutputs import Dout
except ImportError:
    pass

UPPER_FLOAT = const(22) # Pin number of the floating sensor in the upper tank
UPPER_ULTRA = const(19) # Pin number of the ultrasonic sensor in the upper tank

LOWER_FLOAT = const(23) # Pin number of the floating sensor in the lower tank
LOWER_ULTRA = const(18) # Pin number of the ultrasonic sensor in the lower tank

TRIGGER_PIN = const(21) # Pin number of the trigger for ultrasonic sensors

# #TODO instanciate HCSR04
# trg = Pin(TRIGGER_PIN, Pin.OUT)
# echo1 = Pin(UPPER_ULTRA, Pin.IN)
# echo2 = Pin(LOWER_ULTRA, Pin.IN)

class Pump:

    def __init__(
        self,
        pumpOut: Dout,
        loop,
        settings: Settings,
        debug: int = 0,
        log: Optional[RootLogger] = None
        ):

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("pump")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log        

        self._fsmFunction = {
            'IDLE': self._idle,
            'PUMPING': self._pumping,
            'WAIT': self._wait,
            'ERROR': self._error,
        }

        self._pump = pumpOut
        self._loop = loop
        self._cfg  = settings

        # High when tank is full
        self._lowerFloat = Pin(LOWER_FLOAT, Pin.IN, Pin.PULL_UP)
        self._upperFloat = Pin(UPPER_FLOAT, Pin.IN)

        # self._ultrasonic = HCSR04(TRIGGER_PIN, (LOWER_ULTRA,UPPER_ULTRA))
        self._lowerUltra = HCSR04(TRIGGER_PIN, LOWER_ULTRA, 5000)
        self._upperUltra = HCSR04(TRIGGER_PIN, UPPER_ULTRA, 9000)

        self._state = 'IDLE'
        self._stateChange = time.time()

    async def run(self):
        while True:
            self._fsmFunction[self._state]()
            await asyncio.sleep(1) # Cycle every 1s

    def _idle(self):
        if not self._upperFloat.value():
            if self._lowerFloat.value():
                self._pump.turnOn()
                self._setState('PUMPING')
            else:
                self._setState('WAIT')


    def _pumping(self):
        if not self._lowerFloat.value():
            self._pump.turnOff()
            self._setState('WAIT')
        else:
            if self._upperFloat.value():
                self._pump.turnOff()
                self._setState('IDLE')
            else:
                if time.time() - self._cfg.settings['PUMPING_MAX_TIME'] > self._stateChange:
                    self._pump.turnOff()
                    self._setState('ERROR')
                
    def _wait(self):
        if self._lowerFloat.value():
            self._pump.turnOn()
            self._setState('PUMPING')

    def _error(self):
        pass

    def _setState(self, state):
        self._state = state
        self._stateChange = time.time()
        self._log.debug("{}-> State: {}".format(time.time(), self._state))

    async def getStatus(self):
        tasks = (self._upperUltra.distance_cm(), self._lowerUltra.distance_cm())
        res = await asyncio.gather(*tasks)
        return {
            'state': self._state,
            'upperFloat': self._upperFloat.value(),
            'upperUltra': int(res[0]),
            'lowerFloat': self._lowerFloat.value(),
            'lowerUltra': int(res[1]),
            'pump': self._pump.value
        }

    def ackError(self):
        self._state = 'IDLE'