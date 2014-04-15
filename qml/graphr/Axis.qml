import QtQuick 2.0
import Graphr 1.0
import QtQuick.Layouts 1.0

import "Vector.js" as Vector

AxisBase {
    id: axis
    handle: "ax"
    width: parent.width
    height: parent.height

    default property alias plotArea: plotFrame.children

    property real widthFraction: -1
    property real heightFraction: -1

    // Settings open to the user
    property alias backgroundColor: plotFrame.color
    property alias border: plotFrame.border

    // The numbers to display on the axes
    property var xNumbers: []
    property var yNumbers: []

    yAxis.onMajorTicksChanged: updateTickNumbers(0)
    xAxis.onMajorTicksChanged: updateTickNumbers(1)

    margin { top: 10; right: 10; bottom: 50; left: 50 }
    children: [
        Rectangle {
            id: plotFrame
            anchors.fill: parent
            anchors {
                leftMargin: axis.margin.left; rightMargin: axis.margin.right
                topMargin: axis.margin.top; bottomMargin: axis.margin.bottom
            }
            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        AxisCanvas2D {
            id: axisCanvas
            x: 0; y: 0
            axis: axis
            plotRect: Qt.rect(plotFrame.x, plotFrame.y, plotFrame.width, plotFrame.height)
            width: parent.width*scaling
            height: parent.height*scaling
            transform: Scale {
                xScale: 1/axisCanvas.scaling
                yScale: 1/axisCanvas.scaling
            }
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

            onPressed: {
                if (mouse.buttons & Qt.RightButton) {
                    console.log("Right Pressed")
                    clearPlotTips()
                    return
                }

                var pos = Vector.point(mouse)
                updatePoint(pos)
                console.log("Pressed:", mouse.x, mouse.y, pos)

                if (closestPlot) {
                    var local = Vector.point(mapToItem(closestPlot, mouse.x, mouse.y))
                    closestPlot.updateTip(local)
                }
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
    function updateTickNumbers(axis) {
        var ticks, numbers
        if (axis == 0) {
            ticks = xAxis.majorTicks
            numbers = xNumbers
        } else if (axis == 1) {
            ticks = yAxis.majorTicks
            numbers = yNumbers
        } else {
            return;
        }

        var diff = ticks.length - numbers.length
        if (diff > 0) {
            for (var i=0; i<diff; ++i)
                numbers.push(numberCmp.createObject(plotFrame, {'axis': 0}))
        } else if (ticks.length < numbers.length) {
            for (var i=0; i<-diff; ++i)
                numbers.pop().destroy()
        }

        // Update .value for all.
        for (var i=0; i<numbers.length; ++i) {
            numbers[i].value = ticks[i]
        }
    }

    function clearPlotTips() {
        mouseArea.closestPlot = null
        mouseArea.pointIndex = -1
        var plots = getPlotList()
        console.log("Clear tips for", plots)
        for (var i=0; i<plots.length; ++i) {
            plots[i].clearTips()
        }
    }

    Component {
        id: numberCmp
        Text {
            property int axis: -1
            property real value: 0
            text: value.toString()
            x: axis != 0 ? -implicitWidth - 5:
                           plotFrame.width*(value - minX)/(maxX - minX) - implicitWidth/2
            y: axis != 1 ? plotFrame.height + 5:
                           plotFrame.height*(1 - (value - minY)/(maxY - minY)) - implicitHeight/2
        }
    }
}
