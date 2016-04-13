import QtQuick 2.1
import QtQuick.Controls 1.0

ControlItem {
    implicitWidth: btn.implicitWidth
    implicitHeight: btn.implicitHeight

    property alias text: btn.text
    value: 0

    Component.onCompleted: {
        registerProperty("text")
    }

    Button {
        id: btn

        anchors.fill: parent
        enabled: parent.enabled
        onClicked: {
            value += 1
        }
    }
}
