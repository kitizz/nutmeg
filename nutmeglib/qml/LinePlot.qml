import QtQuick 2.0
//import QtQml 2.1
import Nutmeg 0.1

/*!
    \ingroup figures
    LinePlot is used for drawing 2D X-Y data plots.
    Axis should be a direct parent. An Axis can contain many Plot choldren.
*/
LinePlotBase {
    id: linePlot
    width: parent.width
    height: parent.height
    handle: "data"

    property var figure: axis ? axis.figure : null

//    Rectangle {
//        anchors.fill: parent
//        color: "transparent"
//        border { width: 1; color: "red" }
//    }

//    canvas: lineCanvas
//    LinePlotCanvas {
//        id: lineCanvas
//        width: parent.width*scaling
//        height: parent.height*scaling
//        transform: Scale {
//            xScale: 1/lineCanvas.scaling
//            yScale: 1/lineCanvas.scaling
//        }
//    }
}
