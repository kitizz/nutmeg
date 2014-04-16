import QtQuick 2.0
import QtQml 2.1
import Graphr 1.0

LinePlotBase {
    id: linePlot
    width: parent.width
    height: parent.height
    handle: "data"

    property var figure: axis.figure

    signal updateTip(point mouse)
    signal clearTips()

    onUpdateTip: {
        var index = nearestDataTo(itemToData(mouse))

        var info
        if (infos.length == 0) {
            info = pointInfo.createObject(axis, {"index": index})
            axis.dataLimitsChanged.connect(info.updatePos)
            axis.limitsChanged.connect(info.updatePos)
            linePlot.widthChanged.connect(info.updatePos)
            linePlot.heightChanged.connect(info.updatePos)
            infos.push(info)
        } else {
            info = infos[0]
            info.index = index
        }
    }

    onClearTips: {
        console.log("Deleting:", infos)
        for (var i=0; i<infos.length; ++i) {
            infos[i].destroy()
        }
        infos = []
    }

    canvas: lineCanvas
    LinePlotCanvas {
        id: lineCanvas
        width: parent.width*scaling
        height: parent.height*scaling
        transform: Scale {
            xScale: 1/lineCanvas.scaling
            yScale: 1/lineCanvas.scaling
        }
    }

    Component {
        id: pointInfo
        Item {
            id: info
            visible: indexValid && pointVisible
            property point pos: indexValid ? Qt.point(xData[index], yData[index]) : Qt.point(0,0)
            property int index: -1
            property bool indexValid: index >= 0 && index < xData.length
            property bool pointVisible: pos.x >= axis.minX && pos.x <= axis.maxX &&
                                        pos.y >= axis.minY && pos.y <= axis.maxY

            onPosChanged: updatePos()

            function updatePos() {
                if (!indexValid) return
                var framePos = dataToItem(pos)
                framePos = parent.mapFromItem(linePlot, framePos.x, framePos.y)
                x = framePos.x
                y = framePos.y
            }

            Rectangle {
                radius: 3
                x: -radius; y: -radius
                width: 2*radius; height: 2*radius
                border { width: 1 }
            }

            Rectangle {
                id: tip
                opacity: 0.75
                width: childrenRect.width + 2*margin; height: childrenRect.height + 2*margin

                x: 5; y: -height - 5

                property real margin: 3

                border { color: "black"; width: 1 }
                Column {
                    id: col
                    x: tip.margin; y: tip.margin
                    property real fontSize: 10
                    Text { text: "Ind: " + info.index; font.pointSize: col.fontSize}
                    Text { text: "x: " + xData[info.index].toPrecision(3); font.pointSize: col.fontSize }
                    Text { text: "y: " + yData[info.index].toPrecision(3); font.pointSize: col.fontSize }
                }
            }
        }
    }
    property var infos: []
}
