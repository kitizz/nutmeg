import QtQuick 2.0
import Graphr 1.0
import QtQuick.Layouts 1.0

import "Vector.js" as Vector
import "Util.js" as Util

/*!
    \ingroup figures
    One Axis should be placed for each visual plot the user wishes to show.
    Figure should be an ancestor of Axis.
*/
AxisBase {
    id: axisItem
    handle: "ax"
    width: parent.width
    height: parent.height

    default property alias plotArea: plotFrame.children
    property rect plotRect: Qt.rect(plotFrame.x, plotFrame.y, plotFrame.width, plotFrame.height)

    property real widthFraction: -1
    property real heightFraction: -1

    // Settings open to the user
    property alias backgroundColor: plotFrame.color
    property alias border: plotFrame.border

    // The numbers to display on the axes
    property var xNumbers: []
    property var yNumbers: []
    property int xPrecision: 3
    property int yPrecision: 3

    onWidthChanged: updateTickNumbers(0)
    onHeightChanged: updateTickNumbers(1)
    xAxis.onMajorTicksChanged: updateTickNumbers(0)
    yAxis.onMajorTicksChanged: updateTickNumbers(1)
    onLimitsChanged: updateTicks()
    Component.onCompleted: updateTicks()

    margin { top: 25; right: 25; bottom: 50; left: 50 }
    children: [
        Rectangle {
            id: plotFrame
            anchors.fill: parent
            anchors {
                leftMargin: axisItem.margin.left; rightMargin: axisItem.margin.right
                topMargin: axisItem.margin.top; bottomMargin: axisItem.margin.bottom
            }
            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        AxisCanvas2D {
            id: axisCanvas
            x: 0; y: 0
            axis: axisItem
            plotRect: axisItem.plotRect
            width: parent.width*scaling
            height: parent.height*scaling
            transform: Scale {
                xScale: 1/axisCanvas.scaling
                yScale: 1/axisCanvas.scaling
            }
        },

        Text {
            id: xAxisOffset
            property string scale: ""
            property string offset: ""
            text: scale + offset
            anchors { right: parent.right; bottom: parent.bottom; margins: 5 }
        },

        Text {
            id: yAxisOffset
            property string scale: ""
            property string offset: ""
            text: scale + offset
            anchors { left: parent.left; top: parent.top; margins: 5 }
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

                tipTimer.pos = Vector.point(mouse)
                tipTimer.start()
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
    function precisionOf(n) {
        if (n==0) return 0
        return Math.abs( Math.floor(axisItem.log_10(Math.abs(n))))
    }

    function updateTicks() {
        updateTickNumbers(0)
        updateTickNumbers(1)
    }

    function updateTickNumbers(axis) {
//        console.log("Updating tickNumbers:", axis)
        var ticks, numbers, precision, i
        if (axis == 0) {
            ticks = xAxis.majorTicks
            numbers = xNumbers
            precision = xPrecision

        } else if (axis == 1) {
            ticks = yAxis.majorTicks
            numbers = yNumbers
            precision = yPrecision

        } else {
            return false;
        }
        if (!numbers) return false

        var N = numbers.length

        var diff = ticks.length - N
        if (diff > 0) {
            for (i=0; i<diff; ++i)
                numbers.push(numberCmp.createObject(plotFrame, {'axis': axis}))
        } else if (ticks.length < N) {
            for (i=0; i<-diff; ++i)
                numbers.pop().destroy()
        }

        N = numbers.length

        // Define the display precision
        var mean = Util.mean(ticks)
        var std = Util.std(ticks)
        var ratio = Math.abs(std/mean)
        var offset = 0
        var offsetPrec = 0
        var offsetText = ""
        if (ratio < 0.01) {
            offset = axisItem.offsetFromStd(Util.min(ticks), std)
            offsetPrec = precisionOf(ratio)
            var sign = offset >= 0 ? "+ " : "- "
            offsetText = sign + axisItem.formatReal(Math.abs(offset), offsetPrec, -15, precision)
        }

        // Update .value for all.
//        var n = ticks[0] == 0 ? 1 : 0
//        var minPrec = precisionOf(ticks[n] - offset)
        var deltaPrec = precisionOf((ticks[N-1] - ticks[0])/5)
        var minPrec = precisionOf(Math.min(Math.abs(ticks[0]), Math.abs(ticks[N-1])))
        var maxPrec = Math.max(deltaPrec, minPrec)

        var forceNoExp = maxPrec < precision

        // Work out whether the entire axis should be scaled
        var axisScale = 1
        var scaleText = ""
        if (!forceNoExp && deltaPrec >= precision) {
            var prec = Math.floor(axisItem.log_10(Math.abs(ticks[N-1] - ticks[0])))
            axisScale = Math.pow(10, prec)
            scaleText = "1e" + prec + " "
        }

        for (i=0; i<N; ++i) {
            // TODO: Error: TypeError: Cannot set property 'value' of undefined
            numbers[i].value = ticks[i]
            numbers[i].offset = offset
            numbers[i].scale = axisScale
            if (forceNoExp)
                numbers[i].precision = precision + 1
            else
                numbers[i].precision = precision

        }

        // The following statements ensure that this function is run.
        // It appears that the engine optimizes functions out if they
        // have not obvious external effect...
        if (axis == 0) {
            xNumbers = numbers
            xAxisOffset.scale = scaleText
            xAxisOffset.offset = offsetText
        } else if (axis == 1) {
            yNumbers = numbers
            yAxisOffset.scale = scaleText
            yAxisOffset.offset = offsetText
        }

        return true
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
            id: textItem
            property int axis: -1
            property real value: 0
            property real offset: 0
            property real scale: 1
            property int precision: 3

            text: axisItem.formatReal((value - offset)/scale, precision, -3)
            x: {
                if (axis == 0) { // X axis
                    var newX = (value - minX)/(maxX - minX)
                    if (xAxis.inverted) newX = 1 - newX
                    return plotFrame.width*newX - implicitWidth/2

                } else { // Y axis
                    return -implicitWidth - 5
                }
            }
            y: {
                if (axis == 0) { // X axis
                    return plotFrame.height + 5

                } else { // Y axis
                    var newY = (value - minY)/(maxY - minY)
                    if (!yAxis.inverted) newY = 1 - newY
                    return plotFrame.height*newY - implicitHeight/2
                }
            }
        }
    }
}
