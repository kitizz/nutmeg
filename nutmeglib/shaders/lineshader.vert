#version 330

in vec2 myvertex;

out VS_OUT {
    vec4 vColor;
} vs_out;

uniform mat4 modelView;
uniform vec4 lineColor;

void main()
{
//    texCoord = vertexTexCoord;
//    gl_Position = modelMatrix * vertexPosition;
    gl_Position = modelView * vec4(myvertex, 0.0, 1.0);
    vs_out.vColor = lineColor;
}
