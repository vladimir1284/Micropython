from pylab import *
data = [(22352, 1251), (28592, 1248), (34846, 1275), (41142, 1291), (47453, 1270), (53763, 1286), (60088, 1271), (66444, 1280), (72794, 1270), (79158, 1245)] 
t = []
val = []

for x in data:
    t.append(x[0]/1000.)
    val.append(x[1])

plot(t, val, '*')
grid(True)
show()