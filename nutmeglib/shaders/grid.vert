//#version 150

attribute vec4 vertexPosition;
attribute vec3 vertexNormal;

varying vec3 worldNormal;
varying vec3 viewDirection;
varying vec3 worldPosition;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;
uniform mat4 modelView;

void main(void)
{
    worldPosition = vec3(modelMatrix * vertexPosition);
    worldNormal = normalize( modelNormalMatrix * vertexNormal );
//    worldNormal = normalize(vertexNormal);
    viewDirection = normalize( vec3(modelView * vec4(1,0,0,0)) );

    gl_Position = modelViewProjection * vertexPosition;
//    qt_TexCoord0 = qt_MultiTexCoord0;
}
