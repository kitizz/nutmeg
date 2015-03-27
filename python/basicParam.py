import Nutmeg

import numpy as np
from scipy import ndimage

# Init the figure
Nutmeg.init()
fig = Nutmeg.figure('paramExample', 'figure1.qml')

# Set the GUI as described in the qml file
success = fig.setGui('gui1.qml')

# Define a blur function
def applyBlur(dataIn, sigma):
    return ndimage.gaussian_filter1d(dataIn, sigma, axis=1)

N = 100
data = np.random.standard_normal((3, N*10))
data2 = np.random.standard_normal((3, N))

# update will be called whenever sigma is changed
# This returns a blurred version of data based on the sigma param.
update = lambda params: applyBlur(data, params['sigma'])

fig.set('ax[:].blue.x', np.arange(N*10,dtype=float)/10)
fig.set('ax[:].blue.y', Nutmeg.Updater(['sigma'], update=update))

# update2 will be called whenever signa is changed
update2 = lambda params: applyBlur(data2, params['sigma'])

fig.set('ax[:].red.x', np.arange(N, dtype=float))
fig.set('ax[:].red.y', Nutmeg.Updater(['sigma'], update=update2))


# SOme code
