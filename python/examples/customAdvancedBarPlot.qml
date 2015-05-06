import "Util.js" as Util
Figure {
    id: figure
    Axis {
        id: ax
        handle: "ax"
        height: figure.height - legend.height

        margin.bottom: 0

        xAxis.xMajorTicks: canvas.groupCenters
        // {
        //     var locs = {}
        //     var i
        //     for (i=0; i<canvas.groupLabels.length; ++i) {
        //         locs[canvas.groupLabels[i]] = 
        //     }
        // }
        
        CanvasPlot {
            id: canvas
            handle: "barPlot"

            // Play with these
            property real barWidth: 1
            property real spacing: 0
            property real groupSpacing: 1
            property var groupLabels: []
            // Definitely use this
            property var data: []

            // Don't touch this var...
            property var bars: []
            property var groupCenters: []

            readonly property var colors: ["#EF5762", "#9E5DA4", "#589BDA", "#8CCF5C", "#F9A454", "#818181", "#DA7CAF"]

            onDataChanged: {
                console.log("New Data:", data)
                while (bars.length < data.length) {
                    bars.push([])
                }
                while (bars.length > data.length) {
                    var group = bars.pop()
                    while (group.length > 0)
                        group.pop().destroy()
                }

                var i, index = 0
                for (i=0; i < bars.length; ++i) {
                    index += updateGroup(bars[i], data[i], i, index)
                }
            }

            function updateGroup(barGroup, dataGroup, groupIndex, index) {
                while (barGroup.length < dataGroup.length) {
                    barGroup.push(bar.createObject(canvas))
                }
                while (barGroup.length > dataGroup.length) {
                    barGroup.pop().destroy()
                }

                var i
                var groupCenter = 0
                for (i=0; i < barGroup.length; ++i) {
                    barGroup[i].pos = (i+index)*(canvas.barWidth + canvas.spacing) + groupIndex*canvas.groupSpacing
                    groupCenter += barGroup[i].pos
                    barGroup[i].value = dataGroup[i]
                    barGroup[i].fill.color = colors[i%colors.length]
                }
                groupCenters[groupIndex] = groupCenter/i
                return i
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
                    // property int groupIndex: 0

                    readonly property real max: Math.abs(value)
                    readonly property real min: value < 0 ? value : 0
                    
                    x: pos
                    y: min
                    width: canvas.barWidth
                    height: max

                    line { color: Qt.darker(fill.color, 2); width: 4 }
                    fill { color: "#FF5C5C" }

                    Component.onCompleted: console.log("Successfully created", rectShape, canvas.barWidth)
                }
            }

        }
    }

    Axis {
        id: legend
        handle: "legend"
        y: ax.height
        height: legendGrid.implicitHeight + plotMargins.top + plotMargins.bottom
        width: legendGrid.implicitWidth + plotMargins.left + plotMargins.right
        anchors.horizontalCenter: parent.horizontalCenter

        // navigationEnabled: false
        // This makes the plot to screen transform 1:1
        xAxis.majorTicks: null
        yAxis.majorTicks: null
        yAxis.inverted: true
        aspectRatio: 1
        // minX: 0; maxX: legendGrid.implicitWidth
        minY: 0; maxY: legendGrid.implicitHeight
        border.width: 0

        property size boxSize: "20x20"
        property font labelFont: Qt.font({family:"Arial", pointSize: 24, weight: Font.Normal})

        property var labels: []

        Component.onCompleted: {
            registerProperties({"boxSize": "boxSize", "labelFont": "labelFont", "labels": "labels"})
        }

        // // Don't touch this
        property var keys: []

        onLabelsChanged: {
            if (typeof keys === "undefined") return

            while (keys.length < labels.length) {
                var newBox = box.createObject(legendGrid)
                var newLabel = text.createObject(legendGrid)
                // console.log("TextSize:", newLabel.x, newLabel.y, newLabel.width, newLabel.height)
                keys.push({"box": newBox, "label": newLabel})
            }
            while (keys.length > labels.length) {
                var key = keys.pop()
                key["box"].destroy()
                key["label"].destroy()
            }

            var i
            for (i=0; i<labels.length; ++i) {
                var c = canvas.colors[i%canvas.colors.length]
                keys[i].box.fill.color = c
                keys[i].box.line.color = Qt.darker(c, 2)

                keys[i].label.text = labels[i]
            }
        }

        CanvasPlot {
            id: legendPlot

            Grid {
                id: legendGrid
                columns: 6
                spacing: 5
                // width: legend.width
            }

            Component {
                id: box
                CanvasRect {
                    id: rect
                    // property alias color: fill.color
                    line.width: 2
                    width: legend.boxSize.width
                    height: legend.boxSize.height
                }
            }

            Component {
                id: text
                CanvasText {
                    id: label
                    width: textScreenSize.width
                    height: 1
                    font {
                        family: legend.labelFont.family
                        pointSize: legend.labelFont.pointSize
                    }
                }
            }
        }
    }
}