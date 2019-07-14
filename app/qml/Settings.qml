import Nutmeg 0.1
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

                ColumnLayout {
                    TextEntry {
                        label: "Subscribe Port:"
                        text: settings ? settings.portSubscribe : 0
                        hint: "(40000 - 65535)"
                        textItem {
                            inputMethodHints: Qt.ImhDigitsOnly
                            validator: IntValidator { bottom: 40000; top: 65535 }
                        }
                        onSubmit: {
                            console.log("onAccepted", settings)
                            if (!settings) return
                            if (textItem.acceptableInput) {
                                console.log("Setting subscribe to:", parseInt(text, 10))
                                settings.portSubscribe = parseInt(text, 10)
                                console.log("Check:", settings.portSubscribe)
                            } else {
                                text = Qt.binding(function() { return settings.portSubscribe })
                            }
                        }
                    }

                    TextEntry {
                        label: "Publish Port:"
                        text: settings ? settings.portPublish : 0
                        hint: "(40000 - 65535)"
                        textItem {
                            inputMethodHints: Qt.ImhDigitsOnly
                            validator: IntValidator { bottom: 40000; top: 65535 }
                            onAccepted: {
                                if (!settings) return
                                settings.portPublish = text
                                text = Qt.binding(function() { return settings.portPublish })
                            }
                        }
                    }
                }

            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: "Close"
                onClicked: {
                    if (window)
                        window.showHide()
                }
            }
        }
    }
}
