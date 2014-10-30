import QtQuick 2.1
import QtQuick.Controls 1.0

ControlItem {
    implicitWidth: btn.implicitWidth
    implicitHeight: btn.implicitHeight

    value: 0

    Button {
        id: btn

        onClicked: {
            gui.parameterChanged(handle, 1)
        }
    }

    property alias text: btn.text

    Component.onCompleted: {
        findGui()
    }
}
