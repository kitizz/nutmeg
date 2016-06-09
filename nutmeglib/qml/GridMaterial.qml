import Qt3D.Render 2.0

Material {
    id: mat
    property color planeColor: Qt.rgba(0.7, 0.7, 0.7, 0.7)

    parameters: [
        Parameter { name: "planeColor"; value: planeColor }
    ]

    effect: Effect {
        id: gridEffect

        techniques: [
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.NoProfile
                    majorVersion: 2
                    minorVersion: 0
                }

                annotations: Annotation { name: "renderingStyle"; value: "forward" }

                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource( "qrc:/shaders/grid.vert" )
                            fragmentShaderCode: loadSource( "qrc:/shaders/grid.frag" )
                        }
                    }
                ]
            }
        ]
    }
}
