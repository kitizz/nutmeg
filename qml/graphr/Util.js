.pragma library

function dir(obj) {
    var list = []
    for (var p in obj)
        list.push(p)
    return list
}

function isArray(obj) {
    return Object.prototype.toString.call(obj) === '[object Array]'
}

function isObject(obj) {
    return typeof obj === 'object'
}

function getTestFigure() {
    return 'Figure {
    id: figure
    Axis {
        handle: "ax"
//        xAxis.majorTicks: [0,2,5,10,20]
//        xAxis.onMajorTicksChanged: console.log("Major X:", xAxis.majorTicks, yAxis.majorTicks)
//        yAxis.onMajorTicksChanged: console.log("Major Y:", yAxis.majorTicks)
        LinePlot {
            handle: "blue"
            line { color: "#7777FF"; style: "-"; width: 4 }
        }
        LinePlot {
            handle: "red"
            line { color: "#FF7777"; style: "--"; width: 4 }
        }
        LinePlot {
            handle: "green"
            line { color: "#77FF77"; style: ":"; width: 4 }
        }
    }
}'
}
