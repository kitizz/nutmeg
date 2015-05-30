Figure {
    id: figure

    Axis {
        id: ax
        handle: "ax"
        height: parent.height/2
        shareX: "shareAxis"
//        shareY: "shareAxis"

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
        height: parent.height/2
        y: ax.height
        shareX: "shareAxis"
//        shareY: "shareAxis"

        LinePlot {
            handle: "red"
            line { color: "#FF7777"; style: "-"; width: 4 }
        }
    }
}

