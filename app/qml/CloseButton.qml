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

    color: mouseArea.containsMouse ? "white" : "transparent"
    border {
        width: 1
        color: {
            if (mouseArea.containsMouse) {
                if (mouseArea.pressed)
                    return linecolor
                else
                    return Qt.lighter(linecolor)
            } else {
                return "transparent"
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        preventStealing: true
        hoverEnabled: true
        onClicked: button.clicked(mouse)
    }
}
