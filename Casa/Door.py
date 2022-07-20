# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement
import uasyncio as asyncio
from machine import Pin, ADC



class Door:
    """
    Automatic light controller.
    """
    def __init__(
        self,
        pin,
        ):
        """
        pin: Magnetic sensor pin
        """  

        
        self._pin = ADC(Pin(pin))
        self._pin.atten(ADC.ATTN_11DB)

        self._status = "open circuit"

        # Start the main task
        self.task = asyncio.create_task(self._run())

    # ----------------------------------------------------------   
    async def _run(self):
        while True:
            val = self._pin.read()
            if (val < 1000):
                self._status = "short circuit"
            elif (val < 2150):
                self._status = "closed"
            elif (val < 3000):
                self._status = "open"
            else:
                self._status = "open circuit"

            await asyncio.sleep_ms(100) # Cycle every 100ms

    # ----------------------------------------------------------
    def getStatus(self):
        return self._status + " -> %i" % self._pin.read()
