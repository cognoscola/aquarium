#version 410

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0)in vec3 positions;

out vec3 textureCoords;

//fog stuff
out float visibility;
uniform float fogDensity;
uniform float fogGradient;

void main () {

   vec4 positionRelativeToCam = viewMatrix * modelMatrix * vec4 (positions, 1.0);

   gl_Position = projectionMatrix * positionRelativeToCam;
   textureCoords= positions;

    //claculate visibilty of the skybox
   float distance = length(positionRelativeToCam.xyz);
   visibility = exp(-pow((distance*fogDensity),fogGradient));
   visibility = clamp(visibility,0.0,1.0);

}