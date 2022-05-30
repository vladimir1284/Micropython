import uos, machine, network
#uos.dupterm(None, 1) # disable REPL on UART(0)
import webrepl
import gc

from DigitalOutputs import *

# get the interface's IP/netmask/gw/DNS addresses
ap = network.WLAN(network.AP_IF) # create access-point interface
ap.active(True)
# activate the interface
ap.config(essid='Casa', authmode=network.AUTH_OPEN) # set the ESSID of the access point
webrepl.start()
gc.collect()