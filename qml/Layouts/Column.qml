import QtQuick 2.0
import QtQuick.Layouts 1.0
import "../Util.js" as Util

ColumnLayout {
    id: col
    objectName: "column"
    width: parent.width
    height: parent.height

    property int rows: 0

    onChildrenChanged: updateLayout()
    onHeightChanged: updateLayout()

    function updateLayout() {
        rows = children.length

        var i
        var fixedItems = []
        var fixedRatioItems = []
        var variableItems = []
        for (i=0; i<rows; ++i) {
            var item = children[i]
            if (item.objectName == "repeater")
                continue // Repeaters get in the way..

            if (typeof item.heightFraction === 'undefined') {
                fixedItems.push(item)
                continue
            }

            item.Layout.preferredWidth = Qt.binding(function() {
                return item.widthFraction < 0 ? col.width : item.widthFraction*col.width
            })

            if (item.heightFraction < 0 || item.Layout.prefferedHeight <= 0)
                variableItems.push(item)
            else
                fixedRatioItems.push(item)
        }

        var totalHeight = height - spacing*(rows - 1)
        var heightRemaining = totalHeight

        for (i=0; i<fixedItems.length; ++i) {
//            fixedItems[i].Layout.prefferedHeight = fixedItems[i].height
            heightRemaining -= fixedItems[i].Layout.prefferedHeight
        }

        for (i=0; i<fixedRatioItems.length; ++i) {
            var h = fixedRatioItems[i].heightFraction*totalHeight
            fixedRatioItems[i].Layout.preferredHeight = h
            heightRemaining -= h
        }

        var variableHeights = heightRemaining/variableItems.length
        for (i=0; i<variableItems.length; ++i)
            variableItems[i].Layout.preferredHeight = variableHeights

    }
}
