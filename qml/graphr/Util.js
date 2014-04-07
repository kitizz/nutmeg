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
