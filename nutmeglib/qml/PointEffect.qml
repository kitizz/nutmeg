import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: pointEffect

    techniques: [
        Technique {
            graphicsApiFilter {
                api: GraphicsApiFilter.OpenGL
                profile: GraphicsApiFilter.CoreProfile
                majorVersion: 3
                minorVersion: 2
            }

            annotations: Annotation { name: "renderingStyle"; value: "forward" }

            renderPasses: [
                RenderPass {
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource( "qrc:/shaders/point.vert" )
                        fragmentShaderCode: loadSource( "qrc:/shaders/point.frag" )
                    }
                }
            ]
        }
    ]
}

