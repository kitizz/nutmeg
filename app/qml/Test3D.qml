import Nutmeg 0.1

Figure {
    id: figure

    Axis3D {
        handle: "ax3d"

        width: parent.width
        anchors.right: parent.right

        PointCloud {
            handle: "pc"
        }

        RayCloud {
            handle: "rc"
            color: "#CC4444FF"
            linewidth: 2
        }
    }
}

