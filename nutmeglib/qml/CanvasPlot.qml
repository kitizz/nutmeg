import QtQuick 2.5
import Nutmeg 0.1

CanvasPlotBase {
    id: canvasPlot
    width: parent.width
    height: parent.height
    handle: "data"

    property var figure: axis.figure

//    onAxisChanged: {
//        if (!axis) return

//        xDataChanged.connect(axis.updateTicks)
//        yDataChanged.connect(axis.updateTicks)
//    }

    canvas: canvasCanvas // Teehee
    CanvasPlotCanvas {
        id: canvasCanvas
        width: parent.width*scaling
        height: parent.height*scaling
        transform: Scale {
            xScale: 1/canvasCanvas.scaling
            yScale: 1/canvasCanvas.scaling
        }
    }
}
