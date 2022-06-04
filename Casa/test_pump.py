# exec(open('test_pump.py').read(),globals())

import json
import uasyncio as asyncio
from server import serve
from pump import Pump
from DigitalOutputs import bomba
from settings import Settings

from machine import Pin, ADC


#TODO instanciate Flowmeter
flow = Pin(13, Pin.IN)
vbat = ADC(Pin(33))
vbat.atten(ADC.ATTN_11DB)

loop = asyncio.get_event_loop()

cfg = Settings(1)

pump = Pump(bomba, loop, cfg, 1)

async def add_client(ws, path):
    print("Connection on {}".format(path))

    try:
        async for msg in ws:
            print(msg)
            if msg == "get_data":
                await ws.send(json.dumps(
                        {
                            'pump': await asyncio.gather(pump.getStatus()),
                            'Vbat': vbat.read()*0.007
                        }
                    ))
            else:
                if msg == "ack_error":
                    pump.ackError()
                    await ws.send("ok")
                else:
                    await ws.send(msg)
    finally:
        print("Disconnected")


ws_server = serve(add_client, "0.0.0.0", 7777)


loop.run_until_complete(ws_server)
loop.run_until_complete(pump.run())
loop.run_forever()
