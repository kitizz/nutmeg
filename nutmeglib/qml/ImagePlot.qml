import QtQuick 2.0
import QtQml 2.1
import Nutmeg 1.0

/*!
    \ingroup figures
    LinePlot is used for drawing 2D X-Y data plots.
    Axis should be a direct parent. An Axis can contain many Plot choldren.
*/
ImagePlotBase {
    id: imagePlot
    width: parent.width
    height: parent.height
    handle: "data"

    property var figure: axis.figure

    canvas: imageCanvas
    ImageCanvas {
        id: imageCanvas
        width: parent.width*scaling
        height: parent.height*scaling
        transform: Scale {
            xScale: 1/imageCanvas.scaling
            yScale: 1/imageCanvas.scaling
        }
    }
}
