from uModBusAsyncSerial import uModBusSerial
from machine import Pin
import uasyncio as asyncio

tx=Pin(1,Pin.OUT)                                                                                                                                 
rx=Pin(3,Pin.IN) 
mbMaster = uModBusSerial(0, 19200, 8, 1, None, [tx,rx], 16)

async def main_loop():
    while(1):
        print("ok")
        result = await mbMaster.read_holding_registers(1,0,1,signed=False)
        print(result)
        await asyncio.sleep(3)

loop = asyncio.get_event_loop()
loop.create_task(main_loop())
loop.run_forever()