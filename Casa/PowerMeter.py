# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import uasyncio as asyncio
from uasyncio import Event
from micropython import const
from machine import Pin, ADC

THRESSHOLD = 1000

class PowerMeter:
    """
    Driver to count powermeter pulses.
    """
    def __init__(
        self,
        pin,
        pulses_per_kwh = 1600,
        nSamples = 5,
        interval = 10):
        """
        pin: Input pin number
        nSamples: Number of verifications to accept a state change
        interval: Time interval in milliseconds between input value verifications
        """

        self._pin = ADC(Pin(pin))
        self._pin.atten(ADC.ATTN_11DB)

        self._nSamples = nSamples
        self._interval = interval
        self._currentValue = self._pin.read()
        self._ppkwh = pulses_per_kwh
        
        # Number of pulses
        self._count     = 0
        self._lastCount = 0
        self._power      = 0

        # self._data = []

        # Start the main task
        self.pulse_task = asyncio.create_task(self._run())

        # Start the power update task
        self.power_task = asyncio.create_task(self._updatePower())


    async def _run(self):
        """
        Read the analog input and detect pulses
        Debounce and filter routine are included here
        """
        while True:
            for count in range(self._nSamples):
                if abs(self._currentValue - self._pin.read()) < THRESSHOLD:
                    break
                else:
                    await asyncio.sleep_ms(self._interval)
            if count == self._nSamples - 1:
                # Edge detected
                value = self._pin.read()
                if value < self._currentValue:
                    # Falling edge
                    self._count += 1
                    # self._data.append((time.ticks_ms(), value))
                self._currentValue = value

            await asyncio.sleep_ms(self._interval)


    async def _updatePower(self):
        """
        Compute the power (watt) every minute
        """
        while True:
            self._power = (self._count - self._lastCount)/self._ppkwh*60000
            self._lastCount = self._count
            # print(self._data)
            # self._data = []
            await asyncio.sleep(60)

    # ----------------------------------------------------------   
    def getStatus(self):
        return {
            'energy': self.getEnergy(), # kWh
            'power': self._power
        }

    # ----------------------------------------------------------        
    def getEnergy(self):
        """
        Returns the accumulated energy (kWh)
        """
        return self._count/self._ppkwh

    # ----------------------------------------------------------        
    def getPower(self):
        """
        Returns the power (watt)
        """
        return self._power
       