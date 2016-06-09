//#version 150

uniform vec4 planeColor;
uniform vec3 eyePosition;

varying vec3 worldNormal;
varying vec3 worldPosition;
varying vec3 viewDirection;
//out vec4 fragColor;

void main(void)
{
//    float alignment = abs(dot(worldNormal, viewDirection));
    float alignment = 0.7 + 0.3*dot(normalize(eyePosition), worldNormal);
    gl_FragColor = vec4(alignment*planeColor.rgb, planeColor.a);
}
