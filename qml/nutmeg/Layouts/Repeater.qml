import QtQuick 2.0

Item {
    objectName: "repeater"

    property int count: 0

    default property Component delegate: null
    property var items: []
    function clearItems() {
        if (!items) return
        console.log(items)
        for (var i=0; i<items.length; ++i)
            items.pop().destroy()
    }

//    children: [
//    Item {
//        id: priv // A QML pattern for keeping private variables


//    }
//    ]

    onCountChanged: updateItems()

    onParentChanged: {
        clearItems()
        updateItems()
    }
    onDelegateChanged: {
        clearItems()
        updateItems()
    }

    Component.onCompleted: updateItems()

    function updateItems() {
        if (!delegate || !parent || !items) return; // Don't update lastCount

        var delta = count - items.length

        if (delta > 0) { // Add items
            for (var i=0; i<delta; ++i)
                items.push(delegate.createObject(parent))

        } else if (delta < 0) { // Remove them from the end
            for (var i=0; i<-delta; ++i)
                items.pop().destroy()
        }
    }
}
