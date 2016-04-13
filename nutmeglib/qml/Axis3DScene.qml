import Qt3D 2.0
import Qt3D.Renderer 2.0

import QtQuick 2.0 as QtQuick2


Entity {
    id: sceneRoot

    property alias cameraPosition: camera.position
    property alias aspectRatio: camera.aspectRatio

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

        onUpVectorChanged: upVector = Qt.vector3d(0,0,1) // A terrible hack...
    }

    Configuration  {
        controlledCamera: camera
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

    CylinderMesh {
        id: mesh
        radius: 2
        length: 4
        rings: 20
        slices: 20
    }

    CuboidMesh {
        id: mesh2
        xExtent: 1
        yExtent: 1
        zExtent: 2
    }

    Transform {
        id: transform
        Rotate {
            angle: 0
            axis: Qt.vector3d(1, 0, 0)
        }
    }

    Material {
        id: myMaterial
        effect : PointEffect{}
    }

    QtQuick2.Component {
        id: sphere
        Entity {
            id: mainEntity
            objectName: "mainEntity"
            components: [ mesh, myMaterial, transform ]
        }
    }

    Entity {
        id: otherEntity
        objectName: "otherEntity"
        components: [ mesh2, myMaterial, transform ]
    }
}
