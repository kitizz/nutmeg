import QtQuick 2.4
import Nutmeg 1.0

import "Util.js" as Util

ControllerBase {

    property bool enableDebugging: true
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
            var figure = figures[args[0]]
            if (!figure) return null
            figure = figure[0]
            debug("Figure: " + figure)
        }
        debug("Figure axes:" + figure.axes)

        if (N > 1) {
            var axes = getAxesFromFigure(figure, args[1])
            if (N == 2) return axes

            var plots = getDataFromAxis(axes, args[2])
            debug("Plots found: " + plots)
            if (N == 3) return plots

            var nextObjs = plots
            for (var i=3; i<N; ++i) {
                nextObjs = getFromObj(nextObjs, args[i])
            }
            return nextObjs

        } else {
            return figure
        }
    }

    function getAxesFromFigure(fig, handle) {
        debug("AxisArg: " + handle + " - " + Util.dir(fig.axes))
        // This is a little bit crazy, but basically supports python-style fancy indexing

        var axes = fig.axes
        var matches = handle.match(/(.*?)\[(.*?)\]/) // handle[index]

        if (matches) {
            handle = matches[1]
            if (!axes[handle])
                return null

            var index = matches[2]

            var range = index.match(/^(\d*)\:(\d*)$/) // index = s:e

            if (range) {
                var s = (range[1].length == 0) ? 0 : range[1]
                var e = (range[2].length == 0) ? undefined : range[2]
                return axes[handle].slice(s, e)

            } else {
                debug("axes["+handle+"]: " + axes[handle])
                return axes[handle][index]
            }
        } else {
            if (!axes[handle]) {
                return null
            } else {
                var result = axes[handle]
                if (result.length == 1)
                    return result[0]
                else
                    return result
            }
        }
    }

    /*!
     * \function getDataFromAxis(axes, handle)
     * Attempt to get the plot objects from \a axes with the handles, \a handle
     * Axes can be a single Axis object, or an array of Axes.
     * If not all Axes provided have a plot called, handle, this function will
     * attemp return the properties of the Axis/Axes called, handle.
     */
    function getDataFromAxis(axes, handle) {
//        debug("PlotDataArg: " + handle)
//        debug("Datas: " + Util.dir(axis.plots))

        if (Util.isArray(axes)) {
            var plots = []
            for (var i=0; i<axes.length; ++i) {
                var plot = axes[i].plots[handle]
                if (!plot)
                    return getFromObj(axes, handle)

                plots.push(plot)
            }
            return plots

        } else {
            var plot = axes.plots[handle]
            debug("Plot! " + plot)
            if (!plot)
                return getFromObj(axes, handle)
            return plot
        }
    }

    function getFromObj(obj, handle) {
//        debug("PlotDataArg: " + handle)
//        debug("Datas: " + Util.dir(axis.plots))
        if (!obj)
            return null

        if (Util.isArray(obj)) {
            var datas = []
            for (var i=0; i<obj.length; ++i) {
                datas.push(getFromObj(obj[i], handle))
            }

            debug("Datas: " + datas)
            return datas
        } else {
            debug("Getting " + handle + " from " + obj)
            var propName = obj.mapProperty(handle)
            if (!propName) return null
            return obj[propName]
        }
    }

//    /// This is a little bit crazy, but basically supports python-style fancy indexing
//    function getFromObj(obj, handle) {
//        if (!obj) return null

//        var matches = handle.match(/(.*?)\[(.*?)\]/) // handle[index]

//        if (matches) {
//            handle = matches[1]
//            if (!obj[handle])
//                return null

//            var index = matches[2]

//            var range = index.match(/^(\d*)\:(\d*)$/) // index = s:e

//            if (range) {
//                var s = (range[1].length == 0) ? 0 : range[1]
//                var e = (range[2].length == 0) ? undefined : range[2]
//                return obj[handle].slice(s, e)

//            } else {
//                debug("obj["+handle+"]: " + obj[handle])
//                return obj[handle][index]
//            }
//        } else {
//            if (!obj[handle])
//                return null
//            else
//                return obj[handle]
//        }
//    }
}
