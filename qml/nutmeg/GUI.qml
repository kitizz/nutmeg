import QtQuick 2.0

Item {
    width: parent.width
    height: parent.height
    objectName: "GUI"

    property string figureHandle: ""
    property var server: null
    property var parameters: ({})

    function registerParameter(controlObj) {
        parameters[controlObj.handle] = controlObj
    }

    function parameterChanged(handle, value) {
        if (!server) return

        console.log("GUI.parameterChanged:", ['updateParam', figureHandle, handle, value])
        var msg = JSON.stringify(['updateParam', figureHandle, handle, value])
        // Yes, this is a little bit horrible... but the QJsonObject is quite useless...
        server.parameterChanged(figureHandle, handle, msg)
    }
}
