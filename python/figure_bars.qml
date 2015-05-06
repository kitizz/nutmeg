Figure {
    id: figure
    Axis {
        handle: "ax"
        maxY: 2.5
        minY: -2.5
        LinePlot {
            handle: "bluePlot"
            line { color: "#7777FF"; style: "-"; width: 4 }
        }
        LinePlot {
            handle: "redPlot"
            property var bars: []

            line { color: "#FF7777"; style: "-"; width: 4 }

            Component.onCompleted: {
                console.log("redPlot.onCompleted")
                registerProperties({"bars": "bars"})
            }

            onBarsChanged: {
                var xD = [], yD = []
                for (var i=0; i<bars.length; ++i) {
                    xD.push(i - 0.4)
                    xD.push(i - 0.4)
                    xD.push(i + 0.4)
                    xD.push(i + 0.4)
                    xD.push(i - 0.4)

                    yD.push(0)
                    yD.push(bars[i])
                    yD.push(bars[i])
                    yD.push(0)
                    yD.push(0)
                }
                xData = xD
                yData = yD
            }
        }
        LinePlot {
            handle: "greenPlot"
            line { color: "#66FF66"; style: ":"; width: 4 }
        }
    }
}