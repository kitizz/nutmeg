import QtQuick 2.0
import Nutmeg 1.0
import QtQuick.Layouts 1.0

import "Vector.js" as Vector
import "Util.js" as Util

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

    // -------- Layout stuff  -------- //
    /*! \property xLabelMargin
    Set the clearance between the xLabel and the tick numbers. Default: 5
    \qmlOnly
    */
//    property real xLabelMargin: 5

    /*! \property yLabelMargin
    Set the clearance between the yLabel and the tick numbers. Default: 5
    \qmlOnly
    */
//    property real yLabelMargin: 5

    /*! \property tickNumbersMargin
    Set the clearance between the tick numbers and the axis. Default: 4
    \qmlOnly
    */
//    property real tickNumbersMargin: 4

    /*! \property titleMargin
    Set clearance between the title and the axis below. Default: 5
    \qmlOnly
    */
//    property real titleMargin: 5

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
//    property int xPrecision: 3
//    property int yPrecision: 3

//    onWidthChanged: {
//        updateTickNumbers(0)
//        updateTickLocations()
//    }
//    onHeightChanged: {
//        updateTickNumbers(1)
//        updateTickLocations()
//    }
//    xAxis.onMajorTicksChanged: updateTickNumbers(0)
//    yAxis.onMajorTicksChanged: updateTickNumbers(1)
////    onLimitsChanged: updateTicks()
//    Component.onCompleted: updateTicks()



    margin {
        top: 5; right: 25;
        bottom: 5;
        left: 5
    }

    children: [
//        Text {
//            id: titleText
//            text: axisItem.title
//            anchors {
//                horizontalCenter: parent.horizontalCenter
//                top: parent.top; topMargin: margin.top
//            }
//            font: axisItem.titleFont
//            color: axisItem.titleColor
//        },

//        Text {
//            id: xLabelText
//            height: (text == "") ? 0 : implicitHeight
//            text: xAxis.label
//            font: xAxis.labelFont
//            color: xAxis.labelColor
//            anchors {
//                horizontalCenter: parent.horizontalCenter
//                bottom: parent.bottom
//                bottomMargin: margin.bottom
//            }
//        },

//        Item {
//            id: yLabelContainer
//            width: yLabelText.height
//            height: width
//            Text {
//                id: yLabelText
//                height: (text == "") ? 0 : implicitHeight
//                rotation: -90
//                text: yAxis.label
//                font: yAxis.labelFont
//                color: yAxis.labelColor
//                anchors.centerIn: parent
//            }

//            anchors {
//                verticalCenter: parent.verticalCenter
//                left: parent.left
//                leftMargin: margin.left
//            }
//        },

        Rectangle {
            id: plotFrame
            z: -2
            x: plotRect.x
            y: plotRect.y
            width: plotRect.width
            height: plotRect.height

//            anchors {
//                fill: parent
//                topMargin: plotMargins.top
//                leftMargin: plotMargins.left
//                bottomMargin: plotMargins.bottom
//                rightMargin: plotMargins.right
//            }
            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        AxisCanvas2D {
            id: axisCanvas
            x: 0; y: 0; z: -1
            axis: axisItem
//            plotRect: axisItem.plotRect
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
