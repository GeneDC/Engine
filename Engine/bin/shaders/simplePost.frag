#version 410

in vec2 vTexCoord;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

out vec4 FragColour;

uniform float exposure;
uniform float gamma;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

//// simple box blur
//vec4 BoxBlur(vec2 texCoord)
//{
//	vec2 texel = 1.0f / textureSize(diffuseTexture, 0);
//	// 9-tap box kernel
//	vec4 colour = texture(diffuseTexture, texCoord);
//	colour += texture(diffuseTexture, texCoord + texel * vec2(-1,1));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(-1,0));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(-1,-1));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(0,1));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(0,-1));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(1,1));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(1,0));
//	colour += texture(diffuseTexture, texCoord + texel * vec2(1,-1));
//	return colour / 9;
//}
//
//vec4 Distort(vec2 texCoord)
//{
//	vec2 mid = vec2(0.5f);
//	float distanceFromCentre = distance(texCoord, mid);
//	vec2 normalizedCoord = normalize(texCoord - mid);
//	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.05f;
//	vec2 newCoord = mid + bias * normalizedCoord;
//	return texture(diffuseTexture, newCoord);
//}
//

void main()
{
	// calculate texel size
	//vec2 texSize = textureSize( diffuseTexture, 0 );
	//vec2 texelSize = 1.0f / texSize;

	// adjust texture coordinate
	//vec2 scale = (texSize - texelSize) / texSize;
	//vec2 texCoord = vTexCoord / scale + texelSize * 0.5f;

	vec3 hdrColor = texture(diffuseTexture, vTexCoord).rgb;
	vec3 bloomColor = texture(specularTexture, vTexCoord).rgb;
	hdrColor += bloomColor; // additive blending
	// Exposure tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction 
	mapped = pow(mapped, vec3(1.0 / gamma));
	
	FragColour = vec4(mapped, 1.0);//Distort(vTexCoord);//BoxBlur(vTexCoord);//texture(diffuseTexture, vTexCoord);
}