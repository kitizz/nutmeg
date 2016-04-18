import QtQuick 2.0
import Nutmeg 1.0

/*!
    \ingroup layouts
    Layouts.Column is used for laying out multiple Axis items in a column.
*/
ColumnLayout {
    id: col
    objectName: "column"
    width: parent.width
    height: parent.height

    Rectangle {
        anchors.fill: parent
        opacity: 0.2
        color: "blue"
    }
}
