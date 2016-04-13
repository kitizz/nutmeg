import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

RowLayout {
    id: root
    property alias label: labelitem.text
    property alias text: textitem.text
    property alias hint: hintitem.text

    property alias textItem: textitem

    signal submit

    spacing: 5
    Label {
        id: labelitem
    }

    TextField {
        id: textitem
        Layout.maximumWidth: 120
        height: implicitHeight
//        text: ""
        Keys.onReturnPressed: root.submit()
    }

    Label {
        id: hintitem
        text: "(40000 - 65535)"
        color: "#AAAAAA"
    }
}
