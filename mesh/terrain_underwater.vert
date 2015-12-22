#version 410

layout(location = 0)in vec3 positions;
layout(location = 2)in vec2 textureCoords;
layout(location = 1)in vec2 normals;

out vec2 pass_textureCoords;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec4 plane;

const float tiling = 8;

void main(void){

    vec4 vsPos = vec4 (positions, 1.0);
    gl_Position = projectionMatrix  *  viewMatrix * modelMatrix * vsPos;

//    textureCoords = vec2(positions.x/2.0 + 0.5, positions.y/2.0 + 0.5) *tiling ;
    pass_textureCoords = textureCoords;

    gl_ClipDistance[0] = dot(plane,vsPos);
}