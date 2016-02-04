import QtQuick 2.1
import QtQuick.Controls 1.0

ControlItem {
    implicitWidth: btn.implicitWidth
    implicitHeight: btn.implicitHeight

    property alias text: btn.text
    value: 0

    Button {
        id: btn

        enabled: parent.enabled
        onClicked: {
            gui.parameterChanged(handle, 1)
        }
    }


    Component.onCompleted: {
        findGui()
    }
}
