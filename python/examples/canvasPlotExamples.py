import Nutmeg


def basics():
    Nutmeg.figure('basicCanvas', 'canvasPlotBasic.qml')


def customBar():
    fig = Nutmeg.figure('customBar', 'customBarPlot.qml')
    fig.set('ax.barPlot', data=[1,2,3,-0.5,7], barWidth=1.5, spacing=3)


def customAdvancedBar():
    fig = Nutmeg.figure('customAdvancedBar', 'customAdvancedBarPlot.qml')
    fig.set('ax.barPlot', data=[[1,2,3, -2,0.5,6,1],[-0.5,7]], barWidth=1)
    fig.set('legend', labels=["Hey", "There", "Labels", "I", "Love", "Lamp"])

if __name__ == '__main__':
    Nutmeg.init()

    # basics()
    # customBar()
    customAdvancedBar()
