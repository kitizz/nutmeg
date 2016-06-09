import QtQuick 2.1
import QtQuick.Controls 1.4

ControlItem {
    id: ctl
    implicitWidth: btn.implicitWidth
    implicitHeight: btn.implicitHeight

    signal clicked

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
            ctl.clicked()
        }
    }
}
