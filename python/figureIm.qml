Figure {
    id: figure
    Axis {
        handle: "ax"
        yAxis.inverted: true
        aspectRatio: 1
        fitPlots: true

        ImagePlot {
            handle: "im"
        }

        LinePlot {
            id: plot
            handle: "data"
            line { width: 6 }
        }

        LinePlot {
            id: plot2
            handle: "data2"
            line.width: plot.line.width
            xData: [0, 1000]
            yData: [0, 100]
        }
    }
}