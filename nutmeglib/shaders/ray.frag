//#version 150 core

//in vec2 texCoord;
//out vec4 fragColor;

uniform vec4 plotColor;

void main(void)
{
    gl_FragColor = plotColor;
}

