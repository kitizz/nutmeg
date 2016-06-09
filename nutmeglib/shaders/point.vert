//#version 150 core

attribute vec4 vertexPosition;
uniform float pointSize;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjection;

void main(void)
{
    gl_PointSize = pointSize;
    gl_Position = modelViewProjection*vertexPosition;
//    texCoord = vertexTexCoord;
}
