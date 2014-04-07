import QtQuick 2.1
import QtQuick.Controls 1.0

Item {
    id: sliderRoot
    property alias handle: label.text
    property alias value: slider.value
    property alias slider: slider
    property int decimals: 2

    property var gui: null
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
                text: slider.minimumValue
            }

            Label {
                anchors.right: parent.right
                text: slider.maximumValue
            }
        }
    }

    onValueChanged: {
        gui.parameterChanged(handle, value)
    }

    function findGui() {
        if (gui) return // Already found it...

        var ancestor = parent
        while (ancestor) {
            if (ancestor.objectName == "GUI") {
                gui = ancestor
                gui.registerParameter(sliderRoot)
                return
            }
            ancestor = ancestor.parent
        }
    }

    Component.onCompleted: {
        findGui()
    }

    onParentChanged: findGui()
}
