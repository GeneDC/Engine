#pragma once
#include "Application.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "RenderTarget.h"

#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <glm/glm.hpp>
#pragma warning(pop)

// aie
#include <aie/Shader.h>

class Application3D : public Application
{
public:
	Application3D();
	virtual ~Application3D();

	virtual bool Startup();
	virtual void Shutdown();
	virtual void Update(const float& deltaTime);
	virtual void Draw();

private:
	Camera camera;

	glm::vec2 mousePos;

	float moveSpeed;

	aie::ShaderProgram simpleShader;
	Mesh quadMesh;
	glm::mat4 quadTransform;

	aie::ShaderProgram phongTexShader;
	Mesh soulSpear;
	glm::mat4 modelTransform;

	Mesh rock;

	struct Light 
	{
		glm::vec3 direction;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	Light light;	glm::vec3 ambientLight;

	RenderTarget renderTarget;

	Mesh fullScreenQuad;

	aie::ShaderProgram postShader;

	aie::ShaderProgram blurShader;

	RenderTarget blurTarget;

	float lightDirection;

	aie::ShaderProgram phongShader;
	Mesh dragon;
	glm::mat4 dragonTransform;
};

