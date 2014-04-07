import QtQuick 2.0
import Graphr 1.0

LinePlotBase {
    id: linePlot
    width: parent.width
    height: parent.height
    handle: "data"
//    app: axis.app
    onAppChanged: console.log("LinePlot appChanged", app)

    canvas: lineCanvas
    children: [
        LinePlotCanvas {
            id: lineCanvas
            width: parent.width*scaling
            height: parent.height*scaling
            transform: Scale {
                xScale: 1/lineCanvas.scaling
                yScale: 1/lineCanvas.scaling
            }
            app: linePlot.app
//            MouseArea {
//                anchors.fill: parent
//                hoverEnabled: true
//                onPositionChanged: {
//                    console.log(linePlot, "mouse:", mouse.x, mouse.y)
//                    mouse.accepted = false
//                }
//            }
        }
    ]
}
