Figure {
    id: figure

    Axis3D {
        handle: "ax3d"

        width: parent.width
        anchors.right: parent.right

        PointCloud {
            handle: "pc"
        }
    }
}

