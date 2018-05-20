#pragma once
#include "Texture.h"

#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <glm/glm.hpp>
#pragma warning(pop)

#include <string>

struct Material
{
public:

	Material() : name(""), ambient(1), diffuse(1), specular(0), transmittance(0)
		, emission(0), shininess(1), ior(1), dissolve(1), illum(0)
		{}
	~Material() = default;

	std::string name;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 transmittance;
	glm::vec3 emission;

	float shininess;
	float ior;      // index of refraction
	float dissolve; // 1 == opaque; 0 == fully transparent
	int illum;

	Texture diffuseTexture;				// bound slot 0
	Texture alphaTexture;				// bound slot 1
	Texture ambientTexture;				// bound slot 2
	Texture specularTexture;			// bound slot 3
	Texture specularHighlightTexture;	// bound slot 4
	Texture normalTexture;				// bound slot 5
	Texture displacementTexture;		// bound slot 6
};