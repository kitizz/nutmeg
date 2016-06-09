import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QtQuick2

import Nutmeg 0.1


Entity {
    id: sceneRoot

    property var axis: null

    property alias cameraPosition: camera.position
    property alias aspectRatio: camera.aspectRatio

    property real zoom: 30.0
    property vector3d center: Qt.vector3d(0.0, 0.0, 0.0)
    property real azimuth: Math.PI/4
    property real altitude: Math.PI/4

    onZoomChanged: updateCamera()
    onCenterChanged: updateCamera()
    onAzimuthChanged: updateCamera()
    onAltitudeChanged: updateCamera()

    function updateCamera() {
        var dx, dy, dz, az = azimuth, al = altitude
        var maxAl = Math.PI/2 - Math.PI/180
        if (al > maxAl) al = maxAl
        if (al < -maxAl) al = -maxAl
        dx = zoom*Math.cos(az)*Math.cos(al)
        dy = zoom*Math.sin(az)*Math.cos(al)
        dz = zoom*Math.sin(al)

        camera.viewCenter = center
        camera.position = Qt.vector3d(center.x + dx, center.y + dy, center.z + dz)
        camera.upVector = Qt.vector3d(0,0,1)
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 1
        nearPlane : 0.1
        farPlane : 10000.0
        upVector: Qt.vector3d( 0.0, 0.0, 1.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
        left: -9; right: 9
        top: 16; bottom: -16
    }

    Configuration  {
        controlledCamera: camera
    }

    Grid3D {
        minX: axis ? axis.minX : 0
        maxX: axis ? axis.maxX : 0
        minY: axis ? axis.minY : 0
        maxY: axis ? axis.maxY : 0
        minZ: axis ? axis.minZ : 0
        maxZ: axis ? axis.maxZ : 0
    }

    components: [
        FrameGraph {
            id : external_forward_renderer
//            activeFrameGraph : ForwardRenderer {
//                camera: camera
//                viewportRect: Qt.rect(-1, -1, 1, 1) // From Top Left
//                clearColor: "white"
//            }
            activeFrameGraph: Viewport {
                id: viewport
                rect: Qt.rect(0, 0, 1, 1) // From Top Left
                clearColor: "white"

                ClearBuffer {
                    buffers : ClearBuffer.ColorDepthBuffer

                    CameraSelector {
                        id : cameraSelector
                        camera: camera


                    }
                }
            }
        }
    ]
}
