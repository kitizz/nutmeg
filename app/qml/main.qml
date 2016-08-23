import QtQuick 2.4
//import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0
//import QtGraphicalEffects 1.0
import Nutmeg 0.1
import FileIO 1.0


Rectangle {
    id: root

    property var settings: null
    onSettingsChanged: console.log("Settings changed:", settings)

    // The actual app menu is built in C++ and triggers these signals from the C++ side
    signal savePdf
    signal selectedPdf(string file)

    signal testFig

    onSavePdf: {
        var fig = tabViewItem.currentFigure()
        console.log("Save PDF")

//        window.savePdfDialog(fig.handle)
    }

    onSelectedPdf: {
        console.log("Pdf Selected:", file)
        tabViewItem.currentFigure().savePdf(file)
    }

    // TODO: Reenable FileDialog when it is possible to set the filename...
//    FileDialog {
//        id: pdfDialog
//        visible: false

//        title: "Please choose a destination"
//        selectExisting: false
//        selectMultiple: false

////        nameFilters: [ "PDF file (*.pdf)"]
////        selectedNameFilter: "PDF file (*.pdf)"

//        onAccepted: {
//            console.log("You chose: " + pdfDialog.fileUrl)
//        }
//        onRejected: {
//            console.log("Canceled")
//        }
////        Component.onCompleted: visible = true
//    }

    FileIO {
        id: testAxisFile
        source: ":/qml/TestAxis.qml"
        onError: console.log("FileIO Error:", msg)
    }

    FileIO {
        id: testFigFile
        source: ":/qml/TestFigure.qml"
        onError: console.log("FileIO Error:", msg)
    }

    FileIO {
        id: test3dFile
        source: ":/qml/Test3D.qml"
        onError: console.log("FileIO Error:", msg)
    }

    function testfig() {
        // Connect file dialog signals
//        window.fileSelected.connect(selectedPdf)

        var plotTest = true
        if (plotTest) {
            var qmlAxis = testAxisFile.read()
            var qml = testFigFile.read()

            controller.queueTask("AddComponent", "TripleAxis", [qmlAxis], 0)

            controller.queueTask("SetFigure", "testFigure", [qml], 1)
//            return

            var xData = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.7, 0.8, 1.0, 1.2]
            var yData = [0, 0.5, 1, 3, 2, 1, 3, 5, 2, 10]
            controller.queueTask("SetProperty", "testFigure.ax.blue.x", [xData], 2)
            controller.queueTask("SetProperty", "testFigure.ax.blue.y", [yData], 3)

            controller.queueTask("SetProperty", "testFigure.ax.title", ["Yeah... Plot!!"], 4)
            controller.queueTask("SetProperty", "testFigure.ax.grid.axes", ["XY"], 5)

            controller.queueTask("SetProperty", "testFigure.ax.xAxis.label", ["X Label"], 6)
            controller.queueTask("SetProperty", "testFigure.ax.yAxis.label", ["Coolest Y Label Ever"], 7)

//            var yData2 = [1.000045, 1.000030, 1.000025, 1.000023, 1.000022, 1.0000215,
//                                                                    1.0000212, 1.0000211, 1.00002]
            var yData2 = [-0.1, 0, 0.5, 1, 0.9, 0.8, 0.7, 0.3, 0.0, -0.2]
            controller.queueTask("SetProperty", "testFigure.ax2.red.y", [yData2], 9)

        }
    }

    function test3dfig() {
        var qml = test3dFile.read()
        controller.queueTask("SetFigure", "test3D", [qml], 1)
//        server.createFigure( {"figureHandle": "test3D", "qml": qml} )

        var data = [0, 0, 0,
                    7, 1, 5,
                    2, 2, 2,
                    5, 3, 1,
                    6, 2, 7,
                    3, 1, 9,
                    2, 3, 1,
                    6, 5, 2,
                    7, 2, 3]
        var data2 = [0, 1, 0,
                    7, 5, 5,
                    9, 2, 2,
                    6, 3, 1,
                    3, 2, 7,
                    3, 3, 3,
                    2, 3, 0,
                    6, 5, 2,
                    7, 2, 3]
        controller.queueTask("SetProperty", "test3D.ax3d.pc.vertices", [data], 2)
        controller.queueTask("SetProperty", "test3D.ax3d.rc.start", [data], 3)
        controller.queueTask("SetProperty", "test3D.ax3d.rc.end", [data2], 4)
    }

    onTestFig: {
        testfig()
    }

    Server {
        id: server
        controller: controller
        onServerStarted: {
            var msg = "Publish to port: " + subPort + "\nSubscribe to port: " + pubPort
            window.notify("Server started", msg)
        }
        Component.onCompleted: start()
    }

    Controller {
        id: controller
        figureContainer: figureContainer
        guiContainer: userArea

        onFigureCreated: tabViewItem.addFigure(figure)
        onFigureDestroyed: {
            tabViewItem.closeFigure(figure)
        }
    }

    TabView {
        id: tabViewItem
        anchors.fill: parent
        anchors.rightMargin: userOutline.width

        property var figures: []

        function closePressed(figure) {
            controller.deregisterFigure(figure)
        }

        function closeFigure(figure) {
            console.log("Closing figure:", figure)
            var figureIndex = figure.tabIndex
            var listInd = figures.indexOf(figure)
            figures.splice(listInd, 1)
            removeTab(figureIndex)
        }

        function addFigure(figure) {
            figures.push(figure)
            var newTab = addTab(figure.handle, tabDelegate)

            tabViewItem.currentIndex = count - 1
            figure.tabIndex = count - 1
            figure.visible = Qt.binding(function() { return figure.tabIndex === tabViewItem.currentIndex })

            return true
        }

        function currentFigure() {
            return figures[tabViewItem.currentIndex]
        }

        Component {
            id: tabDelegate
            Item {}
        }

        property color linecolor: "#CCCCCC"

        style: TabViewStyle {
            frameOverlap: 1

            tab: TabStyle {
                linecolor:tabViewItem.linecolor
            }

            tabOverlap: 0

            tabBar: Rectangle {
                anchors.fill: parent
//                LinearGradient {
//                    cached: true
//                    anchors.fill: parent
//                    gradient: Gradient {
//                        GradientStop { position: 0; color: "#eeeeee" }
//                        GradientStop { position: 1; color: "#f6f6f6" }
//                    }
//                    Behavior on opacity {
//                        SmoothedAnimation { velocity: -1; duration: 100 }
//                    }
//                }
                Rectangle {
                    anchors { bottom: parent.bottom; right: parent.right }
                    width: parent.width; height: 1
                    color: tabViewItem.linecolor
                }
            }

            frame: Rectangle {
                color: "white"
            }
        }

    }

//    Item {
//        id: actions
//    }

    FigureContainer {
        id: figureContainer
        tabView: tabViewItem
        anchors.fill: tabViewItem
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
            Rectangle {
                width: 1
                height: parent.height
                color: tabViewItem.linecolor
            }
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
