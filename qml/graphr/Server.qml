import QtQuick 2.0
import Graphr 1.0
import "Util.js" as Util

ServerBase {
    id: server
    objectName: "server"

    property var rootApp: null
    onRootAppChanged: console.log("Server, appChanged", rootApp)
    property var figureContainer: null
    property var tabView: null
    property var controller: null
    property var userArea: null

    onRequestReceived: {
//        console.log("Req:", request)
//        var req = JSON.parse(request)
        var req = request
        var command = server[req[0]]
        console.log("Command:", req[0])
        if (typeof command !== "function") {
            console.log("Command, " + req[0] + ", is not a valid command.")
            sendReply(JSON.stringify([1, "Unrecognised command"]))
            return
        }

        var result = command(req)
        sendReply(JSON.stringify(result))
    }

    function createFigure(req) {
        var handle = req[1],
            qml = req[2]

        qml = "import QtQuick 2.1\nimport Graphr 1.0\nimport \"Layouts\" as Layouts\n" + qml

        // Figure handles must be unique, overwrite existing handles
        var oldFigure = controller.get(handle)
        if (oldFigure) {
            oldFigure.guiItem.destroy()
            tabView.removeTab(oldFigure.tabIndex)
            oldFigure.destroy()
        }

        var par = figureContainer ? figureContainer : server
        var fig
        try {
            fig = Qt.createQmlObject(qml, par, "Figures")
        } catch (e) {
            // Offset for the added lines...
            e.qmlErrors[0].lineNumber -= 3
            return [2, e]
        }

        // Update the figure handle if one was provided
        if (handle.length > 0)
            fig.handle = handle
        tabView.addFigure(fig)
        fig.controller = controller
        fig.app = rootApp
        return [0, fig.handle, "Figure created successfully."]
    }

    function sendData(req) {
        var handle = req[1],
            data = nutmeg2json(req[2])

        var parameter = req[3]
        var match = handle.match(/&(.*?)\./)
        var figureHandle = match ? match[1] : ""
        if (figureHandle && parameter)
            server.parameterUpdated(figureHandle, parameter)

        var obj = controller.get(handle)
//        console.log(obj)
        for (var prop in data)
            setProperties(obj, prop, data[prop])

        return [0, "Data updated successfully."]
    }

    function createGui(req) {
        var handle = req[1],
            qml = req[2]

        var figure = controller.get(handle)
        if (!figure || figure.objectName !== "figure") {
            return [3, "Unable to find figure, " + handle + ". GUI requires a valid figure to attach to."]
        }

        qml = "import QtQuick 2.1\nimport Graphr 1.0\nimport \"Controls\"\n" + qml

        var gui
        try {
            gui = Qt.createQmlObject(qml, userArea, "GUIs")
        } catch (e) {
            // Offset for the added lines...
            e.qmlErrors[0].lineNumber -= 3
            return [4, e]
        }

        figure.guiItem = gui
        gui.visible = Qt.binding(function() { return figure.visible })
        gui.server = server
        gui.figureHandle = handle

        var parameters = ({})
        for (var p in gui.parameters) {
            var control = gui.parameters[p]
            parameters[p] = control.value
        }

//        console.log("Create GUI. Parameters:", parameters)

        return [0, "GUI created successfully.", parameters]

    }

    function setProperties(obj, prop, data) {
        /* Takes an obj or array of objects and sets its/their properties according to prop + data */
//        console.log("Setting Properties", obj, prop, data.length)
        if (Util.isArray(obj)) {
            for (var i=0; i<obj.length; ++i) {
                var d = Util.isArray(data) && Util.isArray(data[i]) ? data[i] : data
                setProperties(obj[i], prop, d)
            }
        } else {
//            console.log("Assign to real prop:", obj.map(prop), data.length)
            obj[obj.map(prop)] = data
        }
    }

    function nutmeg2json(data) {
        if (Util.isArray(data)) {
            for (var i=0; i<data.length; ++i) {
                data[i] = nutmeg2json(data[i])
            }
            return data

        } else if (Util.isObject(data)) {
            if (data.list)
                return nutmeg2json(data.list)
            var before = Util.dir(data)
            for (var p in data)
                data[p] = nutmeg2json(data[p])
            return data

        } else {
            return data
        }
    }
}
