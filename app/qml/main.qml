import QtQuick 2.4
//import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.0
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
        id: testFile
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
            var qml = testFile.read()

            controller.queueTask("SetFigure", "testFigure", [qml], 1)
//            return

            var xData = [0, 1, 2, 3, 4, 5, 7, 8, 10, 12]
            var yData = [0, 1, 2, 3, 2, 1, 3, 5, 2, 10]
            controller.queueTask("SetProperty", "testFigure.ax.blue.x", [xData], 2)
            controller.queueTask("SetProperty", "testFigure.ax.blue.y", [yData], 3)

            controller.queueTask("SetProperty", "testFigure.ax.title", ["Yeah... Plot!!"], 4)
            controller.queueTask("SetProperty", "testFigure.ax.grid.axes", ["XY"], 5)

            controller.queueTask("SetProperty", "testFigure.ax.xAxis.label", ["X Label"], 6)
            controller.queueTask("SetProperty", "testFigure.ax.yAxis.label", ["Coolest Y Label Ever"], 7)

            var yData2 = [1.000045, 1.000030, 1.000025, 1.000023, 1.000022, 1.0000215,
                                                                    1.0000212, 1.0000211, 1.00002]
            controller.queueTask("SetProperty", "testFigure.ax2.red.y", [yData2], 9)

        }
    }

    function test3dfig() {
        var qml = test3dFile.read()
        server.createFigure( {"figureHandle": "test3D", "qml": qml} )

        server.sendData( {"handle": "test3D.ax3d.pc", "data": {
                                "x": [0,7,2,5,6,3,2,6,7,3],
                                "y": [0,1,2,3,2,1,3,5,2,10],
                                "z": [0,5,2,1,7,9,1,2,3,4]
        }} )
    }

    onTestFig: {
        testfig()
    }

//    Test3D {}

    Server {
        id: server
        controller: controller
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

        style: TabViewStyle {
            frameOverlap: 1

            tab: TabStyle {}

            tabOverlap: -1

            tabBar: Rectangle {
                anchors.fill: parent
                Rectangle {
                    anchors { bottom: parent.bottom; right: parent.right }
                    width: parent.width; height: 1
                    color: "black"; opacity: 0.2
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
