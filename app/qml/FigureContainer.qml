import QtQuick 2.0

Item {
    id: figureContainer
    objectName: "figureContainer"

    property var tabView: null

    property var lastChildren: []

    onChildrenChanged: {
        print("FigureContainer: Children changed!")
        print(figureContainer.children)

        var newChildren = []
        var addedChildren = []
        var removedChildren = lastChildren
        var c
        for (c in figureContainer.children) {
            newChildren.push(c)
            var ind = lastChildren.indexOf(c)
            if (ind >= 0)
                removedChildren.pop(ind)
            else
                addedChildren.push(c)
        }

        for (c in addedChildren) {
            childAdded(c)
        }

        for (c in removedChildren) {
            childRemoved(c)
        }

        lastChildren = newChildren
    }

    function childAdded(child) {
        print("ChildAdded:", child)
//        tabView.addFigure(child)
    }

    function childRemoved(child) {
//        tabView.closeFigure(child)
    }
}
