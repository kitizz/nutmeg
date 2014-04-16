import QtQuick 2.2
import Graphr 1.0

FigureBase {
    objectName: "figure"
    width: parent.width
    height: parent.height

    handle: "fig"

    property int tabIndex: -1
    property var guiItem: null

    MultiPointPinchArea {
        id: touchArea
        z: 10
        anchors.fill: parent
        mouseEnabled: false
//            areaEnabled: false
        property real panSensitivity: 3.0
        property real scaleLimit: 0.1
        property real startWidth
        property real startHeight
        property real minVel: 1.0/1000 // Pxl/ms
        property real minInitVel: 50.0/1000 // Pxl/ms
        property real damping: 5

        property var currentAxis: null

        onPressed: {
            mrTimer.stopTimer()
        }

        onPinchStarted: {
            var axes = getAxisList()
            for (var i=0; i<axes.length; ++i) {
                var p = mapToItem(axes[i], touchArea.p1.x, touchArea.p1.y)
                console.log("Finding axis under mouse p1", p.x, p.y)
                console.log("Axis size", axes[i].x, axes[i].y, axes[i].width, axes[i].height)
                if (axes[i].contains(Qt.point(p.x, p.y))) {
                    currentAxis = axes[i]
                    break
                }
            }

            startWidth = currentAxis.maxX - currentAxis.minX
            startHeight = currentAxis.maxY - currentAxis.minY
        }

        onPinchUpdated: {
            console.log("Start Update")
            // Outsource this to c++
            // Inputs:
            //      C++ `pinch` object
            //      CurrentAxis
            //
            var dx = pinch.center.x - pinch.previousCenter.x
            var dy = pinch.center.y - pinch.previousCenter.y

            var xScale = (currentAxis.maxX - currentAxis.minX) / currentAxis.plotRect.width
            var yScale = (currentAxis.maxY - currentAxis.minY) / currentAxis.plotRect.height
            dx *= xScale*panSensitivity // TODO: Acceleration goes here
            dy *= yScale*panSensitivity

            var newLimits = Qt.rect(currentAxis.limits.x - dx, currentAxis.limits.y + dy,
                                    currentAxis.limits.width, currentAxis.limits.height)
//            var newMinX = currentAxis.minX - dx
//            var newMaxX = currentAxis.maxX - dx
//            var newMinY = currentAxis.minY - dy
//            var newMaxY = currentAxis.maxY - dy
//            currentAxis.minX -= dx
//            currentAxis.maxX -= dx
//            currentAxis.minY += dy
//            currentAxis.maxY += dy

//            var pinchCenter = mapToItem(currentAxis, pinch.center.x, pinch.center.y)
            var pinchCenter = mapToItem(currentAxis, touchArea.p1.x, touchArea.p1.y)

            // If there is ZOOMING happening
            if (pinch.pinchMode == mode.pinchPan || pinch.pinchMode == mode.pinchXpan) {
                var scale = Math.max( Math.min(1/pinch.scaleX, 1/scaleLimit), scaleLimit )
                var dw = scale * startWidth
                var w = currentAxis.maxX - currentAxis.minX
                var t = pinchCenter.x/currentAxis.plotRect.width
//                    console.log("dw:", dw, " t:", t)
                newLimits.x -= (dw - w)*t
                newLimits.width = dw
//                newMaxX += (dw - w)*(1-t)
            }
            if (pinch.pinchMode == mode.pinchPan || pinch.pinchMode == mode.pinchYpan) {
                var scale = Math.max( Math.min(1/pinch.scaleY, 1/scaleLimit), scaleLimit )
                var dh = scale * startHeight
                var h = currentAxis.maxY - currentAxis.minY
                var t = 1 - pinchCenter.y/currentAxis.plotRect.height
//                    console.log("dw:", dh, " t:", t)
                newLimits.y -= (dh - h)*t
                newLimits.height = dh
//                newMaxY += (dh - h)*(1-t)
            }
            currentAxis.limits = newLimits
        }

        onPinchFinished: {
            mrTimer.lastT = new Date().getTime()
            mrTimer.velX = pinch.velocity.x
            mrTimer.velY = pinch.velocity.y
            mrTimer.startTimer()
        }

        Timer {
            id: mrTimer
            property real lastT: 0
            property real velX: 0
            property real velY: 0
            repeat: true
            interval: 20
            onTriggered: {
                var ct = new Date().getTime()
                var dt = (ct - lastT)

                var vx = velX*dt
                var vy = velY*dt
//                console.log("Vel:", velX, velY, dt, touchArea.damping*dt*0.001)

                velX -= velX*touchArea.damping*dt*0.001
                velY -= velY*touchArea.damping*dt*0.001

                // Use "city block" instead of "euclidean" - slightly faster and simpler
                var vMax = Math.max(Math.abs(velX), Math.abs(velY));
                if (vMax < touchArea.minVel) {
                    stopTimer() // Velocity is too tiny, stop updating
                }

                var currentAxis = touchArea.currentAxis
                var xScale = (currentAxis.maxX - currentAxis.minX) / currentAxis.plotRect.width
                var yScale = (currentAxis.maxY - currentAxis.minY) / currentAxis.plotRect.height
                var sens = touchArea.panSensitivity
                vx *= xScale*sens
                vy *= yScale*sens

                currentAxis.minX -= vx
                currentAxis.maxX -= vx
                currentAxis.minY += vy
                currentAxis.maxY += vy

                lastT = ct
            }

            function startTimer() {
                // Use "city block" instead of "euclidean" - slightly faster and simpler
                var vMax = Math.max(Math.abs(velX), Math.abs(velY));
                if (vMax > touchArea.minInitVel)
                    start()
                else
                    stopTimer()
            }

            function stopTimer() {
                velX = 0;
                velY = 0;
                stop();
            }
        }
    }
}
