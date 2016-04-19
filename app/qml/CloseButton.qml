import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    id: button
    signal clicked(var mouse)

    property real size: 5
    property color linecolor: "#999999"
    clip: true

    width: size; height: size
    radius: size/2

//    color: "#AAFF7070"
//    color: "#CCFF6259"

    Rectangle {
        anchors.centerIn: parent
        width: 0.6*parent.width
        height: 1
        rotation: 45
        color: linecolor
    }
    Rectangle {
        anchors.centerIn: parent
        width: 0.6*parent.width
        height: 1
        rotation: -45
        color: linecolor
    }

//    border.width: mouseArea.containsMouse ? 1 : 0
//    border.color: "white"
    border {
        width: 1
        color: mouseArea.containsMouse ? linecolor : Qt.lighter(linecolor)
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        preventStealing: true
        hoverEnabled: true
        onClicked: button.clicked(mouse)
    }
}
