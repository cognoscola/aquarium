#version 410

layout(location = 0)in vec3 pos;

uniform mat4 projectionMatrix;

void main(void){
    gl_Position = projectionMatrix * vec4(pos,1.0);

}