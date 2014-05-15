import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import Graphr 1.0
import "Util.js" as Util
import "Layouts" as Layouts
import FileIO 1.0

//import "Controls"

Rectangle {
    id: root
//    width: 600
//    height: 360
    property var settings: null
    onSettingsChanged: console.log("Settings changed:", settings)

    FileIO {
        id: testFile
        source: ":/qml/TestFigure.qml"
        onError: console.log("FileIO Error:", msg)
    }

    Component.onCompleted: {
        var plotTest = true
        if (plotTest) {
            var qml = testFile.read()
            server.createFigure( {"figureHandle": "testFigure", "qml": qml} )
//            server.sendData([0, "testFigure.ax", {"minY": 0, "maxY": 8}])
            server.sendData( {"handle": "testFigure.ax.blue", "data": {"y": [0,1,2,3,2,1,3,5,2,10]}} )
            server.sendData( {"handle": "testFigure.ax.red", "data": {"y": [4,8,13,0.1,0.5,3,4,5]}} )
//            server.sendData([0, "testFigure.ax.blue", {"y": [3,1]}])
        }
    }

    Server {
        id: server
        port: settings ? settings.port : 43686
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

        property var figures: []

        function closeFigure(figure) {
            var figureIndex = figure.tabIndex
            figure.destroy()
            removeTab(figureIndex)
        }

        function addFigure(figure) {
            figures.push(figure)
            addTab(figure.handle, tabDelegate)

            tabView.currentIndex = count - 1
            figure.tabIndex = count - 1
            figure.visible = Qt.binding(function() { return figure.tabIndex == tabView.currentIndex })
        }

        Component {
            id: tabDelegate
            Item {
            }
        }

        style: TabViewStyle {
            frameOverlap: 1

            tab: Rectangle {
                id: tab
                property int index: styleData.index
                property var figure: null
                // TODO: This is horrible and brittle, but QtQuickControls 1.1 left me no other option...
                onIndexChanged: if (figure) figure.tabIndex = index
                Component.onCompleted: {
                    console.log("Figures:", tabView.figures)
                    figure = tabView.figures[styleData.index]
                    figure.tabIndex = index
                }

                z: -10
                color: styleData.selected ? "white" :"lightsteelblue"
                border.color:  "#777777"
                implicitWidth: Math.max(text.width + closeButton.size + 8, 80)
                implicitHeight: 20
                radius: 2
                Text {
                    id: text
                    x: 2
                    anchors.verticalCenter: parent.verticalCenter
                    text: styleData.title
                    color: styleData.selected ? "black" : "black"
                }

                CloseButton {
                    id: closeButton
                    size: parent.height*0.7
                    anchors {
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: 0.15*parent.height
                    }
                    onClicked: {
                        tabView.closeFigure(tab.figure)
                    }
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
