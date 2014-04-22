import QtQuick 2.0
import QtQuick.Layouts 1.0

/*!
    \ingroup layouts
    Layouts.Row is used for laying out multiple Axis items in a row.
    (This is currently not fully implemented)
*/
RowLayout {
    objectName: "row"

    onChildrenChanged: updateLayout()
    onWidthChanged: updateLayout()

    function updateLayout() {
        cols = children.length

        var i
        var fixedItems = []
        var variableItems = []
        for (i=0; i<cols; ++i) {
            var item = children[i]
            if (item.heightFraction < 0)
                item.Layout.preferredHeight = height

            if (item.widthFraction < 0)
                variableItems.push(item)
            else
                fixedItems.push(item)
        }

        var widthRemaining = width
        for (i=0; i<fixedItems.length; ++i) {
            var w = fixedItems[i].widthFraction*width
            fixedItems[i].Layout.preferredWidth = w
            heightRemaining -= w
        }

        var variableWidths = widthRemaining/variableItems.length
        for (i=0; i<variableItems.length; ++i)
            variableItems[i].Layout.preferredWidth = variableWidths

    }
}
