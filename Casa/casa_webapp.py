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
from settings import Settings


flow = FlowMeter(13)

pir1 = Pin(14, Pin.IN)
pir2 = Pin(27, Pin.IN)
pir3 = Pin(26, Pin.IN)

vbat = ADC(Pin(33))
vbat.atten(ADC.ATTN_11DB)

powermeter = PowerMeter(39)


ldr_taller = ADC(Pin(36))
ldr_taller.atten(ADC.ATTN_11DB)

door = ADC(Pin(34))
door.atten(ADC.ATTN_11DB)

cfg = Settings(1)

loop = asyncio.get_event_loop()

pump = Pump(bomba, loop, cfg, 1)

def get_data(req, resp): 
    global pump 
    values={
        'pump': await asyncio.gather(pump.getStatus()),
        'Vbat': vbat.read()*0.007,
        'pwr': powermeter.getStatus(),
        'ldr': ldr_taller.read(),
        'flowmeter': flow.getStatus()
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

