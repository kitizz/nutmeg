import Nutmeg 1.0

Figure {
    id: figure

    Grid {
        anchors.fill: parent
        rows: 2
        layoutDirection: "Vertical"
        rowWeights: [0.5, 0.5]

        Axis {
            id: ax
            handle: "ax"
            shareX: "shareAxis"
    //        shareY: "shareAxis"
            height: parent.height/2

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

        Axis {
            id: ax2
            handle: "ax2"
            shareX: "shareAxis"
            height: parent.height - y
            y: ax.height

            LinePlot {
                handle: "red"
                line { color: "#FF7777"; style: "-"; width: 4 }
            }
        }
    }
}

