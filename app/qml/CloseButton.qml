import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    id: button
    signal clicked(var mouse)

    property real size: 5
    clip: true

    width: size; height: size
    radius: size/2

//    color: "#AAFF7070"
    color: "#CCFF6259"

    border.width: mouseArea.containsMouse ? 1 : 0
    border.color: "white"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        preventStealing: true
        hoverEnabled: true
        onClicked: button.clicked(mouse)
    }
}
