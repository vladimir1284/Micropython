# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

from machine import Pin, ADC


class LDR:
    """
    Driver to measure illumination.
    """
    def __init__(
        self,
        pin,
        max_code = 1000,
        min_code = 4096):
        """
        pin: Input pin number
        max_code: Code associated to the maximum bright
        min_code: Code associated to the minimum bright
        """

        self._pin = ADC(Pin(pin))
        self._pin.atten(ADC.ATTN_11DB)

        self._max_code = max_code
        self._min_code = min_code

    # ----------------------------------------------------------   
    def getLuminosity(self):
        return int(100*(self._pin.read() - self._min_code)/(self._max_code - self._min_code))
