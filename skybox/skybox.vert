#version 410

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0)in vec3 positions;
out vec3 textureCoords;
//out float visibility;

//const float density = 0.008;
//const float gradient = 1.5;

void main () {

   vec4 positionRelativeToCam = viewMatrix * modelMatrix * vec4 (positions, 1.0);

   gl_Position = projectionMatrix * positionRelativeToCam;
   textureCoords= positions;

//   float distance = length(positionRelativeToCam.xyz);
//   visibility = exp(-pow((distance*density),gradient));
//   visibility = clamp(visibility,0.0,1.0);

}