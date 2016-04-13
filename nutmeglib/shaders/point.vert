in vec4 vertexPosition;
in vec2 vertexTexCoord;

out vec2 texCoord;

uniform mat4 modelMatrix;

void main(void)
{
    gl_Position = modelMatrix * vertexPosition;
    texCoord = vertexTexCoord;
}
