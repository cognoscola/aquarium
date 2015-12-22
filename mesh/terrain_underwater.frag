#version 410

in vec2 pass_textureCoords;
out vec4 out_Color;

uniform sampler2D baseMap;
uniform sampler2D luminanceMap;

void main () {

//    out_Color = texture(baseMap, pass_textureCoords);
    out_Color  = texture(luminanceMap, pass_textureCoords);

}