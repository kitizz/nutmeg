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

    color: "#AAFFFFFF"

    border.width: 1
    border.color: {
        if (mouseArea.pressed)
            return "#BBB"
        if (mouseArea.containsMouse)
            return "#333"
        return "#888"
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: button.clicked(mouse)
    }
}
