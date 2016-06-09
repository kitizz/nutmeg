//#version 150 core

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
//varying float vertexSide;

uniform vec2 pixelSize;
uniform float linewidth;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjection;

void main(void)
{
    vec4 current = modelViewProjection*vec4(vertexPosition, 1.0);
    vec4 next = modelViewProjection*vec4(vertexPosition + vertexNormal, 1.0);

    vec2 currentScreen = current.xy / current.w;
    vec2 nextScreen = next.xy / next.w;

    vec2 tangent = normalize(nextScreen - currentScreen);
    vec2 normal = linewidth * pixelSize * vec2(-tangent.y, tangent.x);  // Rotate left

    gl_Position = current/current.w + vec4(normal, 0.0, 0.0);
    gl_PointSize = 1.0;
}
