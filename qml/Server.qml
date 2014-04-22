import QtQuick 2.0
import Graphr 1.0
import "Util.js" as Util

/*!
    \ingroup clientApi
    Server handles requests from Nutmeg clients.
*/
ServerBase {
    id: server
    objectName: "server"

//    property var rootApp: null
//    onRootAppChanged: console.log("Server, appChanged", rootApp)
    property var figureContainer: null
    property var tabView: null
    property var controller: null
    property var userArea: null

    /*!
      Handles the initial request message. Selects the appropriate server command
      to execute based on the first argument in the message `["command", ...]`
      \return Success: `command(request)` \n
      Unrecognized command: `[1, msg]`
    */
    onRequestReceived: {
        var req = request
        var command = server[req[0]]
        if (typeof command !== "function") {
            console.log("Command, " + req[0] + ", is not a valid command.")
            sendReply(JSON.stringify([1, "Unrecognized command"]))
            return
        }

        var result = command(req)
        sendReply(JSON.stringify(result))
    }

    /*!
        Create a figure based on the parameters provided in \a request.

        \param type:list Request: ["createFigure", figureHandle, qmlString]
        \param qmlString QML string declaring the layout of the figure. See more in ().
        \return Success: `[0, figureHandle, parameterUpdatePort, msg]` \n
        QML Failure: `[2, {"lineNumber": l, "columnNumber": c, "message": m}]`
    */
    function createFigure(req) {
        var handle = req[1],
            qml = req[2]

        qml = "import QtQuick 2.1\nimport Graphr 1.0\nimport \"Layouts\" as Layouts\n" + qml

        // Figure handles must be unique, overwrite existing handles
        var oldFigure = controller.get(handle)
        if (oldFigure) {
            if (oldFigure.guiItem)
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
            var err = e.qmlErrors[0]
            console.warn("At line", err.lineNumber + ", col", err.columnNumber + ":", err.message, "\n")
            return [2, e]
        }

        // Update the figure handle if one was provided
        if (handle.length > 0)
            fig.handle = handle
        tabView.addFigure(fig)
        fig.controller = controller
        var port = getPortForFigure(handle)
        console.log("Figure's port:", port)
//        fig.installEventFilterApp(rootApp)
        return [0, fig.handle, port, "Figure created successfully."]
    }

    /*!
        Layout parameter controls based on the parameters provided in \p req.

        \param type:list req Request: ["createGui", figureHandle, qmlString]
        \param type:string figureHandle The unique identifier of the new figure
        \param type:string qmlString should describe the layout of the controls used for
        adjusting parameter values related to \p figureHandle.
        \return Success: `[0, msg, parameters]` \n
        Bad \p figureHandle: `[3, msg]` \n
        QML Failure: `[4, {"lineNumber": l, "columnNumber": c, "message": m}]`
    */
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

        return [0, "GUI created successfully.", parameters]

    }

    /*!
        Sets the data or properties of `plotHandle` according the values stored in `dataDict`.
        Multiple objects can be referenced using Python-style pretty indexing (see below).

        For a single set of data, \p dataList should be a 1D list.
        For a multiple sets of data destined for multiple axes or plots, \p dataList
        should be a 2D list, regardless of the number of data points. Where the first dimension
        contains a list for each dataset. In the following example, a figure has an axis group
        of 3 (so they have the same handle). Each axis contains a line plot with the handle "myData":

        >    [ "sendData", "myFigure.axes[0:3].myData", {"x": [0,1,2], "y": [ [2,4,1], [9,1,0], [1,2,3] ]} ]


        \param type:List req Request: ["sendData", handle, dataDict, parameter (optional)]
        \param type:string handle The hierarchical handle to the plot of interest
        (e.g. `myFigure.axis[0].experiment1Data`)
        \param type:dict dataDict The data dictionary should contain one or both of `"x"` and `"y"`
        data for a 2D plot (e.g. `{"x": [0,1,2], "y": [2,4,1]}`)
        \param type:string parameter Optional argument which informs the server which changing
        parameter that this update is in response to. See Socket Architecture for details
        \return Success: `[0, msg]`
    */
    function sendData(req) {
        var handle = req[1],
            data = req[2]

//        console.log("SendData", handle)
        var parameter = req[3]
        var match = handle.match(/(.*?)\./)
        var figureHandle = match ? match[1] : ""
//        console.log("SendData", match, figureHandle, parameter)
        if (figureHandle && parameter)
            server.parameterUpdated(figureHandle, parameter)

        var obj = controller.get(handle)
//        console.log("SendData:", obj)
        for (var prop in data)
            setProperties(obj, prop, data[prop])

        return [0, "Data updated successfully."]
    }

    function setProperties(obj, prop, data) {
        /* Takes an obj or array of objects and sets its/their properties according to prop + data */
        if (!obj) return
        if (Util.isArray(obj)) {
            for (var i=0; i<obj.length; ++i) {
                var d = Util.isArray(data) && Util.isArray(data[i]) ? data[i] : data
                setProperties(obj[i], prop, d)
            }
        } else {
            obj[obj.map(prop)] = data
        }
    }
}
