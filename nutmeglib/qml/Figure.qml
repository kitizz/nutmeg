import QtQuick 2.2
import Nutmeg 0.1
import "Util.js" as Util

/*!
    \ingroup figures
    Figure should be the base item of every Nutmeg figure layout.
    Figures should be be nested in Figures.
*/
FigureBase {
    id: figureBase
    objectName: "figure"
    width: parent.width
    height: parent.height

    handle: "fig"

    property int tabIndex: -1

    function axisAtPoint(point) {
        var axes = getAxisList()
        for (var i=0; i<axes.length; ++i) {
            var p = mapToItem(axes[i], point.x, point.y)
            if (axes[i].contains(Qt.point(p.x, p.y))) {
                return axes[i]
            }
        }
    }

    MouseArea {
        id: mouseArea
        z: 10
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton | Qt.RightButton

        property var currentAxis
        property point startMouse: Qt.point(0,0)
        property rect startLimits: Qt.rect(0,0,0,0)
        property point delta: Qt.point(0,0)
        property bool dragActive: false
        property int mode: -1

        property real zoomSensitivity: 1.0

        onPressed: {
            if ((!(mouse.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))
                 && mouse.button == Qt.MiddleButton) || dragActive) {
                mouse.accepted = false
                return
            }

            currentAxis = axisAtPoint(mouse)
            if (!currentAxis.navigationEnabled)
                return

            dragActive = true

            startMouse.x = mouse.x
            startMouse.y = mouse.y

            startLimits = Util.copyRect(currentAxis.limits)
            if (mouse.modifiers & Qt.ShiftModifier) {
                mode = 1
            } else {
                mode = 0
            }
        }

        onPositionChanged: {
            if (!dragActive || !currentAxis.navigationEnabled) return
            delta.x = mouse.x - startMouse.x
            delta.y = mouse.y - startMouse.y
            // Invert the axes if necessary
            if (currentAxis.xAxis.inverted) delta.x *= -1
            if (currentAxis.yAxis.inverted) delta.y *= -1

            var limits = Util.copyRect(currentAxis.limits)

            // TODO: Add xScale and yScale props to Axis{}
            var xScale = (currentAxis.maxX - currentAxis.minX) / currentAxis.plotRect.width
            var yScale = (currentAxis.maxY - currentAxis.minY) / currentAxis.plotRect.height

            if (mode == 0) { // Pan
                limits.x = startLimits.x - xScale*delta.x
                limits.y = startLimits.y + yScale*delta.y

            } else if (mode == 1) { // Zoom
                var sx = Math.pow(10, delta.x*0.005*zoomSensitivity)
                var sy = Math.pow(10, delta.y*0.005*zoomSensitivity)

                console.log("Scale", sx, sy)
                var dw = (1/sx)*startLimits.width
                var dh = sy*startLimits.height

                if (currentAxis.aspectRatio > 0) {
                    dw = limits.width
                }

                var tx = (startMouse.x - currentAxis.plotRect.x - currentAxis.x)/currentAxis.plotRect.width
                var ty = 1 - (startMouse.y - currentAxis.plotRect.y - currentAxis.y)/currentAxis.plotRect.height
                if (currentAxis.xAxis.inverted) tx = 1 - tx
                if (currentAxis.yAxis.inverted) ty = 1 - ty

                limits.x = startLimits.x - (dw - startLimits.width)*tx
                limits.width = dw
                limits.y = startLimits.y - (dh - startLimits.height)*ty
                limits.height = dh
            }

            currentAxis.limits = limits

        }

        onReleased: {
            dragActive = false
            mode = -1
        }

        onWheel: {
            if (dragActive) return
            currentAxis = axisAtPoint(wheel)
        }
    }

    MultiPointPinchArea {
        id: touchArea
        z: 10
        // By stretching the area beyond the bounds of the parent (and
        // therefore the window) the touch points are better received. It's a
        // weird side effect of how Qml multitouch works
        anchors.centerIn: parent
        width: parent.width*1.5
        height: parent.height*1.5

        mouseEnabled: false
        areaEnabled: !mouseArea.dragActive
        property real panSensitivity: 3.0
        property real scaleLimit: 0.1
        property real startWidth
        property real startHeight
        property real startX
        property real startY
        property real startZoom: 1
        property real minVel: 1/1000 // pxl/ms
        property real minInitVel: 100.0/1000 // pxl/ms
        property real maxVel: 1000.0/1000 // pxl/ms
        property real damping: 5

        property var currentAxis: null
        property point pinchCenter: Qt.point(0,0)

        onPressed: {
            mrTimer.stopTimer()
        }

        onPinchStarted: {
            var p = mapToItem(figureBase, touchArea.p1.x, touchArea.p1.y)
            currentAxis = axisAtPoint(p)
            if (!currentAxis || !currentAxis.navigationEnabled)
                return

            var axisP = mapToItem(currentAxis, touchArea.p1.x, touchArea.p1.y)
            axisP.x -= currentAxis.plotRect.x
            axisP.y -= currentAxis.plotRect.y
            pinchCenter = axisP

            startX = currentAxis.minX
            startY = currentAxis.minY
            startWidth = currentAxis.maxX - currentAxis.minX
            startHeight = currentAxis.maxY - currentAxis.minY

            if (currentAxis.objectName === "axis3d") {
                startZoom = currentAxis.zoom
            }
        }

//        property var lastTimeUpdate: new Date().getTime()
        onPinchUpdated: {
            if (!currentAxis || !currentAxis.navigationEnabled)
                return

            if (currentAxis.objectName === "axis2d") {
                pinch2d()
            }
            if (currentAxis.objectName === "axis3d") {
                pinch3d()
            }
        }

        function pinch2d() {
            var dx = pinch.center.x - pinch.previousCenter.x
            var dy = pinch.center.y - pinch.previousCenter.y
            // Invert if necessary
            if (currentAxis.xAxis.inverted) dx *= -1
            if (currentAxis.yAxis.inverted) dy *= -1

            var xScale = (currentAxis.maxX - currentAxis.minX) / currentAxis.plotRect.width
            var yScale = (currentAxis.maxY - currentAxis.minY) / currentAxis.plotRect.height
            dx *= xScale*panSensitivity // TODO: Acceleration goes here
            dy *= yScale*panSensitivity

            var newLimits = Qt.rect(currentAxis.limits.x, currentAxis.limits.y,
                                    currentAxis.limits.width, currentAxis.limits.height)

            if (pinch.pinchMode == mode.pan) {
                newLimits.x -= dx
                newLimits.y -= dy

            } else if (pinch.pinchMode == mode.pinchPan || pinch.pinchMode == mode.pinchXpan || pinch.pinchMode == mode.pinchYpan) {
                // If there is ZOOMING happening
                // Relative scale center:
                var tx = pinchCenter.x/currentAxis.plotRect.width
                var ty = 1 - pinchCenter.y/currentAxis.plotRect.height

                var useX
                if (currentAxis.aspectRatio > 0) {
                    // We need to zoom both XY at once...
                    useX = pinch.scaleX > pinch.scaleY
                } else if (pinch.pinchMode == mode.pinchXpan) {
                    useX = true
                } else if (pinch.pinchMode == mode.pinchYpan) {
                    useX = false
                }

                if (useX) {
                    var scaleX = Math.max( Math.min(1/pinch.scaleX, 1/scaleLimit), scaleLimit )
                    var w = currentAxis.maxX - currentAxis.minX
                    var nw = scaleX * startWidth
                    newLimits.x -= (nw - w)*tx
                    newLimits.width = nw

                } else {
                    var scaleY = Math.max( Math.min(1/pinch.scaleY, 1/scaleLimit), scaleLimit )
                    var h = currentAxis.maxY - currentAxis.minY
                    var nh = scaleY * startHeight
                    newLimits.y -= (nh - h)*ty
                    newLimits.height = nh
                }

                var center = Qt.point(tx, ty)
                newLimits = currentAxis.maintainAspectRatio(newLimits, center)
            }

            currentAxis.limits = newLimits
//            currentAxis.updateTickLocations()
        }

        function pinch3d() {
//            print("Pinch3d mode:", pinch.pinchMode)

            var dx = pinch.center.x - pinch.previousCenter.x
            var dy = pinch.center.y - pinch.previousCenter.y
            if (pinch.pinchMode == mode.pan) {
                if (figureBase.keyModifiers & Qt.ShiftModifier) {
                    // Translate
                    var center = currentAxis.center
                    var az = currentAxis.azimuth
                    var al = currentAxis.altitude
                    var dxaxis = Qt.vector3d(Math.sin(az), -Math.cos(az), 0)
                    var dyaxis = Qt.vector3d(-Math.cos(az)*Math.sin(al), -Math.sin(az)*Math.sin(al), Math.cos(al))

                    var ddx = dx*currentAxis.zoom/500
                    var ddy = dy*currentAxis.zoom/500
                    currentAxis.center = Qt.vector3d(center.x + ddx*dxaxis.x + ddy*dyaxis.x,
                                                     center.y + ddx*dxaxis.y + ddy*dyaxis.y,
                                                     center.z + ddx*dxaxis.z + ddy*dyaxis.z)

                } else {
                    // Orbit
                    // 400 px / 90 deg
                    var rate = (Math.PI*0.25)/50
                    currentAxis.azimuth -= dx*rate
                    currentAxis.altitude += 0.5*dy*rate
                    if (currentAxis.altitude > Math.PI/2)
                        currentAxis.altitude = Math.PI/2
                    else if (currentAxis.altitude < -Math.PI/2)
                        currentAxis.altitude = -Math.PI/2
                }
            } else if (pinch.pinchMode == mode.pinchXpan || pinch.pinchMode == mode.pinchYpan || pinch.pinchMode == mode.pinchPan) {
//                var sx = pinch.scaleX
//                var sy = pinch.scaleY
                var scale = 1/pinch.scale
                currentAxis.zoom = startZoom * scale
//                print("New zoom:", currentAxis.zoom, "(", scale, ")")
            }
        }

        onPinchFinished: {
            if (!currentAxis || !currentAxis.navigationEnabled)
                return
            if (pinch.pinchMode != mode.pan)
                return
            mrTimer.lastT = new Date().getTime()

            mrTimer.velX = pinch.velocity.x
            if (Math.abs(pinch.velocity.x) > maxVel)
                mrTimer.velX = Util.sign(pinch.velocity.x)*maxVel

            mrTimer.velY = pinch.velocity.y
            if (Math.abs(pinch.velocity.y) > maxVel)
                mrTimer.velY = Util.sign(pinch.velocity.y)*maxVel

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

                velX -= velX*touchArea.damping*dt*0.001
                velY -= velY*touchArea.damping*dt*0.001

                var currentAxis = touchArea.currentAxis
                var newLimits = Qt.rect(currentAxis.limits.x, currentAxis.limits.y,
                                        currentAxis.limits.width, currentAxis.limits.height)

                // Use "city block" instead of "euclidean" - slightly faster and simpler
                var vMax = Math.max(Math.abs(velX), Math.abs(velY));
                if (vMax < touchArea.minVel) {
                    stopTimer() // Velocity is too tiny, stop updating
                }

                var xScale = (currentAxis.maxX - currentAxis.minX) / currentAxis.plotRect.width
                var yScale = (currentAxis.maxY - currentAxis.minY) / currentAxis.plotRect.height


                var sens = touchArea.panSensitivity
                vx *= xScale*sens
                vy *= yScale*sens

                newLimits.x -= vx
                newLimits.y += vy

                currentAxis.limits = newLimits

                lastT = ct
            }

            function startTimer() {
                // Use "city block" instead of "euclidean" - slightly faster and simpler
                var lim = touchArea.currentAxis.limits
                var vMax = Math.max(Math.abs(velX)*lim.width, Math.abs(velY)*lim.height);
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
