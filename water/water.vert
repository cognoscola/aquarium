#version 410

layout(location = 0)in vec3 positions;

uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

out vec2 textureCoords;
out vec4 clipSpace;
out vec3 toCameraVector;
out vec3 fromLightVector;
out float visibility;

const float tiling = 8;
uniform float fogDensity;
uniform float fogGradient;

//Fesnel effect stuff
uniform vec3 cameraPosition;
uniform vec3 lightPosition; //

void main(void){

    vec4 worldPosition = viewMatrix * modelMatrix * vec4(positions,1.0);
    clipSpace = projectionMatrix * worldPosition;
    gl_Position = clipSpace;
    textureCoords = vec2(positions.x/2.0 + 0.5, positions.y/2.0 + 0.5) *tiling ;

    //fresnel stuff
    toCameraVector = cameraPosition - worldPosition.xyz;
    fromLightVector = worldPosition.xyz - lightPosition;

    //fog efffect
    float distance = length(worldPosition.xyz);
    visibility = exp(-pow((distance*fogDensity),fogGradient));
    visibility = clamp(visibility,0.0,1.0);

}