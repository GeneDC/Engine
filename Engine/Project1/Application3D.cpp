#include "Application3D.h"

#include "Loader.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

// aie
#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <aie/Gizmos.h>
#include <glm/ext.hpp>
#pragma warning(pop)

#include <aie/Input.h>

#include <iostream>

using namespace aie;

Application3D::Application3D()
{
}

Application3D::~Application3D()
{
}

bool Application3D::Startup()
{
	// Create gizmos
	Gizmos::create(30000, 30000, 30000, 30000);

	camera.SetPos({ 0, 0, 10, 1 });

	float aspectRatio = (float)width / (float)height;
	float viewAngle = glm::pi<float>() * 0.25f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
	camera.SetProjectionMatrix(glm::perspective(viewAngle, aspectRatio, nearClip, farClip));

	shader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simple.vert");
	shader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simple.frag");
	// Check if the shader loaded correctly
	if (shader.link() == false)
	{
		std::cout << "Shader Error: " << shader.getLastError() << std::endl;
		return false;
	}

	//quadMesh.InitialiseQuad();

	// Make sure the obj loaded correctly
	assert(Loader::LoadOBJ(quadMesh, "./cube.OBJ") == true);

	//// Define 8 vertices for 12 tris
	//Mesh::Vertex verts[8];
	//verts[0].position = { 1.f, -1.0f, -1.0f, 1.0f };
	//verts[1].position = { 1.f, -1.0f, 1.0f, 1.0f };
	//verts[2].position = { -1.f, -1.0f, 1.0f, 1.0f };
	//verts[3].position = { -1.f, -1.0f, -1.0f, 1.0f };
	//verts[4].position = { 1.f, 1.0f, -1.0f, 1.0f };
	//verts[5].position = { 1.f, 1.0f, 1.0f, 1.0f };
	//verts[6].position = { -1.f, 1.0f, 1.0f, 1.0f };
	//verts[7].position = { -1.f, 1.0f, -1.0f, 1.0f };

	//unsigned int indices[36] = 
	//{
	//	5, 1, 4,
	//	5, 4, 8,
	//	3, 7, 8,
	//	3, 8, 4,
	//	2, 6, 3,
	//	6, 7, 3,
	//	1, 5, 2,
	//	5, 6, 2,
	//	5, 8, 6,
	//	8, 7, 6, 
	//	1, 2, 3,
	//	1, 3, 4
	//};

	//for (size_t i = 0; i < 36; i++)
	//{
	//	indices[i]--;
	//}

	//quadMesh.Initialise(8, verts, 36, indices);

	// make the quad 5 units wide
	quadTransform = 
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 
	};

	moveSpeed = 10.0f;

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glEnable(GL_DEPTH);

	return true;
}

void Application3D::Shutdown()
{
	// Destroy Gizmos
	Gizmos::destroy();
}

void Application3D::Update(const float & a_deltaTime)
{
	Input* input = Input::getInstance();

	glm::mat4& cameraTransform = camera.GetTransform();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Translate(cameraTransform[2] * -moveSpeed * a_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Translate(cameraTransform[2] * moveSpeed * a_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Translate(cameraTransform[0] * moveSpeed * a_deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Translate(cameraTransform[0] * -moveSpeed * a_deltaTime);

	// Move up with space bar relative to the camera
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.Translate(cameraTransform[1] * moveSpeed * a_deltaTime);
	// Move down with left shift relative to the camera
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.Translate(cameraTransform[1] * -moveSpeed * a_deltaTime);

	mousePos = { input->getMouseX(), input->getMouseY() };

	glm::vec2 centreMousePos = glm::vec2{ width >> 1, height >> 1 } - mousePos;

	auto rot = glm::angleAxis(centreMousePos.x * 0.05f * a_deltaTime, glm::vec3{ 0, 1, 0 });
	//camera.GetTransform() *= glm::mat4_cast(rot);

	rot *= glm::angleAxis(-centreMousePos.y * 0.05f * a_deltaTime, glm::vec3{ 1, 0, 0 });
	camera.GetTransform() *= glm::mat4_cast(rot);

	//auto& forward = cameraTransform[2];
	//auto& right	 = cameraTransform[0];
	//right.y = 0.0f;
	//right = glm::normalize(right);

	//glm::vec4 newUp = { glm::cross(glm::vec3(forward), glm::vec3(right)), 0 };
	//
	//cameraTransform[0] = right;
	//cameraTransform[1] = newUp;
	//cameraTransform[2] = forward;

	// Set the mouse to be at the centre of the screen
	glfwSetCursorPos(window, width >> 1, height >> 1);


	// Close the app if esc is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		shouldExit = true;
}

void Application3D::Draw()
{
	// Clear gizmos
	Gizmos::clear();

	// Bind the shader
	shader.bind();

	float elapsedTime = (float)glfwGetTime();
	std::cout << elapsedTime << std::endl;
	shader.bindUniform("elapsedTime", elapsedTime);

	// Bind the transform
	auto pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * quadTransform;
	shader.bindUniform("ProjectionViewModel", pvm);

	quadMesh.Draw();

	Gizmos::addTransform(camera.GetTransform(), 3.0f);
	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
			glm::vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
			glm::vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::draw(camera.GetProjectionMatrix() * camera.GetViewMatrix());
}
