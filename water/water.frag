#version 410

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCameraVector;
in vec3 fromLightVector;
in float visibility; //for fog effect

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform vec3 lightColour;
uniform vec3 skyColour;//for fog effect

uniform float moveFactor;
uniform vec3 cameraOrientation;

const float waveStrength = 0.04;
const float shineDamper = 20.0;
const float reflectivity = 0.5;

const float near = 0.1;
const float far  = 200.0f;

void main () {

    //perform perspective division on the clip space coordinate and
    //convert from texture coordinates to screen space coordinates
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);//a reflection is usually upside down, so negate the y component

    //get the water depth by finding out the distance between the water's height
    //and the bottom of the water floor. We do this by getting depth
    //information from the refraction frame buffer and subtracting by
    //depth information taken from the default frame buffer.
    float depth = texture(depthMap, refractTexCoords).r;
    float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    depth = gl_FragCoord.y;
    float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    float waterDepth = floorDistance - waterDistance;

    //calculate the amount of distortion based on the values represented by the distortion map
    // and add the result to the texture
    vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg*0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength *  clamp(waterDepth/20.0, 0.0, 1.0);

    refractTexCoords += totalDistortion;
    reflectTexCoords += totalDistortion;

    vec4 reflectColour = texture(reflectionTexture,reflectTexCoords);
    vec4 refractColour = texture(refractionTexture,refractTexCoords);

    //sample the normal map  for lighting effects
    vec4 normalMapColour = texture(normalMap,distortedTexCoords);
    vec3 normal = vec3(normalMapColour.r * 2.0 - 1.0 , normalMapColour.b * 3.0, normalMapColour.g * 2.0 - 1.0);
    normal = normalize(normal);

    //calculate a vector between the water quad and the camera
    vec3 viewVector = normalize(toCameraVector);

    //use the viewvector to calculate the fresnel effect
    //the refractive factor indicates how much of the of the colour in the quad should be
    //from the reflection and how much should be from refraction texture
    float refractiveFactor = dot( viewVector,normal);

    //speculiar lighting calculation
	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColour * specular * reflectivity * clamp(waterDepth/1.0, 0.0, 1.0);;

    out_Color = mix(reflectColour, refractColour,refractiveFactor);
    out_Color = mix(out_Color, vec4(0.0,0.3,0.5,1.0),0.2) + vec4(specularHighlights, 0.0);
    out_Color.a = clamp(waterDepth/1.0, 0.0, 1.0);
    out_Color.a = visibility;
}
