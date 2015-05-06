Figure {
    id: figure

//    Axis3D {
//        handle: "ax3d"

//        width: parent.width
//        anchors.right: parent.right

//        PointCloud {
//            handle: "pc"
//        }
//    }

    Axis {
        handle: "ax"
//        width: parent.width/2
//        xAxis.majorTicks: [0,2,5,10,20]
//        xAxis.onMajorTicksChanged: console.log("Major X:", xAxis.majorTicks, yAxis.majorTicks)
//        yAxis.onMajorTicksChanged: console.log("Major Y:", yAxis.majorTicks)
        LinePlot {
            handle: "blue"
            line { color: "#7777FF"; style: "--"; width: 4 }
        }
        LinePlot {
            handle: "red"
            line { color: "#FF7777"; style: "-"; width: 4 }
        }
        LinePlot {
            handle: "green"
            line { color: "#77FF77"; style: ":"; width: 4 }
        }
    }
}
