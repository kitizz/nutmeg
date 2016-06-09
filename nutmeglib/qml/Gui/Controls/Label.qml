import QtQuick 2.1
import QtQuick.Controls 1.4

ControlItem {
    implicitWidth: txt.implicitWidth
    implicitHeight: txt.implicitHeight

    property alias text: txt.text
    value: 0

    Component.onCompleted: {
        registerProperty("text")
    }

    Text {
        id: txt

        anchors.fill: parent
        enabled: parent.enabled
    }
}
