import Nutmeg
import time

from scipy import ndimage
import numpy as np

# Init the figure
Nutmeg.init()
fig = Nutmeg.figure('paramExample', 'figure1.qml')

# Set the GUI as described in the qml file
success = fig.setGui('gui1.qml')
sigmaParam = fig.parameter("sigma")

N = 100
data = np.random.standard_normal((3, N*10))


def applyBlur(dataIn, sigma):
    return ndimage.gaussian_filter1d(dataIn, sigma, axis=1)


while True:
    if sigmaParam.changed:
        sigma = sigmaParam.read()
        print "Sigma Changed:", sigma

        dataBlur = applyBlur(data, sigma)

        fig.set('ax.blue', y=dataBlur)
