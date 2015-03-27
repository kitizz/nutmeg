Figure {
    id: figure
    Axis {
        handle: "ax"
        
        CanvasPlot {
            CanvasLine {
                xData: [0,1,2,3,6,3,0]
                yData: [3,4,3,2,1,0,-1]
                line { color: "red"; width: 2 }
            }

            CanvasRect {
                rect: Qt.rect(5,5,3,4)
                line { color: "#A81818"; width: 4 }
                fill { color: "#FF5C5C" }
            }
        }
    }
}