import sys, os
sys.path.append( os.path.abspath("..") )

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

x = np._r[0:10.]
ySin = np.sin(x/3)
fig.set('ax[0].green.y', ySin)

yTan = np.tan(x/3)
fig.set('ax[2].blue.y', yTan)
