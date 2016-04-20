import QtQuick 2.4
import QtGraphicalEffects 1.0

Rectangle {
    id: tab
    property int index: styleData.index
    property var figure: null
    property color linecolor: "#BBBBBB"

//    property var tabViewItem: tabView
    // TODO: This is horrible and brittle, but QtQuickControls 1.1 left me no other option...
    onIndexChanged: if (figure) figure.tabIndex = index
    Component.onCompleted: {
        console.log("Figures:", tabViewItem.figures)
        figure = tabViewItem.figures[styleData.index]
        figure.tabIndex = index
    }

    z: -10
    color: styleData.selected ? "white" : "transparent"
    implicitWidth: Math.max(text.width + closeButton.size + 16, 80)
    implicitHeight: 22

    Text {
        id: text
        x: 0.5*(parent.width - closeButton.width - 0.3*parent.height - text.implicitWidth)
        anchors.verticalCenter: parent.verticalCenter
        text: styleData.title
        color: styleData.selected ? "black" : "black"
    }

    CloseButton {
        id: closeButton
        size: parent.height*0.6

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 0.2*parent.height
        }
        onClicked: {
            tabViewItem.closePressed(tab.figure)
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        visible: !styleData.selected
        color: "#f6f6f6"

        width: parent.width
        height: 1
        Behavior on width {
            SmoothedAnimation { velocity: -1; duration: 70 }
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        color: tab.linecolor

        width: styleData.selected ? 0 : parent.width
        height: 1
        Behavior on width {
            SmoothedAnimation { velocity: -1; duration: 70 }
        }
    }

    Rectangle {
        id: spacer
        anchors.rightMargin: 0
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        width: 1
        height: parent.height
        color: tab.linecolor
    }
}
