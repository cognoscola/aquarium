#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 normals;
layout(location = 2)in vec2 textureCoords;

out vec2 baseTexCoords;
out vec2 luminanceTexCoords;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec4 plane;

const float tiling = 16;

//fog stuff
out float visibility;
uniform float fogDensity;
uniform float fogGradient;


void main(void){

    vec4 vsPos = vec4 (positions, 1.0);
    vec4 positionRelativeToCam = viewMatrix * modelMatrix * vsPos;
    gl_Position = projectionMatrix  * positionRelativeToCam;

    baseTexCoords = textureCoords;
    luminanceTexCoords = vec2(textureCoords.x/2.0 + 0.5, textureCoords.y/2.0 + 0.5) *tiling;

    gl_ClipDistance[0] = dot(plane,vsPos);

    float distance = length(positionRelativeToCam.xyz);
    visibility = exp(-pow((distance*fogDensity),fogGradient));
    visibility = clamp(visibility,0.0,1.0);


}