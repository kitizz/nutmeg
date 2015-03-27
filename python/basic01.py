# Code starts
import Nutmeg
import numpy as np

# Initialise the Nutmeg module. This connects to the core.
Nutmeg.init()

# Create the figure from a qml file
fig = Nutmeg.figure('basic01', "figure_single.qml")

# Set the data
randomData = np.random.standard_normal(10)
fig.set('ax.redPlot.bars', randomData)

x = np.r_[0:10.:0.01]
ySin = np.sin(x)
fig.set('ax.greenPlot', {'x': x, 'y': ySin})

yTan = np.tan(x)
fig.set('ax.bluePlot', {'x': x, 'y': yTan})
