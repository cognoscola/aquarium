#version 410

layout(location = 0)in vec3 positions;
layout(location = 1)in vec2 normals;
layout(location = 2)in vec2 textureCoords;

out vec2 pass_textureCoords;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform vec4 plane;

const float tiling = 16;

void main(void){

    vec4 vsPos = vec4 (positions, 1.0);
    gl_Position = projectionMatrix  *  viewMatrix * modelMatrix * vsPos;

    pass_textureCoords = textureCoords;
//     pass_textureCoords = vec2(textureCoords.x/2.0 + 0.5, textureCoords.y/2.0 + 0.5) *tiling ;
    gl_ClipDistance[0] = dot(plane,vsPos);

}