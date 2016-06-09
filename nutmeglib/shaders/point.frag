//#version 150 core

//out vec4 fragColor;

uniform vec4 plotColor;

void main(void)
{
//    float l = length(gl_PointCoord - 0.5);
//    float a = step(0.5, 1.0-l);
//    fragColor = vec4(plotColor.rgb, a * plotColor.a);
    gl_FragColor = plotColor;
}

