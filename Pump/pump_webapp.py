#
# This is a picoweb example showing a centralized web page route
# specification (classical Django style).
#
import picoweb
import webrepl
import urandom
import uasyncio as asyncio

from machine import Pin
# n = 0;
# blinkDelay = 100
# led = Pin(2, Pin.OUT)

values = {
    'uLevel': 0,
    'lLevel': 0,
    'pumpState': 0,
}

uTank = {
    'min_level': 20,
    'capacity': 550,
    'min': 25,
    'restart': 60,
    'height': 110,
    'gap': 16,
}
lTank = {
    'min_level': 20,
    'capacity': 200,
    'min': 20,
    'restart': 80,
    'height': 60,
    'gap': 16,
}

pump = {
    'Tmax': 5,
    'startDelay': 3
}

indicators = {'upper_tank':uTank,'lower_tank':lTank,'pump':pump}

def sendIndicatorConfigs(req, resp):
    req.parse_qs()
    yield from picoweb.jsonify(resp, indicators[req.form['indicator']])

def webreplHandle(req, resp):
    req.parse_qs()
    if (req.form['state'] =='on'):
        webrepl.start()
    if (req.form['state'] =='off'):
        webrepl.stop()
    yield from picoweb.jsonify(resp, {'help':'webrepl?state=on/off'})

def get_values(req, resp):
    yield from picoweb.jsonify(resp, values)


async def updateValues():
    global values
    while(1):
        values['uLevel'] = urandom.getrandbits(7)%100
        values['lLevel'] = urandom.getrandbits(7)%100
        values['pumpState'] = urandom.getrandbits(1)
        await asyncio.sleep(1)

import ulogging as logging
logging.basicConfig(level=logging.INFO)
#logging.basicConfig(level=logging.DEBUG)

def main(name):
    loop = asyncio.get_event_loop()
    loop.create_task(updateValues())

    ROUTES = [
        # You can specify exact URI string matches...
        ("/", lambda req, resp: (yield from app.sendfile(resp, "tanque.html"))),
        ("/indicators.js", lambda req, resp: (yield from app.sendfile(resp, "indicators.js"))),
        ("/w3.css", lambda req, resp: (yield from app.sendfile(resp, "w3.css"))),
        ("/webrepl.html", lambda req, resp: (yield from app.sendfile(resp, "webrepl.html"))),
        ("/FileSaver.js", lambda req, resp: (yield from app.sendfile(resp, "FileSaver.js"))),
        ("/term.js", lambda req, resp: (yield from app.sendfile(resp, "term.js"))),
        ('/get_values', get_values),
        ('/get_indicator_config', sendIndicatorConfigs),
        ('/webrepl', webreplHandle),
    ]
    app = picoweb.WebApp(name, ROUTES)

    # debug values:
    # -1 disable all logging
    # 0 (False) normal logging: requests and errors
    # 1 (True) debug logging
    # 2 extra debug logging
    app.run(host="0.0.0.0", port=80, debug=0)

