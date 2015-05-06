import QtQuick 2.2

import Qt3D 2.0 as Qt3D
import Qt3D.Render 2.0

import Nutmeg 1.0

import "Util.js" as Util

PointCloudBase {
    handle: "pointCloud"

    width: parent.width
    height: parent.width

    property var rootEntity: null

    property real pointSize: 3

    onXDataChanged: updateData()
    onYDataChanged: updateData()
    onZDataChanged: updateData()
    onRootEntityChanged: updateData()


    // ------- INTERNAL ---------
    property var points: []

    function updateData() {
        if (!rootEntity)
            return

        var N = xData.length
        console.log(N + " points in point cloud...")
        if (yData.length != N || zData.length != N) {
            console.log("Data still updating...")
            return
        }

        var i, newDot;
        for (i = 0; i < N; ++i) {
            console.log("Creating dot under " + rootEntity + " at " + xData[i] + ", " + yData[i] + ", " + zData[i])
            var p = Qt.vector3d(xData[i], yData[i], zData[i])
            newDot = dot.createObject(rootEntity, { "dx": p.x, "dy": p.y, "dz": p.z,
                                          "scaling": Qt.binding(function() {
                                                return pointSize*Util.distance3d(p, axis.cameraPosition)/width
                                          })
                                        })
        }
    }

//    Qt3D.Entity {
//        id: rootEntity
//    }

    Component {
        id: dot
        Qt3D.Entity {
            id: dotEnt
            property alias dx: translate.dx
            property alias dy: translate.dy
            property alias dz: translate.dz
            property real scaling: 1.0
            objectName: "dot"
            components: {
                console.log("Setting components:", material.objectName)
                return [ sphere, material, trans ]
            }
            Qt3D.Transform {
                id: trans;
                Qt3D.Translate { id: translate }
                Qt3D.Scale {
                    id: scale
                    scale3D: Qt.vector3d( dotEnt.scaling, dotEnt.scaling, dotEnt.scaling )
                }
            }

            SphereMesh {
                id: sphere
                radius: 0.5
                rings: 6
                slices: 5
            }

            PhongMaterial {
                id: material
                ambient: "black"
                diffuse: "black"
                specular: "black"
//                enabled: counter !== 1
            }

//            Material {
//                id: material
//                objectName: "pointMaterial"
//                effect: PointEffect {}
//                parameters: [
//                    Parameter { name: "color1"; value: Qt.vector3d( 1.0, 0.0, 0.0 ) },
//                    Parameter { name: "diffuse"; value: Qt.rgba( 1.0, 0.0, 0.0, 1.0 ) }
//                ]
//            }

//            Material {
//                id: material
//                effect : Effect {
//                }
//            }
        }
    }
}

