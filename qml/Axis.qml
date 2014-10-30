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

    default property alias plotArea: plots.children
    property rect plotRect: Qt.rect(plotFrame.x, plotFrame.y, plotFrame.width, plotFrame.height)

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

    // -------- Layout stuff  -------- //
    /*! \property xLabelMargin
    Set the clearance between the xLabel and the tick numbers. Default: 5
    \qmlOnly
    */
    property real xLabelMargin: 5

    /*! \property yLabelMargin
    Set the clearance between the yLabel and the tick numbers. Default: 5
    \qmlOnly
    */
    property real yLabelMargin: 5

    /*! \property tickNumbersMargin
    Set the clearance between the tick numbers and the axis. Default: 4
    \qmlOnly
    */
    property real tickNumbersMargin: 4

    /*! \property titleMargin
    Set clearance between the title and the axis below. Default: 5
    \qmlOnly
    */
    property real titleMargin: 5

    //----------------------
    //      Internal
    //----------------------
    property real xNumbersMargin: 0
    property real yNumbersMargin: 0
    // The numbers to display on the axes
    property var xNumbers: []
    property var yNumbers: []
    property int xPrecision: 3
    property int yPrecision: 3

    onWidthChanged: {
        updateTickNumbers(0)
        updateTickLocations()
    }
    onHeightChanged: {
        updateTickNumbers(1)
        updateTickLocations()
    }
    xAxis.onMajorTicksChanged: updateTickNumbers(0)
    yAxis.onMajorTicksChanged: updateTickNumbers(1)
//    onLimitsChanged: updateTicks()
    Component.onCompleted: updateTicks()



    margin {
        top: 5; right: 25;
        bottom: 5;
        left: 5
    }

    children: [
        Text {
            id: titleText
            text: axisItem.title
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top; topMargin: margin.top
            }
            font: axisItem.titleFont
            color: axisItem.titleColor
        },

        Text {
            id: xLabelText
            height: (text == "") ? 0 : implicitHeight
            text: xAxis.label
            font: xAxis.labelFont
            color: xAxis.labelColor
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: margin.bottom
            }
        },

        Item {
            id: yLabelContainer
            width: yLabelText.height
            height: width
            Text {
                id: yLabelText
                height: (text == "") ? 0 : implicitHeight
                rotation: -90
                text: yAxis.label
                font: yAxis.labelFont
                color: yAxis.labelColor
                anchors.centerIn: parent
            }

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: margin.left
            }
        },

        Rectangle {
            id: plotFrame
            z: -2
            anchors {
                leftMargin: yLabelMargin + yNumbersMargin + tickNumbersMargin
                rightMargin: axisItem.margin.right
                topMargin: titleMargin;
                bottomMargin: xLabelMargin + xNumbersMargin + tickNumbersMargin
                top: titleText.bottom; bottom: xLabelText.top
                left: yLabelContainer.right; right: parent.right
            }
            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        AxisCanvas2D {
            id: axisCanvas
            x: 0; y: 0; z: -1
            axis: axisItem
            plotRect: axisItem.plotRect
            width: parent.width*scaling
            height: parent.height*scaling
            transform: Scale {
                xScale: 1/axisCanvas.scaling
                yScale: 1/axisCanvas.scaling
            }
        },

        Item {
            id: plots
            anchors.fill: plotFrame
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
        updateTickLocations()
    }

    function updateTickNumbers(axis) {
//        console.log("Updating tickNumbers:", axis)
        var t1 = new Date().getTime()

        var ticks, numbers, precision, i
        if (axis === 0) {
            ticks = xAxis.majorTicks
            numbers = xNumbers
            precision = xPrecision

        } else if (axis === 1) {
            ticks = yAxis.majorTicks
            numbers = yNumbers
            precision = yPrecision

        } else {
            return false;
        }
        if (!numbers) return false

        // Let's do some optimization: align the new set of ticks with the old set
        // Note: Assumes ticks are in ascending order.
        var newNumbers = []
        var ind = 0
        var kept = []

        for (i=0; i<ticks.length; ++i) {
            while (ind < numbers.length && ticks[i] > numbers[ind].value) {
                numbers.splice(ind, 1)[0].destroy()
            }

            if (ind >= numbers.length || ticks[i] < numbers[ind].value) {
                numbers.splice(ind, 0, (numberCmp.createObject(plotFrame, {'axis': axis})))
                ind++
            } else if (ticks[i] === numbers[ind].value) {
                kept.push(ticks[i])
                ind++
            }
        }
        while (numbers.length > ticks.length) {
            numbers.pop().destroy()
        }

        var N = numbers.length

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

//        var t2 = new Date().getTime()
        var maxSize = 0
        var num
        for (i=0; i<N; ++i) {
            // TODO: Error: TypeError: Cannot set property 'value' of undefined
            num = numbers[i]
            num.value = ticks[i]
            num.offset = offset
            num.scale = axisScale
            num.precision = (forceNoExp) ? precision + 1 : precision

            num.updateTheText()

            var size = (axis === 0) ? num.implicitHeight : num.implicitWidth

            if (size > maxSize)
                maxSize = size;
        }
//        var t3 = new Date().getTime()

        // The following statements ensure that this function is run.
        // It appears that the engine optimizes functions out if they
        // have not obvious external effect...
        if (axis === 0) {
//            xNumbers = numbers
            xNumbersMargin = maxSize
            xAxisOffset.scale = scaleText
            xAxisOffset.offset = offsetText
        } else if (axis === 1) {
//            yNumbers = numbers
            yNumbersMargin = maxSize
            yAxisOffset.scale = scaleText
            yAxisOffset.offset = offsetText
        }

//        var t4 = new Date().getTime()
//        console.log("UpdateTicks:", t2-t1, t3-t2, t4-t3)
        return true
    }

    function updateTickLocations() {
        var i
        if (typeof xNumbers !== 'undefined') {
            for (i=0; i<xNumbers.length; ++i)
                xNumbers[i].update()
        }

        if (typeof yNumbers !== 'undefined') {
            for (i=0; i<yNumbers.length; ++i)
                yNumbers[i].update()
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
            id: textItem
            property int axis: -1
            property real value: 0
            property real offset: 0
            property real scale: 1
            property int precision: 3

//            text: axisItem.formatReal((value - offset)/scale, precision, -3)
            font: axis == 0 ? xAxis.tickFont : yAxis.tickFont
            color: axis == 0 ? xAxis.tickTextColor : yAxis.tickTextColor

            function updateTheText() {
                text = axisItem.formatReal((value - offset)/scale, precision, -3)
            }

            function update() {
                // Update position
                var newX, newY
                if (axis == 0) { // X axis
                    newX = (value - minX)/(maxX - minX)
                    if (xAxis.inverted) newX = 1 - newX
                    newX = plotFrame.width*newX - 0.5*implicitWidth

                    newY = plotFrame.height + tickNumbersMargin

                } else { // Y axis
                    newX = -implicitWidth - 5

                    newY = (value - minY)/(maxY - minY)
                    if (!yAxis.inverted) newY = 1 - newY
                    newY = plotFrame.height*newY - 0.5*implicitHeight
                }

                x = newX
                y = newY
            }
        }
    }
}
