import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    property real minX: -1
    property real maxX: 1
    property real minY: -1
    property real maxY: 1
    property real minZ: -1
    property real maxZ: 1

    // Grid axis
    GridPlane {
        id: xpos
        offset: -maxX
        minW: minY
        maxW: maxY
        minH: minZ
        maxH: maxZ
        angle: 90
        axis: Qt.vector3d(0,0,1)
    }

    GridPlane {
        id: xneg
        offset: minX
        minW: -maxY
        maxW: -minY
        minH: minZ
        maxH: maxZ
        angle: -90
        axis: Qt.vector3d(0,0,1)
    }

    GridPlane {
        id: ypos
        offset: -maxY
        minW: -minX
        maxW: -maxX
        minH: minZ
        maxH: maxZ
        angle: 180
        axis: Qt.vector3d(0,0,1)
    }

    GridPlane {
        id: yneg
        offset: minY
        minW: maxX
        maxW: minX
        minH: minZ
        maxH: maxZ
        angle: 0
        axis: Qt.vector3d(0,0,1)
    }

    GridPlane {
        id: zpos
        offset: -maxZ
        minW: minX
        maxW: maxX
        minH: minY
        maxH: maxY
        angle: -90
        axis: Qt.vector3d(1,0,0)
    }

    GridPlane {
        id: zneg
        offset: minZ
        minW: maxX
        maxW: minX
        minH: -minY
        maxH: -maxY
        angle: 90
        axis: Qt.vector3d(1,0,0)
    }
}
