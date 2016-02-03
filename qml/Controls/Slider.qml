import QtQuick 2.1
import QtQuick.Controls 1.0

/*!
    \ingroup controls
    Slider is a typical sliding control.
*/
ControlItem {
    id: sliderRoot

    property alias text: label.text
    property alias slider: slider
    property int decimals: 2

    value: slider.value

    implicitHeight: col.implicitHeight
    implicitWidth: parent.width

    Column {
        id: col
        width: parent.width
        Item {
            implicitHeight: childrenRect.height
            width: slider.width
            Label {
                id: label
                anchors.left: parent.left
                anchors.leftMargin: 5
            }
            Label {
                id: valueLabel
                anchors.right: parent.right
                anchors.rightMargin: 5
                text: Number(slider.value).toFixed(decimals)
            }
        }

        Slider {
            id: slider
            width: parent.width
            tickmarksEnabled: true
        }

        Item {
            implicitHeight: childrenRect.height
            width: slider.width
            Label {
                anchors.left: parent.left
                text: Number(slider.minimumValue).toFixed(decimals)
            }

            Label {
                anchors.right: parent.right
                text: Number(slider.maximumValue).toFixed(decimals)
            }
        }
    }

    onValueChanged: {
        if (gui)
            gui.parameterChanged(handle, value)
    }

    Component.onCompleted: {
        findGui()
    }
}
