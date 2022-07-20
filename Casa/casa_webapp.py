# exec(open('casa_webapp.py').read(),globals())
#
# This is a picoweb example showing a centralized web page route
# specification (classical Django style).
#
import gc
import picoweb
import uasyncio as asyncio
import json
from machine import Pin, ADC, WDT

from pump import Pump
from DigitalOutputs import bomba, lamp_taller
from FlowMeter import FlowMeter
from PowerMeter import PowerMeter
from VoltMeter import VoltMeter
from settings import Settings
from DigitalInput import DigitalInput
from LDR import LDR
from Light import Light
from Door import Door

#wdt = WDT(timeout=100000)  # enable it with a timeout of 100s

flow = FlowMeter(13)

pir_taller  = DigitalInput(26, active_high = True, nSamples = 3, interval = 5)
pir_hall    = DigitalInput(27, active_high = True, nSamples = 3, interval = 5)
pir_sala    = DigitalInput(14, active_high = True, nSamples = 3, interval = 5)

vbat = VoltMeter(33)

powermeter = PowerMeter(39)


ldr_taller = LDR(36)

door = Door(34)

cfg = Settings(1)

pump = Pump(bomba, powermeter, cfg)

light_taller = Light(pir_taller, lamp_taller, ldr = ldr_taller)

def get_data(req, resp):
    values={
        'pump':pump.getStatus(),
        'Vbat': vbat.getVoltage(),
        'pwr': powermeter.getStatus(),
        'light':light_taller.getStatus(),        
        'flowmeter': flow.getStatus(),
        'PIRs':{
            'sala': pir_taller.isActive(),
            'saleta': pir_hall.isActive()
        },
        'door': door.getStatus()
    }
    yield from picoweb.jsonify(resp, values)

def ack_error(req, resp):
    pump.ackError()
    yield from picoweb.jsonify(resp, {'result':'Successfully acknowledged!'})

async def clean():
    while True:
        wdt.feed() # reset timer (feed watchdog)
        gc.collect()
        await asyncio.sleep(1)

import ulogging as logging
#logging.basicConfig(level=logging.INFO)
logging.basicConfig(level=logging.DEBUG)

def main(name):
    # loop = asyncio.get_event_loop()
    # loop.create_task(clean())

    ROUTES = [
        # You can specify exact URI string matches...
        ("/", lambda req, resp: (yield from app.sendfile(resp, "casa.html"))),
        ('/get_data', get_data),
        ('/ack_error', ack_error),
    ]
    app = picoweb.WebApp(name, ROUTES)

    # debug values:
    # -1 disable all logging
    # 0 (False) normal logging: requests and errors
    # 1 (True) debug logging
    # 2 extra debug logging
    app.run(host="0.0.0.0", port=80, debug=-1)

