#version 410

in vec2 baseTexCoords;
in vec2 luminanceTexCoords;

out vec4 out_Color;

uniform sampler2D baseMap;
uniform sampler2D luminanceMap;
uniform vec3 skyColour;

in float visibility;

void main () {

    out_Color = texture(baseMap, baseTexCoords);
    vec4 light = texture(luminanceMap, luminanceTexCoords);

    //luminance mixture
    out_Color = mix(out_Color, light, light.r);

    //fog mixture
    out_Color = mix(vec4(skyColour, 1.0),out_Color,visibility);

}