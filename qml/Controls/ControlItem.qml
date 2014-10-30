import QtQuick 2.0

Item {
    id: controlItem
    property string handle: "guiItem"
    property var value: 0
    property var gui: null

    function findGui() {
        if (gui) return // Already found it...

        var ancestor = parent
        while (ancestor) {
            if (ancestor.objectName == "GUI") {
                gui = ancestor
                gui.registerParameter(controlItem)
                return
            }
            ancestor = ancestor.parent
        }
    }

    Component.onCompleted: {
        findGui()
    }

    onParentChanged: findGui()
}
