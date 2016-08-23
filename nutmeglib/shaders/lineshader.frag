#version 330

//in vec2 texCoord;
in vec4 fColor;
in float t;

out vec4 fragColor;

uniform float thickness;

void main()
{
    float edge = 1.0 - 2.0*abs(t - 0.5);
    float alpha = clamp(edge * thickness, 0, 1);
//    float alpha = 1;
    fragColor = vec4(fColor.rgb, alpha);
}
