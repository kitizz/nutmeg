import Nutmeg


Nutmeg.init()
fig = Nutmeg.figure('imageExample', "figureIm.qml")

im = open("img.jpg")
imData = im.read()
fig.set('ax.im.binary', imData)

fig.set('ax.data', x=[0, 1, 2, 800, 1500], y=[0, 500, 100, 800, 1000])

# fig.set('ax.data2', {'x': [0, 1, 2, 800, 1500], 'y': [0, 300, 200, 900, 1000]})
