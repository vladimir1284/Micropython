# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
import uasyncio as asyncio
from micropython import const
from machine import Pin, time_pulse_us

FILTER_SIZE = const(15)  # Number of elements in the median filter

class HCSR04:
    """
    Driver to use the untrasonic sensor HC-SR04.
    The sensor range is between 2cm and 4m.
    The timeouts received listening to echo pin are converted to OSError('Out of range')
    """
    # echo_timeout_us is based in chip range limit 30ms (400cm)
    def __init__(self, trigger_pin, echo_pin, echo_timeout_us=30000):
        """
        trigger_pin: Output pin to send pulses
        echo_pin: Readonly pin to measure the distance. The pin should be protected with 1k resistor
        echo_timeout_us: Timeout in microseconds to listen to echo pin. 
        By default is based in sensor limit range (4m)
        """
        self._echo_timeout_us = echo_timeout_us
        # Init trigger pin (out)
        self._trigger = Pin(trigger_pin, mode=Pin.OUT, pull=None)
        self._trigger.value(0)

        # Init echo pin (in)
        self._echo = Pin(echo_pin, mode=Pin.IN, pull=None)

    def _send_pulse_and_wait(self):
        """
        Send the pulse to trigger and listen on echo pin.
        We use the method `machine.time_pulse_us()` to get
        the microseconds until the echo is received.
        """
        self._trigger.value(0) # Stabilize the sensor
        time.sleep_us(5)
        self._trigger.value(1)
        # Send a 10us pulse.
        time.sleep_us(10)
        self._trigger.value(0)
        try:
            pulse_time = time_pulse_us(self._echo, 1, self._echo_timeout_us)
            return pulse_time
        except OSError as ex:
            if ex.args[0] == 110: # 110 = ETIMEDOUT
                raise OSError('Out of range')
            raise ex

    async def distance_cm(self):
        """
        Get the distance in centimeters with floating point operations.
        It uses a median filter of FILTER_SIZE steps to reduce the noise.
        """
        pulses_array = []
        for _ in range(FILTER_SIZE):
            try:
                pulses_array.append(self._send_pulse_and_wait())
            except OSError as ex:
                pulses_array.append(0)
            await asyncio.sleep(0) # Release Task


        # To calculate the distance we get the pulse_time and divide it by 2 
        # (the pulse walk the distance twice) and by 29.1 becasue
        # the sound speed on air (343.2 m/s), that It's equivalent to
        # 0.034320 cm/us that is 1cm each 29.1us
        cms = sorted(pulses_array)[FILTER_SIZE//2] * 0.017
        return cms
