import QtQuick 2.4
import Nutmeg 0.1

/*!
    \ingroup figures
    StreamingPlot is used for drawing 2D X-Y data plots with streaming data.
    It has append and extend methods for incrementally adding data.
    The size property defines how big the buffer should be.
    Axis should be a direct parent. An Axis can contain many Plot choldren.
*/
StreamingPlotBase {
    id: streamingPlot
    width: parent.width
    height: parent.height
    handle: "data"

    property var figure: axis ? axis.figure : null

    canvas: lineCanvas
    LinePlotCanvas {
        id: lineCanvas
        width: parent.width*scaling
        height: parent.height*scaling
        transform: Scale {
            xScale: 1/lineCanvas.scaling
            yScale: 1/lineCanvas.scaling
        }
    }
}
