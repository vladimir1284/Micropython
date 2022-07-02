# exec(open('casa_webapp.py').read(),globals())
#
# This is a picoweb example showing a centralized web page route
# specification (classical Django style).
#
import picoweb
import uasyncio as asyncio
import json
from machine import Pin, ADC

from pump import Pump
from DigitalOutputs import bomba
from FlowMeter import FlowMeter
from PowerMeter import PowerMeter
from VoltMeter import VoltMeter
from settings import Settings
from DigitalInput import DigitalInput
from LDR import LDR


flow = FlowMeter(13)

pir1 = DigitalInput(14, active_high = True, interval = 10)
pir2 = DigitalInput(27, active_high = True, interval = 10)
pir3 = DigitalInput(26, active_high = True, interval = 10)

vbat = VoltMeter(33)

powermeter = PowerMeter(39)


ldr_taller = LDR(36)

door = ADC(Pin(34))
door.atten(ADC.ATTN_11DB)

cfg = Settings(1)

loop = asyncio.get_event_loop()

pump = Pump(bomba, loop, cfg, 1)

def get_data(req, resp): 
    global pump 
    values={
        'pump':pump.getStatus(),
        'Vbat': vbat.getVoltage(),
        'pwr': powermeter.getStatus(),
        'ldr': ldr_taller.getLuminosity(),
        'flowmeter': flow.getStatus(),
        'PIRs':{
            'sala': pir1.isActive(),
            'saleta': pir2.isActive(),
            'taller': pir3.isActive(),
        }
    }
    yield from picoweb.jsonify(resp, values)

def ack_error(req, resp):    
    global pump
    pump.ackError()
    yield from picoweb.jsonify(resp, {'result':'Successfully acknowledged!'})

import ulogging as logging
#logging.basicConfig(level=logging.INFO)
logging.basicConfig(level=logging.DEBUG)

def main(name):
    loop.create_task(pump.run())

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

