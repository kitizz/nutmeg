import QtQuick 2.1
import QtQuick.Controls 1.0
import Graphr 1.0
import "Util.js" as Util
import "Layouts" as Layouts
//import "Controls"

Rectangle {
    id: root
    width: 360
    height: 360

    Server {
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
//                        LinePlot {
//                            handle: "blue"
//                            lineColor: "#7777FF"; lineStyle: "-"; lineWidth: 4
//                        }
//                        LinePlot {
//                            handle: "red"
//                            lineColor: "#FF7777"; lineStyle: "-"; lineWidth: 4
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
        width: parent.width - resizeTarget.x

        Rectangle {
            anchors.fill: parent
            opacity: 0.3
            border { color: "black"; width: 1 }
        }
        Item {
            id: userArea
            anchors { fill: parent; margins: 5 }
        }
    }

    Item {
        id: resizeTarget
        x: parent.width - 150
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
            maximumX: parent.width - 50
            minimumX: 50
        }
    }


}
