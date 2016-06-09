import QtQuick 2.4
import Nutmeg 0.1

import QtQuick.Scene3D 2.0
import Qt3D.Render 2.0

import "Util.js" as Util

Axis3DBase {
    id: axisItem
    handle: "ax"
    objectName: "axis3d"
    width: parent.width
    height: parent.height

    default property alias plotArea: plots.children
    readonly property alias plotMargins: pltMarg

//    readonly property var cameraPosition: rootEntity.cameraPosition
    property real azimuth: Math.PI/4
    property real altitude: Math.PI/4
    property real zoom: 30.0
    property vector3d center: Qt.vector3d(0,0,0)

    rootEntity: sceneEntity

//    preferredPlotRect: Qt.rect(plotFrame.x, plotFrame.y, plotFrame.width, plotFrame.height)

//    AxisMargins {
//        id: pltMarg
//        top: titleMargin + titleText.y + titleText.height
//        left: yLabelMargin + yNumbersMargin + tickNumbersMargin + yLabelContainer.x + yLabelContainer.width
//        bottom: xLabelMargin + xNumbersMargin + tickNumbersMargin + margin.bottom + xLabelText.height
//        right: axisItem.margin.right
//    }
    AxisMargins {
        id: pltMarg
        top: 5
        left: 5
        bottom: 5
        right: 5
    }

    onAddedPlot: {
        console.log(plot.entity, "added to", rootEntity)
//        console.log(Util.dir(plot.entity))
//        plot.rootEntity = rootEntity
    }

    onRemovedPlot: {
        plot.rootEntity = null
    }

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

    children: [
        Rectangle {
            id: plotFrame
            z: -2
            anchors {
                fill: parent
                topMargin: plotMargins.top
                leftMargin: plotMargins.left
                bottomMargin: plotMargins.bottom
                rightMargin: plotMargins.right
            }
            color: "white"
            border { color: "#AAAAAA"; width: 1 }
        },

        Scene3D {
            id: axisCanvas
//            x: 0; y: 0
            z: -1
            anchors.fill: plotFrame
            anchors.margins: 1
            aspects: "input"
            Axis3DScene {
                id: sceneEntity
                axis: axisItem
                aspectRatio: axisCanvas.width/axisCanvas.height
                azimuth: axisItem.azimuth
                altitude: axisItem.altitude
                zoom: axisItem.zoom
                center: axisItem.center
            }
        },

        Item {
            id: plots
            anchors.fill: plotFrame
        }
    ]
}

