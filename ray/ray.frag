#version 410

in vec2 baseTexCoords;

out vec4 out_Color;

uniform sampler2D baseMap;
//uniform sampler2D luminanceMap;
//uniform vec3 skyColour;
//in float visibility;

uniform vec2 resolution;
uniform float globalTime;

float rayStrength(vec2 raySource, vec2 rayRefDirection, vec2 coord, float seedA, float seedB, float speed)
{
	vec2 sourceToCoord = coord - raySource;
	float cosAngle = dot(normalize(sourceToCoord), rayRefDirection);

	return clamp((0.45 + 0.15 * sin(cosAngle * seedA + globalTime * speed)) +
		(0.3 + 0.2 * cos(-cosAngle * seedB + globalTime * speed)),
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
	vec2 rayPos1 = vec2(resolution.x * 0.4, resolution.y * -2); //position of center
	vec2 rayRefDir1 = normalize(vec2(1.0, -0.116)); //spin direction
	 float raySeedA1 = 36.2214;
	 float raySeedB1 = 21.11349;
	 float raySpeed1 = 1.5; //spin speed

	vec2 rayPos2 = vec2(resolution.x * 0.6, resolution.y * -2);
    vec2 rayRefDir2 = normalize(vec2(1.0, 0.241));
    const float raySeedA2 = 22.39910;
    const float raySeedB2 = 18.0234;
    const float raySpeed2 = 1.1;

	// Calculate the colour of the sun rays on the current fragment
	vec4 rays1 =
		vec4(1.0, 1.0, 1.0, 1.0 * rayStrength(rayPos1, rayRefDir1, coord, raySeedA1, raySeedB1, raySpeed1));

	vec4 rays2 =
		vec4(1.0, 1.0, 1.0, 1.0 * rayStrength(rayPos2, rayRefDir2, coord, raySeedA2, raySeedB2, raySpeed2));


	out_Color = rays1 * 0.6 + rays2 * 0.6;

//    out_Color.a *= 0.5;
	// Attenuate brightness towards the bottom, simulating light-loss due to depth.
	// Give the whole thing a blue-green tinge as well.

	float fade;
    if(baseTexCoords.y < 0.5  ){
        fade = 1-((coord.y - 0.8) / (resolution.y - 0.8) );
    }else if(baseTexCoords.y > 0.5  ){
        fade = ((  coord.y ) / (  resolution.y - 0.8 ));
    }else {
        fade = 1;
    }
    fade = clamp(fade, 0.0, 1.0);

//	float brightnessWidth  = 1.0 - (coord.x / resolution.x );

//	if(baseTexCoords.y < 0.5  ){
	    out_Color.a *=  fade;

//	}else if (baseTexCoords.y > 0.5){
//	    out_Color.a *=  1 - bottomFade;
//	}

  /* if( baseTexCoords.x > 0.5){
   	    out_Color.a *= brightnessWidth;
   }else if(baseTexCoords.x < 0.5){
   	    out_Color.a *=  1- brightnessWidth;
   }*/

//	out_Color.x *= 0.1 + (brightness * 0.8);
//	out_Color.y *= 0.3 + (brightness * 0.6);
//	out_Color.z *= 0.5 + (brightness * 0.5);

}


