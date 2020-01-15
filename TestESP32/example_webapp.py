#
# This is a picoweb example showing a centralized web page route
# specification (classical Django style).
#
import ure as re
import picoweb
import network
import uasyncio as asyncio

from machine import Pin
n = 0;
blinkDelay = 100
led = Pin(2, Pin.OUT)


# get the interface's IP/netmask/gw/DNS addresses
ap = network.WLAN(network.AP_IF) # create access-point interface
ap.active(True)
# activate the interface
ap.config(essid='ESP-8266', authmode=network.AUTH_OPEN) # set the ESSID of the access point

def get_rand_number(req, resp):
    global n
    n = n+1
    yield from picoweb.jsonify(resp, {'rand_number': n})
    
def receive_data(req, resp):
    global blinkDelay
    if req.method == "POST":
        yield from req.read_form_data()
    else:  # GET, apparently
        # Note: parse_qs() is not a coroutine, but a normal function.
        # But you can call it using yield from too.
        req.parse_qs()

    # Whether form data comes from GET or POST request, once parsed,
    # it's available as req.form dictionary
    blinkDelay=int(req.form['delay'])
    print(blinkDelay)

    yield from picoweb.start_response(resp)
    yield from resp.awrite("Data received")


ROUTES = [
    # You can specify exact URI string matches...
    ("/", lambda req, resp: (yield from app.sendfile(resp, "index.html"))),
    ("/gauge.min.js", lambda req, resp: (yield from app.sendfile(resp, "gauge.min.js"))),
    ("/input-knobs.js", lambda req, resp: (yield from app.sendfile(resp, "input-knobs.js"))),
    ('/get_rand_number', get_rand_number),
    ('/send_data', receive_data),
]


async def blinkLed():
    while(1):
        led.value(not(led.value()))
        await asyncio.sleep(blinkDelay/1000.)

import ulogging as logging
logging.basicConfig(level=logging.INFO)
#logging.basicConfig(level=logging.DEBUG)

loop = asyncio.get_event_loop()
loop.create_task(blinkLed())

app = picoweb.WebApp(__name__, ROUTES)
# debug values:
# -1 disable all logging
# 0 (False) normal logging: requests and errors
# 1 (True) debug logging
# 2 extra debug logging
app.run(host="0.0.0.0", port=80, debug=1)

