import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1

import FileIO 1.0

Item {

    FileIO {
        id: nzmqtFile
        source: ":/licenses/LICENSE_nzmqt"
        onError: console.log("FileIO Error:", msg)
    }

    TabView {
        id: tabView
        anchors.fill: parent

        Tab {
            title: "Nutmeg"
        }

        Tab {
            title: "nzmqt"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: 'Nutmeg uses the Qt ZMQ bindings available at \
<a href="http://github.com/jonnydee/nzmqt">github.com/jonnydee/nzmqt</a>.\
The nzmqt project made life much simpler, so many thanks.'
                }

                Group {
                    title: "License"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    TextArea {
                        readOnly: true
                        text: nzmqtFile.read()
                        anchors.fill: parent
                    }
                }
            }
        }

        Tab {
            title: "ZMQ"
        }

//        style: TabViewStyle {
//            frameOverlap: 1

//            tab: Rectangle {
//                z: -10
//                color: styleData.selected ? "white" :"lightsteelblue"
//                border.color:  "#777777"
//                implicitWidth: Math.max(text.width + 4, 80)
//                implicitHeight: 20
//                radius: 2
//                Text {
//                    id: text
//                    anchors.centerIn: parent
//                    text: styleData.title
//                    color: styleData.selected ? "black" : "black"
//                }
//            }

//            frame: Rectangle {
//                color: "white"
//                border { color: "#777777"; width: 1 }
//            }
//        }

    }
}
