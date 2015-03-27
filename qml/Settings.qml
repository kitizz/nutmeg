import Nutmeg 1.0
import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Item {
    width: 100
    height: 62

    property var window: null
    property var settings: null

    Item {
        anchors.fill: parent
        anchors.margins: 10

        ColumnLayout {
            anchors.fill: parent
            Group {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "Server"

                RowLayout {
                    spacing: 5
                    Label {
                        text: "Port:"
                    }

                    TextField {
                        Layout.maximumWidth: 120
                        height: implicitHeight
                        text: settings ? settings.port : 0
                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator { bottom: 40000; top: 65535 }
                        onAccepted: {
                            if (!settings) return
                            settings.port = text
                        }

                        Keys.onReturnPressed: {
                            if (!settings) return
                            if (acceptableInput)
                                settings.port = text
                            else
                                text = settings.port
                        }
                    }

                    Label {
                        text: "(40000 - 65535)"
                        color: "#AAAAAA"
                    }
                }
            }

            Button {
                anchors.right: parent.right
                text: "Close"
                onClicked: {
                    if (window)
                        window.showHide()
                }
            }
        }
    }
//    TabView {
//        id: tabView

//        Tab {
//            title: "Server"
//            TextField {
//                inputMethodHints: Qt.ImhDigitsOnly
//                validator: IntValidator { bottom: 40000; top: 65535 }
//                onAccepted: {
//                    console.log("Text:", text)
//                }
//            }
//        }
//    }

}
