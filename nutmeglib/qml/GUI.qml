//import QtQuick 2.0
import Nutmeg 1.0

GuiBase {
    width: parent.width
    height: parent.height
    objectName: "GUI"

//    property string figureHandle: ""
//    property var server: null
//    property var parameters: ({})

//    function registerParameter(controlObj) {
//        // This is a horrible hack for a Qt5.6 bug. Hopefully temporary..
//        // The parameter's object doesn't seem to get updated when read by Server.qml after this..
//        var new_param = ({})
//        for (var p in parameters)
//            new_param[p] = parameters[p]
//        new_param[controlObj.handle] = controlObj
//        parameters = new_param
////        parameters[controlObj.handle] = controlObj
//    }

//    function parameterChanged(handle, value) {
//        if (!server) return

//        console.log("GUI.parameterChanged:", ['updateParam', figureHandle, handle, value])
//        var msg = JSON.stringify(['updateParam', figureHandle, handle, value])
//        // Yes, this is a little bit horrible... but the QJsonObject is quite useless...
//        server.parameterChanged(figureHandle, handle, msg)
//    }
}
