import QtQuick 2.4
import QtGraphicalEffects 1.0

Rectangle {
    id: tab
    property int index: styleData.index
    property var figure: null
//    property var tabViewItem: tabView
    // TODO: This is horrible and brittle, but QtQuickControls 1.1 left me no other option...
    onIndexChanged: if (figure) figure.tabIndex = index
    Component.onCompleted: {
        console.log("Figures:", tabViewItem.figures)
        figure = tabViewItem.figures[styleData.index]
        figure.tabIndex = index
    }

    z: -10
    color: "white"
    implicitWidth: Math.max(text.width + closeButton.size + 16, 80)
    implicitHeight: 22

    LinearGradient {
        cached: true
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0; color: "#F6F6F6" }
            GradientStop { position: 1; color: "white" }
        }
        opacity: styleData.selected ? 0 : 1
    }

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
        property bool selected: styleData.selected
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: text.horizontalCenter

        color: "#46E831"
        width: text.width

        height: selected ? 2 : 0
//        width: selected ? 0.8*parent.width : parent.width
//        color: selected ? "#46E831" : "#777777"
        Behavior on height {
            SmoothedAnimation {
                velocity: -1
                duration: 100
            }
        }
    }

    Rectangle {
        id: spacer
        anchors.rightMargin: 0
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        width: 1
        height: 0.6 * parent.height
        color: "black"
        opacity: 0.2

    }
}