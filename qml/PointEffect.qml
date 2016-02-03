import Qt3D 2.0
import Qt3D.Renderer 2.0

Effect {
    id: pointEffect

    techniques: [
        Technique {
            openGLFilter {
                api: OpenGLFilter.Desktop
                profile: OpenGLFilter.Core
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

