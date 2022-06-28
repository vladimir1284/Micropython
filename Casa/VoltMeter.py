# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import uasyncio as asyncio
from uasyncio import Event
from micropython import const
from machine import Pin, ADC

class VoltMeter:
    """
    Driver measure DC battery voltage.
    """
    def __init__(
        self,
        pin,
        factor: float = 0.007,
        nSamples = 100,
        interval = 100):
        """
        pin: Input pin number
        factor: Calibration coefficient
        nSamples: Number of averaged samples
        interval: Time interval in milliseconds between samples
        """

        self._pin = ADC(Pin(pin))
        self._pin.atten(ADC.ATTN_11DB)

        self._nSamples = nSamples
        self._interval = interval
        self._factor = factor

        
        self._sum: float = 0

        # Start the main task
        self.pulse_task = asyncio.create_task(self._run())


    async def _run(self):
        """
        Read the analog input and average the samples
        """
        while True:
            value = 0
            for _ in range(self._nSamples):
                value += self._pin.read()
                await asyncio.sleep_ms(self._interval)

            self._sum = value

    # ----------------------------------------------------------        
    def getVoltage(self):
        """
        Returns the voltage (V) 
        """
        return self._sum/self._nSamples*self._factor
       