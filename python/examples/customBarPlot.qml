import "Util.js" as Util
Figure {
    id: figure
    Axis {
        handle: "ax"
        
        CanvasPlot {
            id: canvas
            handle: "barPlot"

            property real barWidth: 1
            property real spacing: 1

            property var data: []

            // Don't touch this var...
            property var bars: []

            onDataChanged: {
                while (bars.length < data.length) {
                    var newBar = bar.createObject(canvas)
                    bars.push(newBar)
                }
                while (bars.length > data.length) {
                    bars.pop().destroy()
                }

                var i
                for (i=0; i < bars.length; ++i) {
                    bars[i].pos = i
                    bars[i].value = data[i]
                }

                updateShapes()
            }

            Component.onCompleted: {
                registerProperties({"data": "data", "barWidth":"barWidth", "spacing":"spacing"})
            }
            
            Component {
                id: bar
                CanvasRect {
                    id: rectShape
                    property real pos: 0
                    property real value: 0

                    readonly property real max: Math.abs(value)
                    readonly property real min: value < 0 ? value : 0
                    
                    rect: Qt.rect((pos+0.5)*canvas.spacing-canvas.barWidth*0.5, min, canvas.barWidth, max)
                    line { color: "#A81818"; width: 4 }
                    fill { color: "#FF5C5C" }

                    Component.onCompleted: console.log("Successfully created", rectShape, canvas.barWidth)
                }
            }

        }
    }
}