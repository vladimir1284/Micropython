# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init

import time
import uasyncio as asyncio
from micropython import const
from machine import Pin
from HCSR04 import HCSR04
from DigitalInput import DigitalInput
from settings import Settings

try:
    from typing import Optional
    from ulogging import RootLogger
    from DigitalOutputs import Dout
    from PowerMeter import PowerMeter
except ImportError:
    pass

MINPOWER = const(20) # Power in watts to detect a blackout 

UPPER_FLOAT = const(22) # Pin number of the floating sensor in the upper tank
UPPER_ULTRA = const(19) # Pin number of the ultrasonic sensor in the upper tank

LOWER_FLOAT = const(23) # Pin number of the floating sensor in the lower tank
LOWER_ULTRA = const(18) # Pin number of the ultrasonic sensor in the lower tank

TRIGGER_PIN = const(21) # Pin number of the trigger for ultrasonic sensors


class Pump:

    def __init__(
        self,
        pumpOut: Dout,
        pwr: PowerMeter,
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
        self._pwr = pwr
        self._cfg  = settings

        # High when tank is full
        self._lowerFloat = DigitalInput(LOWER_FLOAT,
                                        active_high = False,
                                        interval = 10)
        self._upperFloat = DigitalInput(UPPER_FLOAT,
                                        active_high = False,
                                        interval = 10)

        # self._lowerUltra = HCSR04(TRIGGER_PIN, LOWER_ULTRA, 5000)
        # self._upperUltra = HCSR04(TRIGGER_PIN, UPPER_ULTRA, 9000)

        self._state = 'IDLE'
        self._stateChange = time.time()

        # Start the main task
        self.task = asyncio.create_task(self._run())

    async def _run(self):
        while True:
            self._fsmFunction[self._state]()
            await asyncio.sleep(1) # Cycle every 1s

    def _idle(self):
        if self._upperFloat.isActive():
            if not self._lowerFloat.isActive() and self._pwr.getPower() > MINPOWER:
                self._pump.turnOn()
                self._setState('PUMPING')
            else:
                self._setState('WAIT')


    def _pumping(self):
        if self._lowerFloat.isActive() or self._pwr.getPower() < MINPOWER:
            self._pump.turnOff()
            self._setState('WAIT')
        else:
            if not self._upperFloat.isActive():
                self._pump.turnOff()
                self._setState('IDLE')
            else:
                if time.time() - self._cfg.settings['PUMPING_MAX_TIME'] > self._stateChange:
                    self._pump.turnOff()
                    self._setState('ERROR')
                
    def _wait(self):
        if not self._lowerFloat.isActive() and self._pwr.getPower() > MINPOWER:
            self._pump.turnOn()
            self._setState('PUMPING')

    def _error(self):
        pass

    def _setState(self, state):
        self._state = state
        self._stateChange = time.time()
        self._log.debug("{}-> State: {}".format(time.time(), self._state))

    def getStatus(self):
        return {
            'state': self._state,
            'upperFloat': self._upperFloat.isActive(),
            # 'upperUltra': self._upperUltra.distance_cm(),
            'lowerFloat': self._lowerFloat.isActive(),
            # 'lowerUltra': self._lowerUltra.distance_cm(),
            'pump': self._pump.value
        }

    def ackError(self):
        self._state = 'IDLE'