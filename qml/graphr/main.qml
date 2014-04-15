import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Graphr 1.0
import "Util.js" as Util
import "Layouts" as Layouts
//import "Controls"

Rectangle {
    id: root
    width: 360
    height: 360

    Component.onCompleted: {
        var plotTest = true
        if (plotTest) {
            var qml = Util.getTestFigure()
            server.createFigure([0, "testFigure", qml])
            server.sendData([0, "testFigure.ax", {"minY": 0, "maxY": 8}])
            server.sendData([0, "testFigure.ax.blue", {"y": [0,1,2,3,2,1,3,5,2,10]}])
            server.sendData([0, "testFigure.ax.red", {"y": [4,8,13,0,0.5,3,4,5]}])
        }
    }

    Server {
        id: server
        address: "tcp://127.0.0.1:1234"
        updateAddress: "tcp://127.0.0.1:2468"
        Component.onCompleted: start()

        figureContainer: figureContainer
        tabView: tabView
        controller: controller
        userArea: userArea
    }

    Controller {
        id: controller
    }

    TabView {
        id: tabView
        anchors.fill: parent
        anchors.rightMargin: userOutline.width

        function addFigure(figure) {
            addTab(figure.handle, tabDelegate)
            figure.tabIndex = count - 1
            figure.visible = Qt.binding(function() { return figure.tabIndex == tabView.currentIndex })
        }

        Component {
            id: tabDelegate
            Item {
                Rectangle {
                    width: 50; height: 50
                    opacity: 0.5
                    color: "red"
                }
            }
        }

        style: TabViewStyle {
            frameOverlap: 1

            tab: Rectangle {
                z: -10
                color: styleData.selected ? "white" :"lightsteelblue"
                border.color:  "#777777"
                implicitWidth: Math.max(text.width + 4, 80)
                implicitHeight: 20
                radius: 2
                Text {
                    id: text
                    anchors.centerIn: parent
                    text: styleData.title
                    color: styleData.selected ? "black" : "black"
                }
            }

            frame: Rectangle {
                color: "white"
                border { color: "#777777"; width: 1 }
            }
        }

    }

    Item {
        id: figureContainer
        objectName: "container"
        anchors.fill: tabView
        anchors.topMargin: 30

//        Figure {
//            id: figure
//            Layouts.Column {
//                spacing: 0
//                Layouts.Repeater {
//                    count: 3
//                    Axis {
//                        handle: "ax"
//                        // LinePlot {}
//                        xAxis.majorTicks: [0,2,5,10]
//                        xAxis.onMajorTicksChanged: console.log("Major X:", xAxis.majorTicks)
//                        yAxis.onMajorTicksChanged: console.log("Major Y:", yAxis.majorTicks)
//                        LinePlot {
//                            handle: "blue"
//                            line { color: "#7777FF"; style: "-"; width: 4 }
//                        }
//                        LinePlot {
//                            handle: "red"
//                            line { color: "#FF7777"; style: "--"; width: 4 }
//                        }
//                    }
//                }
//            }
//        }
    }

    Item {
        id: userOutline
        anchors {
            right: parent.right; top: parent.top; bottom: parent.bottom
        }
        width: -resizeTarget.x

        Rectangle {
            anchors.fill: parent
            border { color: "#AAAAAA"; width: 1 }
        }
        Item {
            id: userArea
            anchors { fill: parent; margins: 5 }
        }
    }

    Item {
        id: resizeTarget
        x: -150
    }

    MouseArea {
        anchors.horizontalCenter: userOutline.left
        height: userOutline.height
        width: 5
        hoverEnabled: true
        cursorShape: Qt.SizeHorCursor
        drag {
            target: resizeTarget
            axis: Drag.XAxis

            maximumX: -50
            minimumX: -parent.width + 50
        }
    }


}
