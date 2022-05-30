# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init
# pylint: disable=global-statement

import time
from utime import sleep_us
from micropython import const, alloc_emergency_exception_buf
from machine import Pin, Timer
try:
    from typing import Optional
    from ulogging import RootLogger
except ImportError:
    pass

# Debugging is simplified if the following code is included in any program using interrupts
alloc_emergency_exception_buf(100)

STABDELAY  = const(5)   # us
PULSEWITH  = const(10)  # us
TRIGPERIOD = const(65)  # ms

class HCSR04:
    """
    Driver to use the untrasonic sensor HC-SR04.
    The sensor range is between 2cm and 4m.

    The timeouts received listening to echo pin are converted to OSError('Out of range')

    """
    # echo_timeout_us is based in chip range limit (400cm)
    def __init__(
        self,
        trigger_pin: int, 
        echo_pins: list, 
        echo_timeout_us: int = 500*2*30, # 4m
        debug: int = 0,
        log: Optional[RootLogger] = None
        )-> None:
        """
        trigger_pin: Output pin to send pulses
        echo_pin: Readonly pin to measure the distance. The pin should be protected with 1k resistor
        echo_timeout_us: Timeout in microseconds to listen to echo pin. 
        By default is based in sensor limit range (4m)
        """

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("controller")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log
        
        self.echo_timeout_us = echo_timeout_us
        # Init trigger pin (out)
        self._trigger = Pin(trigger_pin, mode=Pin.OUT, pull=None)
        self._trigger.value(0)

        # Init echo pins (in)
        self.sensors = {}
        for echo_pin in echo_pins:
            self.sensors.setdefault(echo_pin, ECHO(echo_pin))

        timer_65ms = Timer(0)
        timer_65ms.init(period=TRIGPERIOD, mode=Timer.PERIODIC, callback=self._start_trigger)

    # ---- Handle sensor interrupt ---------
    def _start_trigger(self, a):
        self._trigger.value(0) # Stabilize the sensor
        sleep_us(STABDELAY)
        self._trigger.value(1)
        # Send a 10us pulse.
        sleep_us(PULSEWITH)
        self._trigger.value(0)


    def distance_mm(self, echo_pin:int):
        """
        Get the distance in milimeters without floating point operations.
        """
        pulse_time = self.sensors[echo_pin].pulse_time
        # To calculate the distance we get the pulse_time and divide it by 2 
        # (the pulse walk the distance twice) and by 29.1 becasue
        # the sound speed on air (343.2 m/s), that It's equivalent to
        # 0.34320 mm/us that is 1mm each 2.91us
        # pulse_time // 2 // 2.91 -> pulse_time // 5.82 -> pulse_time * 100 // 582 
        mm = pulse_time * 100 // 582
        return mm

    def distance_cm(self, echo_pin:int):
        """
        Get the distance in centimeters with floating point operations.
        It returns a float
        """
        #pulse_time = time.ticks_diff(self.sensors[echo_pin].received_us, self._sent_us)

        # To calculate the distance we get the pulse_time and divide it by 2 
        # (the pulse walk the distance twice) and by 29.1 becasue
        # the sound speed on air (343.2 m/s), that It's equivalent to
        # 0.034320 cm/us that is 1cm each 29.1us
        cms = (self.sensors[echo_pin].count / 2) / 29.1
        return cms

class ECHO:
    def __init__(self, pin: int):
        self.pulse_time = 0
        self.count = 0
        self._start_time = 0
        self.pin = Pin(pin, mode=Pin.IN, pull=None)
        self.pin.irq(trigger=Pin.IRQ_FALLING, handler=self._echo_received)
        self.pin.irq(trigger=Pin.IRQ_RISING, handler=self._signal_transmitted)

    # ---- Handle sensor interrupt ---------
    def _signal_transmitted(self, pin: Pin):
        self._start_time = time.ticks_us()

    # ---- Handle sensor interrupt ---------
    def _echo_received(self, pin: Pin):
        self.pulse_time = time.ticks_us()-self._start_time
        self.count += 1