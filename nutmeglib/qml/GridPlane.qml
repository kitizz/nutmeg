import Qt3D.Core 2.0
import Qt3D.Render 2.0

Entity {
    id: grid
    property real angle: 0
    property vector3d axis: Qt.vector3d(1,0,0)
    property real offset: 10
    property real minW: -5
    property real maxW: 5
    property real minH: -5
    property real maxH: 5

    PlaneMesh {
        id: mesh
        width: Math.abs(maxW - minW)
        height: Math.abs(maxH - minH)
    }
    Transform {
        id: tran
        matrix: {
            var m = Qt.matrix4x4()
            m.rotate(angle, axis)
            var cx = 0.5*(minW + maxW)
            var cy = 0.5*(minH + maxH)
            m.translate(Qt.vector3d(cx, offset, cy))
            return m
        }
    }
    GridMaterial {
        id: mat
    }

    components: [mesh, mat, tran]
}
