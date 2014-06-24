import QtQuick 2.0
import Graphr 1.0

import "Util.js" as Util

ControllerBase {

    property bool enableDebugging: false
    /// Fetch a plot component based on its location, relative to top
    /// Example: figure[0].ax[1].data
    function debug(s) {
        if (!enableDebugging) return
        console.log(s)
    }

    function get(arg) {
        debug("\nFigures: " + Util.dir(figures))
        var args = arg.split('.')
        debug("Args: " + args)
        var N = args.length

        if (N > 0) {
            var figure = getFromObj(figures, args[0])
            debug("Figure: " + figure)
        }
        if (!figure) return null

        if (N > 2) {
            return getAxesFromFigure(figure, args[1], args[2])
        } else if (N > 1) {
            return getAxesFromFigure(figure, args[1])
        } else {
            return figure
        }
    }

    function getAxesFromFigure(fig, handle, dataHandle) {
        debug("AxisArg: " + handle + " - " + Util.dir(fig.axes))
        var axes = getFromObj(fig.axes, handle)

        if ((typeof(dataHandle) === 'undefined') || (dataHandle.length == 0) ||
                (!axes) ) //|| (!Util.isArray(axes))
            return axes
        // Another test for array:  Object.prototype.toString.call(axes) === '[Object array]'
        // May cause bugs if axis ever gets 'length' property
        if (Util.isArray(axes)) {
            var datas = []
            for (var i=0; i<axes.length; ++i) {
                datas.push(getDataFromAxis(axes[i], dataHandle))
            }

            debug("Datas: " + datas)
            return datas
        } else {
            return getDataFromAxis(axes, dataHandle)
        }
    }

    function getDataFromAxis(axis, handle) {
        debug("PlotDataArg: " + handle)
        debug("Datas: " + Util.dir(axis.plots))
        var data = getFromObj(axis.plots, handle)
        debug("Data: " + data)
        return data
    }


    /// This is a little bit crazy, but basically supports python-style fancy indexing
    function getFromObj(obj, handle) {
        if (!obj) return null

        var matches = handle.match(/(.*?)\[(.*?)\]/) // handle[index]

        if (matches) {
            handle = matches[1]
            if (!obj[handle])
                return null

            var range = matches[2].match(/^(\d*)\:(\d*)$/) // index = s:e

            if (range) {
                var s = (range[1].length == 0) ? 0 : range[1]
                var e = (range[2].length == 0) ? undefined : range[2]
                return obj[handle].slice(s, e)

            } else {
                debug("obj["+handle+"]: " + obj[handle])
                return obj[handle][matches[2]]
            }
        } else {
            if (!obj[handle])
                return null
            else
                return obj[handle][0]
        }
    }
}
