# pylint: disable=import-outside-toplevel
# pylint: disable=consider-using-f-string
# pylint: disable=attribute-defined-outside-init

import time
import uasyncio as asyncio
from micropython import const
from machine import Pin
from DigitalInput import DigitalInput
from settings import Settings

try:
    from typing import Optional
    from ulogging import RootLogger
    from DigitalOutputs import Dout
    from DigitalInput import DigitalInput
    from Door import Door
except ImportError:
    pass

PASSWD = "111284"

class Alarm:

    def __init__(
        self,
        siren: Dout,
        door: Door,
        perimeter: list[DigitalInput],
        access: list[DigitalInput],
        settings: Settings,
        debug: int = 0,
        log: Optional[RootLogger] = None
        ):

        if log is None and debug >= 0:
            import ulogging
            log = ulogging.getLogger("alarm")
            if debug > 0:
                log.setLevel(ulogging.DEBUG)
        self._log = log        

        self._fsmFunction = {
            'DISARMED': self._disarmed,
            'AWAY': self._away,
            'NIGHT': self._night,
            'WAIT_AWAY': self._wait_away,
            'DELAY_AWAY': self._delay_away,
            'DELAY_NIGHT': self._delay_night,
            'DISARM_AWAY': self._disarm_away,
            'DISARM_NIGHT': self._disarm_night,
            'ERROR': self._error,
        }

        self._cfg  = settings
        self._siren = siren
        self._door = door
        self._perimeter = perimeter
        self._access = access

        self._state = 'DISARMED'
        self._next_state = None
        self._cause = None
        self._stateChange = time.time()

        # Start the main task
        self.task = asyncio.create_task(self._run())

    async def _run(self):
        while True:
            self._fsmFunction[self._state]()
            await asyncio.sleep_ms(10) # Cycle every 10ms


    def _error(self):
        asyncio.create_task(self._error_tsk())

    async def _error_tsk(self):
        self._buzz(400)
        await asyncio.sleep_ms(500)
        self._buzz(400)
        self._setState('DISARMED')

    def _disarmed(self):
        if self._next_state is not None:           
            if self._next_state == 'AWAY':
                if self._door.getStatus() == "closed":
                    self._setState('ERROR')
                else:
                    self._setState('WAIT_AWAY')
            else:
                if self._next_state == 'NIGHT':
                    self._setState('DELAY_NIGHT')

            self._next_state = None

    def _wait_away(self):
        if self._door.getStatus() == "closed":
            self._setState('DELAY_AWAY')

    def _delay_away(self):
        if time.time() - self._cfg.settings['ALARM_DELAY'] > self._stateChange:
            self._buzz(200)
            self._setState('AWAY')

    def _delay_night(self):
        if self._door.getStatus() == "open":
            self._setState('ERROR')

        if time.time() - self._cfg.settings['ALARM_DELAY'] > self._stateChange:
            self._buzz(200)
            self._setState('NIGHT')

    def _check(self):
        # Check pir sensors
        for pir in self._access + self._perimeter:
            if pir.isActive():
                return False

        # Wait for password when door is opened
        if self._door.getStatus() == 'open':
            return False
        return True

    def _away(self):
        # Check pir sensors
        for pir in self._access + self._perimeter:
            if pir.isActive():
                self._cause = "Activated PIR: {}".format(pir.description)
                self._event()

        # Wait for password when door is opened
        if self._door.getStatus() == 'open':
            self._cause = "Wait after opened door"
            self._buzz(200)
            self._setState('DISARM_AWAY')
        

    def _night(self):
        # Check pir sensors
        for pir in self._perimeter:
            if pir.isActive():
                self._cause = "Activated PIR: {}".format(pir.description)
                self._event()

        # Check door sensor
        if self._door.getStatus() == 'open':
            self._cause = "Opened door"
            self._event()

        # Wait for password when access Pir are activated
        for pir in self._access:
            if pir.isActive():
                self._cause = "Wait after Pir {} activated".format(pir.description)
                self._buzz(200)
                self._setState('DISARM_NIGHT')


    def _disarm_away(self):
        if time.time() - self._cfg.settings['ALARM_DISARM_DELAY'] > self._stateChange:
            if self._cause is not None:
                self._event()
            if self._check():
                self._setState('AWAY')       

    def _disarm_night(self):
        if time.time() - self._cfg.settings['ALARM_DISARM_DELAY'] > self._stateChange:
            if self._cause is not None:
                self._event()
            if self._check():
                self._setState('NIGHT')

    # Store event and make sound for delay seconds
    def _event(
        self,
        delay: int = 60
        ):
        # Store event
        try:
            with open("events.csv",'a', encoding='utf-8') as f:
                f.write(str(time.localtime())+","+self._cause+"\n")
        except Exception as err:
            self._log.error(err)

        self._cause = None
        # Sound
        self._buzz(1000*delay)

    def _setState(self, state):
        self._state = state
        self._stateChange = time.time()
        self._log.debug("{}-> State: {}".format(time.time(), self._state))

    def _buzz(self, delay: int):
        asyncio.create_task(self._sound(delay))

    # Make sound for delay ms
    async def _sound(self, delay: int):
        if (delay > 0):
            self._siren.turnOn()
            await asyncio.sleep_ms(delay)
            self._siren.turnOff()

    def clearEvents(self):
        try:
            with open("events.csv",'w', encoding='utf-8') as f:
                f.write('')
        except Exception as err:
            return err

    def getEvents(self):
        try:
            with open("events.csv",'r', encoding='utf-8') as f:
                return f.read()
        except Exception as err:
            return err

    def changeMode(
        self,
        mode: str, 
        passwd: str
        ):
        if passwd == PASSWD:
            if mode  == 'DISARMED':
                self._buzz(200)
                self._next_state = None
                self._setState('DISARMED')
            else:
                self._next_state = mode
            return 'Success!'
        else:
            return 'Wrong PASSWORD!'
            
    # ----------------------------------------------------------   
    def getStatus(self):
        status = {
            'door': self._door.getStatus(),
            'state': self._state
        }
        for pir in self._access + self._perimeter:
            status.setdefault(pir.description, pir.isActive())

        return status