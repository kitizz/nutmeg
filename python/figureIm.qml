Figure {
    id: figure
    Axis {
        handle: "ax"
        yAxis.inverted: true
        aspectRatio: 1
        fitPlots: true

        ImagePlot {
            handle: "im"
            // imageScale: 1.0
        }
    }
}