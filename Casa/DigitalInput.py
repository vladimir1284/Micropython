# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import uasyncio as asyncio
from uasyncio import Event
from machine import Pin, time_pulse_us


class DigitalInput:
    """
    Driver to debounce and filter digital inputs.
    """
    def __init__(
        self,
        pin: int,
        description: str,
        pull = None,
        active_high = True,
        nSamples = 5, 
        interval = 1,
        raising_edge: Event = None,
        falling_edge: Event = None):
        """
        pin: Input pin number
        pull: Pull up or pull down if needed
        active_high: If the input is to be considered active in HIGH state
        nSamples: Number of verifications to accept a state change
        interval: Time interval in milliseconds between input value verifications
        raising_edge: uAsyncio Event to be set on raising edge detection
        falling_edge: uAsyncio Event to be set on falling edge detection

        Emits events on falling and raising edges if the objet is provided
        """

        if pull is not None:
            self._pin = Pin(pin, Pin.IN, pull)
        else:
            self._pin = Pin(pin, Pin.IN)

        self._active_high = active_high
        self._nSamples = nSamples
        self._interval = interval
        self._currentValue = True
        self._raising_edge = raising_edge
        self._falling_edge = falling_edge
        self.description = description
        
        # Start the main task
        self.task = asyncio.create_task(self._run())


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
                    if self._raising_edge is not None:
                        self._raising_edge.set()
                else:
                    # Falling edge
                    if self._falling_edge is not None:
                        self._falling_edge.set()

            await asyncio.sleep_ms(self._interval)

    # ----------------------------------------------------------        
    def isActive(self):
        return self._currentValue == self._active_high
       