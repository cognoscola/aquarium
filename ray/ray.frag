#version 410

in vec2 baseTexCoords;

out vec4 out_Color;

uniform sampler2D baseMap;

uniform vec2 resolution;
uniform float globalTime;
uniform float life; //deteremined by the lifespan
uniform vec3 skyColour;

in float visibility;

float rayStrength(vec2 raySource, vec2 rayRefDirection, vec2 coord, float seedA, float seedB, float speed)
{
	vec2 sourceToCoord = coord - raySource;
	float cosAngle = dot(normalize(sourceToCoord), rayRefDirection);

	return clamp((0.45  + 0.65 * sin(cosAngle * seedA + globalTime * speed)) +
		(0.1 + 0.5 * cos(-cosAngle * seedB + globalTime * speed)),
		0.0, 1.0) *
		clamp((resolution.x - length(sourceToCoord)) / resolution.x, 0.9, 1.0);
}

void main () {

//    out_Color = texture(baseMap, baseTexCoords);
//    vec4 light = texture(luminanceMap, luminanceTexCoords);

    //luminance mixture
//    out_Color = mix(out_Color, light, light.r);

    //fog mixture
//    out_Color = mix(vec4(skyColour, 1.0),out_Color,visibility);

    vec2 uv = baseTexCoords.xy/ resolution.xy;
    uv.y = 1.0 - uv.y;
    vec2 coord = vec2(baseTexCoords.x, resolution.y - baseTexCoords.y);

// Set the parameters of the sun rays
	vec2 rayPos1 = vec2(resolution.x * 0.4, resolution.y * -1.5); //position of center
	vec2 rayRefDir1 = normalize(vec2(1.0, -0.116)); //spin direction
	float raySeedA1 = 36.2214;
	float raySeedB1 = 21.11349;
	float raySpeed1 = 1; //spin speed

	vec2 rayPos2 = vec2(resolution.x * 0.6, resolution.y * -1.5);
    vec2 rayRefDir2 = normalize(vec2(1.0, 0.241));
    const float raySeedA2 = 22.39910;
    const float raySeedB2 = 18.0234;
    const float raySpeed2 = 1;

	// Calculate the colour of the sun rays on the current fragment
	vec4 rays1 =
		vec4(1.0, 1.0, 1.0, 1.0 * rayStrength(rayPos1, rayRefDir1, coord, raySeedA1, raySeedB1, raySpeed1));

	vec4 rays2 =
		vec4(1.0, 1.0, 1.0, 1.0 * rayStrength(rayPos2, rayRefDir2, coord, raySeedA2, raySeedB2, raySpeed2));


	out_Color = rays1 * 0.6 + rays2 * 0.6;
	out_Color.a *= 0.5;

	float fadeVertical;

    if(baseTexCoords.y < 0.5 ){
        fadeVertical = 1-((coord.y - 0.8) / (resolution.y - 0.8) );
    }else if(baseTexCoords.y > 0.5  ){
        fadeVertical = ((  coord.y ) / (  resolution.y - 0.8 ));
    }else {
        fadeVertical = 1;
    }
    fadeVertical = clamp(fadeVertical, 0.0, 1.0);

	float fadeHorizontal;
	if(baseTexCoords.x > 0.5 ){
        fadeHorizontal = 1- ((coord.x - 0.8) / (resolution.x - 0.8) );
    }else if(baseTexCoords.x < 0.5  ){
        fadeHorizontal = ((  coord.x ) / (  resolution.x - 0.8 ));
    }else {
        fadeHorizontal = 1;
    }
    fadeHorizontal = clamp(fadeHorizontal, 0.0, 1.0);

    if((baseTexCoords.x > 0.8 && baseTexCoords.y > 0.8) ||
       (baseTexCoords.x < 0.2 && baseTexCoords.y > 0.8) ||
       (baseTexCoords.x < 0.2 && baseTexCoords.y < 0.2) ||
       (baseTexCoords.x > 0.8 && baseTexCoords.y < 0.2) )
    {
        out_Color.a *= fadeHorizontal * fadeVertical;
    }else{
        out_Color.a *=  min(fadeHorizontal, fadeVertical);
    }

    out_Color.a *= clamp(sin(life),0.0,1.0);

    //fog effect
//    out_Color = mix(vec4(skyColour, out_Color.a),out_Color,visibility);
    out_Color.a *=  visibility;

}


