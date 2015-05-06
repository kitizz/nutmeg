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

function isDefined(obj) {
    return typeof obj !== 'undefined'
}

function sign(x) {
    return typeof x === 'number' ? x ? x < 0 ? -1 : 1 : x === x ? 0 : NaN : NaN;
}

function mean(lst) {
    var m = 0
    for (var i=0; i<lst.length; ++i) {
        m += lst[i]
    }
    return m/lst.length
}

function std(lst) {
    var m = mean(lst)
    var v = 0
    for (var i=0; i<lst.length; ++i) {
        v += Math.pow(lst[i]-m, 2)
    }
    return Math.sqrt(v/lst.length)
}

function min(lst) {
    var bestM = lst[0], bestI = 0
    for (var i=1; i<lst.length; ++i) {
        if (lst[i] < bestM) {
            bestM = lst[i]
            bestI = i
        }
    }
    return bestM
}

function max(lst) {
    var bestM = lst[0], bestI = 0
    for (var i=1; i<lst.length; ++i) {
        if (lst[i] > bestM) {
            bestM = lst[i]
            bestI = i
        }
    }
    return bestM
}

function copyRect(r) {
    return Qt.rect(r.x, r.y, r.width, r.height)
}

function distance3d(v1, v2) {
    var dx = v1.x - v2.x, dy = v1.y - v2.y, dz = v1.z - v2.z
    return Math.sqrt(dx*dx + dy*dy + dz*dz)
}
