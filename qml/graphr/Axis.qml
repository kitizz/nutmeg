import QtQuick 2.0
import Graphr 1.0
import QtQuick.Layouts 1.0

AxisBase {
    id: axis
    handle: "ax"
    width: parent.width
    height: parent.height
//    app: figure.app
    onAppChanged: console.log("Axis appChanged", app)

    property real widthFraction: -1
    property real heightFraction: -1

    Rectangle {
        anchors.fill: parent
        opacity: 0.5
        border { color: "red"; width: 1 }
    }
}
