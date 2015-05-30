Figure {
    id: fig
    Layouts.Column {
        spacing: 0.05*fig.height
        Axis {
            handle: "axis1"
            Layout.preferredHeight: 0.65*fig.height
            LinePlot {
                handle: "data"
                line { color: "#7777FF"; width: 4 }
            }
        }
        Axis {
            handle: "axis2"
            Layout.preferredHeight: 0.3*fig.height
            LinePlot {
                handle: "data"
                line { color: "#FF7777"; width: 4 }
            }
        }
    }
}
