#version 410

in vec3 textureCoords;
in float visibility;
out vec4 out_Color;

uniform samplerCube cubeMap;
//uniform vec3 skyColour;

void main () {
     out_Color = texture(cubeMap, textureCoords);
//     out_Color = mix(vec4(skyColour, 1.0),out_Color,visibility);

}