# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement
import time
import uasyncio as asyncio

try:
    from typing import Optional
    from DigitalInput import DigitalInput
    from DigitalOutputs import Dout
    from LDR import LDR
except ImportError:
    pass


class Light:
    """
    Automatic light controller.
    """
    def __init__(
        self,
        pir: DigitalInput,
        lamp: Dout,
        delay = 2,
        ldr: Optional[LDR] = None,
        threshold = 70
        ):
        """
        pir:Motion sensor
        lamp: Digital output driving the lamp
        delay: Time in minutes to hold the lamp on after movement
        ldr: Luminosity sensor for activating the automatic light only at night
        threshold: Luminosity must be above this value to activate the automatic lamp
        """  

        self._fsmFunction = {
            'IDLE': self._idle,
            'ON': self._on
        }

        self._setState('IDLE')

        self._pir = pir
        self._lamp = lamp
        self._setDelay(delay)
        self._ldr = ldr
        self._threshold = threshold

        # Start the main task
        self.task = asyncio.create_task(self._run())

    # ----------------------------------------------------------   
    def _setDelay(self, delay):
        """
        Set the delay time in seconds from the argument in minutes
        delay: Motion sensor delay in minutes
        """
        self._delay = 60*delay

    # ----------------------------------------------------------   
    async def _run(self):
        while True:
            self._fsmFunction[self._state]()
            await asyncio.sleep_ms(100) # Cycle every 100ms

    # ----------------------------------------------------------   
    def _idle(self):
        if self._pir.isActive():
            if self._ldr is None: 
                self._lamp.turnOn()
                self._setState('ON')
            else:
                if self._ldr.getLuminosity() < self._threshold:
                    self._lamp.turnOn()
                    self._setState('ON')


    # ----------------------------------------------------------   
    def _on(self):
        if self._pir.isActive():
           self._stateChange = time.time() # Restart time count
        else:
            if time.time() - self._delay > self._stateChange:
                self._lamp.turnOff()
                self._setState('IDLE')

    # ----------------------------------------------------------
    def _setState(self, state):
        self._state = state
        self._stateChange = time.time()

    # ----------------------------------------------------------
    def getStatus(self):
        status = {
            'state': self._state,
            'pir': self._pir.isActive(),
            'delay': self._delay - time.time()  + self._stateChange
        }
        if self._ldr is not None:
            status.setdefault('ldr', self._ldr.getLuminosity())
        return status
