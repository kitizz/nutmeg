import QtQuick 2.0
import QtQuick.Layouts 1.0
import "../Util.js" as Util

/*!
    \ingroup layouts
    Layouts.Column is used for laying out multiple Axis items in a column.
*/
ColumnLayout {
    id: col
    objectName: "column"
    width: parent.width
    height: parent.height

    property int rows: 0
    property string groupName: ""
    property var axes: []

    onChildrenChanged: updateLayout()
    onHeightChanged: updateLayout()

    function updateLayout() {
        var N = children.length

        var i
        var newAxes = []
        var fixedItems = []
        var fixedRatioItems = []
        var variableItems = []
        for (i=0; i<N; ++i) {
            var item = children[i]
            if (item.objectName == "repeater")
                continue // Repeaters get in the way..
            newAxes.push(item)

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
        if (!listEqual(axes, newAxes)) {
            if (newAxes[0].figure != null) {
                axes = newAxes
                rows = newAxes.length
                groupName = axes[0].figure.registerAxisGroup(axes, 2, groupName)
            }
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

    function listEqual(l1, l2) {
        if (l1.length !== l2.length)
            return false
        for (var i=0; i<l1.length; ++i) {
            if (l1[i] !== l2[i])
                return false
        }
        return true
    }
}
