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

    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue
    property alias stepSize: slider.stepSize
//    value: slider.value
    onValueChanged: {
        slider.value = value
        if (gui)
            gui.parameterChanged(handle, value)
    }

    implicitHeight: col.implicitHeight
    implicitWidth: parent.width

    onSetValue: {
        slider.value = newvalue
    }

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
            onValueChanged: sliderRoot.value = value
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

    Component.onCompleted: {
        findGui()
    }
}
