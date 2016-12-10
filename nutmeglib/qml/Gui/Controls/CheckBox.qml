import QtQuick 2.1
import QtQuick.Controls 1.4

ControlItem {
    id: ctl
    implicitWidth: box.implicitWidth
    implicitHeight: box.implicitHeight

    property alias text: box.text
    value: 0

    onValueChanged: {
        box.checked = (value != 0)
    }

    Component.onCompleted: {
        registerProperty("text")
    }

    CheckBox {
        id: box

        anchors.fill: parent
        enabled: parent.enabled
        onCheckedChanged: value = checked
    }
}
