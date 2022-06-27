# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import uasyncio as asyncio
from uasyncio import Event
from micropython import const
from machine import Pin, time_pulse_us


class FlowMeter:
    """
    Driver to count flowmeter pulses.
    """
    def __init__(
        self,
        pin,
        pulses_per_litre = 267,
        nSamples = 5, 
        interval = 1):
        """
        pin: Input pin number
        nSamples: Number of verifictions to acept a state change
        interval: Time interval in miliseconds between input value verifications
        """

        self._pin = Pin(pin, Pin.IN)

        self._nSamples = nSamples
        self._interval = interval
        self._currentValue = True
        self._ppl = pulses_per_litre
        
        # Number of pulses
        self._count     = 0
        self._lastCount = 0
        self._flow      = 0

        # Start the main task
        self.pulse_task = asyncio.create_task(self._run())

        # Start the flow update task
        self.flow_task = asyncio.create_task(self._updateFlow())


    async def _run(self):
        """
        Read the digital input and detect new value
        Debunce and filter routine are included here
        """
        while True:
            for count in range(self._nSamples):
                if self._currentValue == self._pin.value():
                    break
                else:
                    await asyncio.sleep_ms(self._interval)
            if count == self._nSamples - 1:
                # Edge detected
                self._currentValue = not self._currentValue
                if self._currentValue:
                    # Raising edge
                    self._count += 1

            await asyncio.sleep_ms(self._interval)


    async def _updateFlow(self):
        """
        Compute the flow (L/min) every 10s
        """
        while True:
            self._flow = (self._count - self._lastCount)/self._ppl*6
            self._lastCount = self._count
            await asyncio.sleep(10)

    # ----------------------------------------------------------   
    def getStatus(self):
        return {
            'liters': self.getLiters(),
            'flow': self._flow
        }

    # ----------------------------------------------------------        
    def getLiters(self):
        return self._count/self._ppl

    # ----------------------------------------------------------        
    def getFlow(self):
        return self._flow
       