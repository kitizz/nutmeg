Figure {
    id: figure
    Layouts.Column {
        spacing: 0
        Layouts.Repeater {
            count: 3
            Axis {
                handle: "ax"
                // LinePlot {}
                LinePlot {
                    handle: "blue"
                    lineColor: "#7777FF"; lineStyle: "-"; lineWidth: 4
                }
                LinePlot {
                    handle: "red"
                    lineColor: "#FF7777"; lineStyle: "-"; lineWidth: 4
                }
                LinePlot {
                    handle: "green"
                    lineColor: "#77FF77"; lineStyle: ":"; lineWidth: 4
                }
            }
        }
    }
}