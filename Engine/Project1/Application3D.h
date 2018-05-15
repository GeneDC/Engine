#pragma once
#include "Application.h"
#include "Camera.h"
#include "Mesh.h"

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

	aie::ShaderProgram shader;
	Mesh quadMesh;
	glm::mat4 quadTransform;
};

