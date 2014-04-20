import QtQuick 2.0

Item {
    height: titleItem.height

    default property alias content: content.children

    property alias title: titleItem.text
    property color borderColor: "#AAAAAA"

    Text {
        id: titleItem
        x: 10
        width: implicitWidth
        height: implicitHeight
    }

    Rectangle {
        color: borderColor
        height: 1
        anchors.verticalCenter: titleItem.verticalCenter
        anchors.left: parent.left
        anchors.right: titleItem.left
        anchors.rightMargin: 2
    }

    Rectangle {
        color: borderColor
        height: 1
        anchors.verticalCenter: titleItem.verticalCenter
        anchors.left: titleItem.right
        anchors.leftMargin: 2
        anchors.right: parent.right
    }

    Rectangle {
        color: borderColor
        width: 1
        anchors.left: parent.left
        anchors.top: titleItem.verticalCenter
        anchors.bottom: parent.bottom
    }
    Rectangle {
        color: borderColor
        width: 1
        anchors.right: parent.right
        anchors.top: titleItem.verticalCenter
        anchors.bottom: parent.bottom
    }
    Rectangle {
        color: borderColor
        height: 1
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Item {
        id: content
        anchors {
            top: titleItem.bottom
            left: parent.left; right: parent.right
            bottom: parent.bottom
            margins: 5
        }
    }

}
