#version 410

in vec2 vTexCoord;

uniform sampler2D diffuseTexture;

out vec4 FragColour;

// simple box blur
vec4 BoxBlur(vec2 texCoord) 
{
vec2 texel = 1.0f / textureSize(diffuseTexture, 0);
// 9-tap box kernel
vec4 colour = texture(diffuseTexture, texCoord);
colour += texture(diffuseTexture, texCoord + texel * vec2(-1,1));
colour += texture(diffuseTexture, texCoord + texel * vec2(-1,0));
colour += texture(diffuseTexture, texCoord + texel * vec2(-1,-1));
colour += texture(diffuseTexture, texCoord + texel * vec2(0,1));
colour += texture(diffuseTexture, texCoord + texel * vec2(0,-1));
colour += texture(diffuseTexture, texCoord + texel * vec2(1,1));
colour += texture(diffuseTexture, texCoord + texel * vec2(1,0));
colour += texture(diffuseTexture, texCoord + texel * vec2(1,-1));
return colour / 9;
}

vec4 Distort(vec2 texCoord) {
vec2 mid = vec2(0.5f);
float distanceFromCentre = distance(texCoord, mid);
vec2 normalizedCoord = normalize(texCoord - mid);
float bias = distanceFromCentre +
sin(distanceFromCentre * 15) * 0.05f;
vec2 newCoord = mid + bias * normalizedCoord;
return texture(diffuseTexture, newCoord);
}

void main()
{
	// calculate texel size
	//vec2 texSize = textureSize( diffuseTexture, 0 );
	//vec2 texelSize = 1.0f / texSize;

	// adjust texture coordinate
	//vec2 scale = (texSize - texelSize) / texSize;
	//vec2 texCoord = vTexCoord / scale + texelSize * 0.5f;

	// Sample post effect
	FragColour =  Distort(vTexCoord);//BoxBlur(vTexCoord);//texture(diffuseTexture, vTexCoord);
}