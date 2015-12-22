#version 410

in vec2 pass_textureCoords;
out vec4 out_Color;

uniform sampler2D textureSampler;
uniform sampler2D luminance;

void main () {

    out_Color = texture(luminance, pass_textureCoords);
//    out_Color = texture(textureSampler, pass_textureCoords);
//    out_Color.a = light.r;

}