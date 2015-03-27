import Nutmeg
import time

# Init the figure
Nutmeg.init()
fig = Nutmeg.figure('paramExample', 'figure1.qml')

# Set the GUI as described in the qml file
success = fig.setGui('gui1.qml')
sigmaParam = fig.parameter("sigma")

while True:
    if sigmaParam.changed:
        print "Sigma Changed:", sigmaParam.read()