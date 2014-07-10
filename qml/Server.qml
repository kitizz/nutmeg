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

    property var figureContainer: null
    property var tabView: null
    property var controller: null
    property var userArea: null

    /*!
      Handles the initial request message. Selects the appropriate server command
      to execute based on the first argument in the message `["command", request={"arg1": val1, "arg2": val2, ...}]`
      \return Success: `command(request)` \n
      Unrecognized command: `[1, {"message": m}]`
    */
    onJsonReceived: {
        // Msg: {"handle": h, "data": {props: values}, "parameter": param}
        var command = server[request[0]]
        if (typeof command !== "function") {
            console.log("Command, " + request[0] + ", is not a valid command.")
            sendReply(JSON.stringify([1, {"message": "Unrecognized command"}]))
            return
        }

        var result = command(request[1])

        sendReply(JSON.stringify(result))
    }

    onBinaryReceived: { // (msg, data)
        // msg: {"handle": h, "property": prop, "parameter": param}
        // Duck punch the msg and pass it to sendData
        // We do it this way because JSON isn't so happy with converting binary data
        msg.data = {}
        msg.data[msg.property] = data
        var result = sendData(msg)

        sendReply(JSON.stringify(result))
    }

    /*!
        Create a figure based on the parameters provided in \a args.

        \param type:dictionary args: {"figureHandle": f, "qml": qmlString}
        \param figureHandle: (String) The unique handle for the figure being created.
        \param qml: (String) QML string declaring the layout of the figure. See more in ().
        \return Success: `[0, {"figureHandle": f, "port": parameterUpdatePort, "message": m}]` \n
        QML Failure: `[2, {"lineNumber": l, "columnNumber": c, "message": m}]`
    */
    function createFigure(args) {
        var figureHandle = args.figureHandle,
            qml = args.qml

        qml = "import QtQuick 2.1\nimport Graphr 1.0\nimport \"Layouts\" as Layouts\n" + qml

        // Figure handles must be unique, overwrite existing handles
        var oldFigure = controller.get(figureHandle)
        if (oldFigure) {
            if (oldFigure.guiItem)
                oldFigure.guiItem.destroy()
            tabView.closeFigure(oldFigure)
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
            return [2, err]
        }

        // Update the figure handle if one was provided
        if (figureHandle.length > 0)
            fig.handle = figureHandle
        tabView.addFigure(fig)
        fig.controller = controller
        var port = getPortForFigure(figureHandle)
        console.log("Figure's port:", port)
//        fig.installEventFilterApp(rootApp)
        return [0, {"figureHandle": fig.handle, "port": port, "message": "Figure created successfully."}]
    }

    /*!
        Layout parameter controls based on the parameters provided in \p args.

        \param type:dictionary args: A dictionary of args. {"figureHandle": f, "qml": qml]
        \param figureHandle: (String) The unique identifier of the new figure
        \param qml: (String) Should describe the layout of the controls used for
        adjusting parameter values related to \p figureHandle.
        \return Success: `[0, {"message": m, "parameters": p}]` \n
        Bad \p figureHandle: `[3, {"message": m}]` \n
        QML Failure: `[4, {"lineNumber": l, "columnNumber": c, "message": m}]`
    */
    function createGui(args) {
        var figureHandle = args.figureHandle,
            qml = args.qml

        var reply = {}

        var figure = controller.get(figureHandle)
        if (!figure || figure.objectName !== "figure") {
            reply.message = "Unable to find figure, " + handle + ". GUI requires a valid figure to attach to."
            return [3, reply]
        }

        qml = "import QtQuick 2.1\nimport Graphr 1.0\nimport \"Controls\"\n" + qml

        var gui
        try {
            gui = Qt.createQmlObject(qml, userArea, "GUIs")
        } catch (e) {
            // Offset for the added lines...
            e.qmlErrors[0].lineNumber -= 3
            return [4, e.qmlErrors[0]]
        }

        figure.guiItem = gui
        gui.visible = Qt.binding(function() { return figure.visible })
        gui.server = server
        gui.figureHandle = figureHandle

        var parameters = ({})
        for (var p in gui.parameters) {
            var control = gui.parameters[p]
            parameters[p] = control.value
        }

        reply.message = "GUI created successfully."
        reply.parameters = parameters
        return [0, reply]

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
        \return Success: `[0, {"message": m}]` \n
        Bad Property: `[5, {"message": m}]`
    */
    function sendData(args) {
        var handle = args.handle,
            data = args.data,
            parameter = args.parameter

        //  Match: (foo.bar.)extra
        var match = handle.match(/(.*?)\./)
        var figureHandle = match ? match[1] : ""
//        console.log("SendData", match, figureHandle, parameter)
        if (figureHandle && parameter)
            server.parameterUpdated(figureHandle, parameter)

        var obj = controller.get(handle)
        if (!obj)
            return [6, {"message": "Object, " + handle + ", does not exist."}]

        for (var prop in data) {
            console.log("Setting", prop, "in", obj)
            var props = prop.split(".")
            var realHandle = [handle].concat(props.slice(0, -1)).join(".")
            console.log("Real Handle:", realHandle)
            var realObj = controller.get(realHandle)
            var realProp = props[props.length - 1]

            var result = setProperties(realObj, realProp, data[prop])
            if (!result)
                return [5, {"message": "Property, " + prop + ", of " + handle + " cannot be set."}]
        }

        return [0, {"message": "Data updated successfully."}]
    }

    function setProperties(obj, prop, data) {
        /* Takes an obj or array of objects and sets its/their properties according to prop + data */
        if (!obj) return
        if (Util.isArray(obj)) {
            for (var i=0; i<obj.length; ++i) {
                var d = Util.isArray(data) && Util.isArray(data[i]) ? data[i] : data
//                if (obj[i])
                var result = setProperties(obj[i], prop, d)
                if (!result) return false
            }
            return true
        } else {
            // Check if the property is valid
            console.log("Setting property", prop, "for", obj)
            var propName = obj.map(prop)
            if (!propName) return false

            obj[propName] = data
            console.log("Done")
            return true
        }
    }
}
