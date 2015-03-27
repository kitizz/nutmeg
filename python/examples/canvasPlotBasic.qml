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
                x: 5; y: 5
                width: 3; height: 4
                line { color: "#A81818"; width: 4 }
                fill { color: "#FF5C5C" }
            }

            CanvasText {
                text: "Testing how good text is"
                x: 5
                y: 2
                font.pointSize: 30
            }
        }
    }
}