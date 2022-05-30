import uasyncio as asyncio
from server import serve
from DigitalOutputs import ssr2A

loop = asyncio.get_event_loop()

async def blink():
    while True:
        if ssr2A.value:
            ssr2A.turnOff()
        else:
            ssr2A.turnOn()
        await asyncio.sleep(1)

async def add_client(ws, path):
    print("Connection on {}".format(path))

    try:
        async for msg in ws:
            print(msg)
            if msg == "on":
                tsk_blink = loop.create_task(blink())
            else:
                if msg == "off":
                    try:
                        tsk_blink.cancel()
                    except Exception as err:
                        print(err)

            await ws.send(msg)
    finally:
        print("Disconnected")


ws_server = serve(add_client, "0.0.0.0", 7776)

loop.run_until_complete(ws_server)
loop.run_forever()
