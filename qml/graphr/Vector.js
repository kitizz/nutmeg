.pragma library

function point(obj) {
    if (obj.x && obj.y)
        return Qt.point(obj.x, obj.y)
    else
        return Qt.point(0,0)
}

function add(p1, p2) {
    return Qt.point(p1.x + p2.x, p1.y + p2.y)
}

function subtract(p1, p2) {
    return Qt.point(p1.x - p2.x, p1.y + p2.y)
}

function scale(s, p) {
    return Qt.point(s*p.x, s*p.y)
}

function length(p) {
    return Math.sqrt(length2(p))
}

function length2(p) {
    return p.x*p.x + p.y*p.y
}

function distance(p1, p2) {
    // Skip the other function calls for performance...
    var x = p1.x-p2.x, y = p1.y-p2.y
    return Math.sqrt(x*x + y*y)
}

function distance2(p1, p2) {
    // Skip the other function calls for performance...
    var x = p1.x-p2.x, y = p1.y-p2.y
    return x*x + y*y
}

