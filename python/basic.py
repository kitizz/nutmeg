# Code starts
import Nutmeg
import numpy as np

# Initialise the Nutmeg module. This connects to the core.
Nutmeg.init()

# Create the figure from a qml file
fig = Nutmeg.figure('fig', "figure1.qml")

# Set the data
randomData = np.random.standard_normal(10)
fig.set('ax[1].red.y', randomData)

x = np.r_[0:10.:0.01]
ySin = np.sin(x)
fig.set('ax[0].green', {'x': x, 'y': ySin})

yTan = np.tan(x)
fig.set('ax[2].blue', {'x': x, 'y': yTan})
