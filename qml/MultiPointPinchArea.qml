import QtQuick 2.5

import "Vector.js" as Vector
import "Util.js" as Util

MultiPointTouchArea {
    id: area
    property alias pinch: pinchItem
    property alias mode: modeItem
    readonly property bool touchActive: (p1.pressed || p2.pressed) && (!p3.pressed)
    property bool areaEnabled: true
    property bool debugEnabled: true
    maximumTouchPoints: areaEnabled ? 2 : 0

    property double minDisp: 10 // Minimum displacement of pixels before panning begins
    property double minZoomDelta: 0.1 // Minimum zoom delta before zoom-pan begins

    signal pinchStarted(int pinchMode)
    signal pinchUpdated
    signal pinchFinished

    property alias p1: p1
    property alias p2: p2

    mouseEnabled: false

    touchPoints: [
        TouchPoint { id: p1 },
        TouchPoint { id: p2 },
        TouchPoint { id: p3 }
    ]

    onPressed: {
        if (!areaEnabled) {// Maybe it only has to go here =)
            return
        }

        if (p3.pressed) {
            pinchFinished()
            return
        }

        if (p1.pressed && p2.pressed){// && !p3.pressed) {
            pinch.startCenter.x = (p1.x + p2.x)/2
            pinch.startCenter.y = (p1.y + p2.y)/2
            pinch.startDx = Math.abs(p2.x - p1.x)
            pinch.startDy = Math.abs(p2.y - p1.y)
            pinch.startDistance = Vector.distance(p1, p2)
        } /*else if (p3.pressed) {
            pinchFinished()
        }*/
    }

    property vector2d totalP: Qt.vector2d(0,0)
    property real totalDt: 0
//    property int maxHistory: 4

    onUpdated: {
        // A lot of time being spent in here...
        if (!areaEnabled )//|| p3.pressed)
            return

        if (p1.pressed && p2.pressed && !p3.pressed) {
            //console.log("Internal Pinch Update")
            pinch.previousCenter.x = pinch.center.x
            pinch.previousCenter.y = pinch.center.y
            pinch.center.x = (p1.x + p2.x)/2
            pinch.center.y = (p1.y + p2.y)/2

            pinch.lastTime = pinch.time
            pinch.time = new Date().getTime()

            velTimer.restart()

            if (!pinch.active) {
                if (Math.abs(pinch.scale - 1) > minZoomDelta) {
                    // 0 <= theta <= 90
                    var theta = (Math.atan2(Math.abs(pinch.dy), Math.abs(pinch.dx))/Math.PI*180)
                    console.log("Theta:", theta)
                    if (theta < 45) {
                        pinchStarted(mode.pinchXpan)
                    } else { //if (theta > 45)
                        pinchStarted(mode.pinchYpan)
                    }
//                    else //if (theta > 45)
//                        pinchStarted(mode.pinchYpan)

                } else {
                    var dcenter = pinch.center.minus(pinch.startCenter).length()
                    if (dcenter > minDisp)
                        pinchStarted(mode.pan)
                }
            } else {
                var dt = (pinch.time - pinch.lastTime)
                totalDt += dt
                var dp = pinch.center.minus(pinch.previousCenter)
                totalP = totalP.plus(dp)

                pinch.velocity = totalP.times(1/totalDt)

                totalDt = dt
                totalP = dp
                pinchUpdated() // Emit the signal
            }
        }
    }

    onReleased: {
        if (!p1.pressed && !p2.pressed) { // Yes, this is the correct logic
            pinchFinished() // No more pinching
        }
    }

    onPinchStarted: {
        pinch.pinchMode = pinchMode
        pinch.startCenter.x = (p1.x + p2.x)/2
        pinch.startCenter.y = (p1.y + p2.y)/2
        console.log("Internal pinch started", pinch.pinchMode)
        if (pinchMode == mode.pinchPan ||
                pinchMode == mode.pinchYpan || pinchMode == mode.pinchXpan) {
//            console.log("Mode: Zoom + Pan")
            if (pinchMode == mode.pinchXpan)
                pinch.startDx = Math.abs(p2.x - p1.x)
            if (pinchMode == mode.pinchYpan)
                pinch.startDy = Math.abs(p2.y - p1.y)

            pinch.startDistance = Vector.distance(p1, p2)
        } /*else {
            console.log("Mode: Pan")
        }*/

        pinch.active = true
    }

    onPinchFinished: {
//        console.log("Internal pinch finished")
        pinch.active = false
        pinch.pinchMode = mode.none
        print("Pinch done!!")
    }

    Timer {
        id: velTimer
        repeat: false
        interval: 50
        onTriggered: {
            totalDt = 0
            totalP = Qt.vector2d(0,0)
            pinch.velocity = Qt.vector2d(0,0)
        }
    }

    Item {
        id: pinchItem

        property bool active: false
        property int pinchMode: mode.none

        // Update times
        property real lastTime: new Date().getTime()
        property real time: new Date().getTime()

        // Get the pinch location
        property vector2d startCenter: Qt.vector2d(0,0)
        property vector2d center: Qt.vector2d(0,0)
        property vector2d deltaCenter: Qt.vector2d(center.x-startCenter.x, center.y-startCenter.y)
        property vector2d previousCenter: Qt.vector2d(0,0)
        property vector2d velocity: Qt.vector2d(0,0)

        // Get the pinch size
        property real startDx: 1
        property real dx: p1.x - p2.x
        property real scaleX: Math.abs(dx/startDx)
        property real startDy: 0
        property real dy: p1.y - p2.y
        property real scaleY: Math.abs(dy/startDy)
        property real startDistance: 10
        property real distance: Math.sqrt(dx*dx + dy*dy) // Should I use the Util function?
        property real scale: distance/startDistance
    }

    Rectangle {
        width: 8; height: width
        visible: debugEnabled
        color: "red"
        opacity: p1.pressed ? 1 : 0.1
        x: p1.x; y: p1.y; z: 100
    }

    Rectangle {
        width: 8; height: width
        visible: debugEnabled
        color: "blue"
        opacity: p2.pressed ? 1 : 0.1
        x: p2.x; y: p2.y; z: 100
    }

    Rectangle {
        width: 8; height: width
        visible: debugEnabled
        color: "green"
        opacity: p3.pressed ? 1 : 0.1
        x: p3.x; y: p3.y; z: 100
    }

    // Dynamic/Temporary variables (Changing these does not affect behaviour)
    property real lastT: 0
    property real start: 0
    property double offsetX: 0
    property double offsetY: 0
    property double startContentX: 0
    property double startContentY: 0

    property double offsetZoom: 1.0

    Item {
        id: modeItem
        property int none: 0
        property int pinchPan: 1
        property int pan: 2
        property int pinchXpan: 3
        property int pinchYpan: 4
    }
}
