import Nutmeg
from numpy import sin, cos, pi

# Assuming the core is on port 43686 (default)
Nutmeg.init()

x = [ 0.01*n for n in range(100) ]
y1 = [ sin(10*pi*t) for t in x ]
y2 = [ 10*pi*cos(10*pi*t) for t in x ]

fig = Nutmeg.figure("myFigure", "myFigure.qml")

fig.set("axis1.data", x=x, y=y1)
fig.set("axis2.data", x=x, y=y2)
