from machine import Pin

class Shifter:  
    def __init__(self, ser, save, clk):
        # ser - data pin, save - rclk, clk - srclk pin
        self._ser = ser
        self._clk = clk
        self._save = save
        self._clk.off()

    def sendData(self, data):
        for bit in data:
            self._ser.value(bit)
            self._clk.on()
            self._clk.off()

        self._save.off()
        self._save.on()

class DigitalOutputs595:
    def __init__(self, ser_pin, save_pin, clk_pin, size):
        self._data = []
        for _ in range(size):
            self._data.append(0)

        ser  = Pin(ser_pin, Pin.OUT)                                                                                                                                 
        clk  = Pin(clk_pin, Pin.OUT)                                                                                                                              
        save = Pin(save_pin, Pin.OUT)
        self._shifter = Shifter(ser,save, clk)
        self._shifter.sendData(self._data)
      
    def digitalWrite(self, address, value):
        self._data[address] = value
        self._shifter.sendData(self._data)
      
    def getValue(self, address):
        return self._data[address]
