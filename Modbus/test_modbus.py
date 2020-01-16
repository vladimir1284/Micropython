from uModBusAsyncSerial import uModBusSerial
from machine import Pin
import uasyncio as asyncio

tx=Pin(1,Pin.OUT)                                                                                                                                 
rx=Pin(3,Pin.IN) 
mbMaster = uModBusSerial(0, baudrate=19200, pins=[tx,rx])

async def main_loop():
    while(1):
        result = await mbMaster.read_holding_registers(1,0,1,signed=False)
        print(result)
        await asyncio.sleep(3)

loop = asyncio.get_event_loop()
loop.create_task(main_loop())
loop.run_forever()