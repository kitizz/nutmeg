import QtQuick 2.0
import QtQuick.Layouts 1.0
import Nutmeg 0.1

/*!
    \ingroup figures
    One Axis should be placed for each visual plot the user wishes to show.
    Figure should be an ancestor of Axis.
*/
Axis2DBase {
    id: axisItem
    handle: "ax"
    objectName: "axis2d"
    width: parent.width
    height: parent.height

    default property alias plotArea: plots.children

    property real widthFraction: -1
    property real heightFraction: -1

    // ------------------------------
    //      Config open to the user
    // ------------------------------
    /*! \property type:color backgroundColor
    Set the color of the background.
    \qmlOnly
    */
    property alias backgroundColor: plotFrame.color

    /*! \property type:border border
    Set the border of the axis plotting frame.
    \qmlOnly
    */
    property alias border: plotFrame.border

    /*! \property navigationEnabled
    Set whether the user can navigate this axis, or if it is fixed.
    \qmlOnly
    */
    property bool navigationEnabled: true

    //----------------------
    //      Internal
    //----------------------
    property real xNumbersMargin: 0
    property real yNumbersMargin: 0
    // The numbers to display on the axes
    property var xNumbers: []
    property var yNumbers: []

    margin {
        top: 5; right: 25;
        bottom: 5;
        left: 5
    }

    children: [
        Rectangle {
            id: plotFrame
            z: -2
            x: plotRect.x
            y: plotRect.y
            width: plotRect.width
            height: plotRect.height

            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        AxisCanvas2D {
            id: axisCanvas
            x: 0; y: 0; z: 100
            axis: axisItem
            width: parent.width * 2
            height: parent.height * 2
            transform: Scale {
                xScale: 1.0/(2)
                yScale: 1.0/(2)
            }
        },

        Item {
            id: plots
            anchors.fill: plotFrame
        },

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            property var closestPlot: null
            property point plotPoint: Qt.point(0,0)
            property int pointIndex: -1
            property real maxDistance: Infinity

            Timer {
                id: tipTimer
                repeat: false
                interval: 300
                property point pos

                onTriggered: {
                    mouseArea.updatePoint(pos)
                    console.log("Pressed:", pos)

                    if (mouseArea.closestPlot) {
                        var local = Vector.point(mapToItem(mouseArea.closestPlot, pos.x, pos.y))
                        mouseArea.closestPlot.updateTip(local)
                    }
                }
            }

            onDoubleClicked: {
                tipTimer.stop()
                axisItem.limits = undefined
            }

            onPressed: {
                if (mouse.buttons & Qt.RightButton) {
                    console.log("Right Pressed")
                    clearPlotTips()
                    return
                }

                // Disable this for now...
//                tipTimer.pos = Vector.point(mouse)
//                tipTimer.start()
            }

            onPositionChanged: {
                if (!(mouse.buttons & Qt.LeftButton)) return
                if (closestPlot) {
                    var local = Vector.point(mapToItem(closestPlot, mouse.x, mouse.y))
                    closestPlot.updateTip(local)
                }
            }

            function updatePoint(mouseP) {
                // Look through the plots in this axis, and find the one with the
                // closest data point to the click.
                var bestPlot = null
                var bestPoint = Qt.point(0,0)
                var bestDist = Infinity
                var bestIndex = -1

                var plotList = getPlotList()
                for (var i=0; i<plotList.length; ++i) {
                    var plot = plotList[i]

                    var localMouse = Vector.point(mapToItem(plot, mouseP.x, mouseP.y))
                    var p = plot.itemToData(localMouse)
                    var ind = plot.nearestDataTo(p)
                    var closest = plot.frameLocationAt(ind)

                    var dist = Vector.distance2(closest, localMouse)
                    if (dist < bestDist) {
                        bestDist = dist
                        bestPlot = plot
                        bestPoint = closest
                        bestIndex = ind
                    }
                }
                if (bestPlot && bestDist < maxDistance*maxDistance) {
                    closestPlot = bestPlot
                    plotPoint = bestPoint
                    pointIndex = bestIndex
                } else {
                    closestPlot = null
                    pointIndex = -1
                }
            }
        }
    ]

    // ------------------
    //      Functions
    // ------------------

    function clearPlotTips() {
        mouseArea.closestPlot = null
        mouseArea.pointIndex = -1
        var plots = getPlotList()
        console.log("Clear tips for", plots)
        for (var i=0; i<plots.length; ++i) {
            plots[i].clearTips()
        }
    }

}
