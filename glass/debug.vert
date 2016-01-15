#version 410

layout(location = 0)in vec3 pos;

uniform mat4 projectionMatrixT;

void main(void){
    gl_Position = projectionMatrixT * vec4(pos,1.0);
}