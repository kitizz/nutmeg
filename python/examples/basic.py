import Nutmeg
from numpy import sin, cos, pi, r_

# Assuming the core is on port 43686 (default)
Nutmeg.init()

x = r_[0:1:0.01]
y1 = sin(10*pi*x)
y2 = 10*pi*cos(10*pi*x)

fig = Nutmeg.figure("myFigure", "myFigure.qml")

fig.set("axis1.data", x=x, y=y1)
fig.set("axis2.data", x=x, y=y2)
