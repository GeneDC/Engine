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

	camera.SetPos({ 0, 2, 10, 1 });

	float aspectRatio = (float)width / (float)height;
	float viewAngle = glm::pi<float>() * 0.25f;
	float nearClip = 1.0f;
	float farClip = 1000.0f;
	camera.SetProjectionMatrix(glm::perspective(viewAngle, aspectRatio, nearClip, farClip));

	shader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simpleTexture.vert");
	shader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simpleTexture.frag");
	// Check if the shader loaded correctly
	if (shader.link() == false)
	{
		std::cout << "Shader Error: " << shader.getLastError() << std::endl;
		return false;
	}

	phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
	// Check if the shader loaded correctly
	if (phongShader.link() == false)
	{
		std::cout << "Shader Error: " << phongShader.getLastError() << std::endl;
		return false;
	}

	// Make sure the obj loaded correctly
	assert(Loader::LoadOBJ(soulSpear, "./assets/soulspear/soulspear.obj") == true);
	assert(Loader::LoadOBJ(quadMesh, "./quad.obj") == true);

	//quadMesh.InitialiseQuad();

	quadTransform = 
	{
		10,0,0,0,
		0,10,0,0,
		0,0,10,0,
		0,0,0,1 
	};

	modelTransform = glm::mat4(1);

	light.diffuse = { 1, 1, 0 };
	light.specular = { 1, 1, 0 };
	ambientLight = { 1, 1, 1 };

	moveSpeed = 10.0f;

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
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

	// rotate light
	light.direction = glm::normalize(glm::vec3(glm::cos(glfwGetTime() * 2),
		glm::sin(glfwGetTime() * 2), 0));

	// Close the app if esc is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		shouldExit = true;
}

void Application3D::Draw()
{
	// Clear gizmos
	Gizmos::clear();

	glm::mat4 tempTransform = camera.GetTransform();
	tempTransform[3] = { 0, 0, 0, 1 };
	Gizmos::addTransform(tempTransform, 3.0f);

	glm::vec4 white(0.5f, 0.5f, 0.5f, 1);
	glm::vec4 black(0.2f, 0.2f, 0.2f, 1);
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
	//float elapsedTime = (float)glfwGetTime();
	//std::cout << elapsedTime << std::endl;
	//shader.bindUniform("elapsedTime", elapsedTime);


	// bind transforms for lighting
	phongShader.bindUniform("NormalMatrix",	glm::inverseTranspose(glm::mat3(modelTransform)));

	// bind light
	phongShader.bindUniform("Ia", ambientLight);
	phongShader.bindUniform("Id", light.diffuse);
	phongShader.bindUniform("Is", light.specular);
	phongShader.bindUniform("LightDirection", light.direction);

	phongShader.bindUniform("cameraPosition", glm::vec3(camera.GetPosition()));

	// Bind the transform
	auto pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::mat4(1);
	phongShader.bindUniform("ProjectionViewModel", pvm);

	phongShader.bind();

	soulSpear.Draw();

	glm::mat4 mat = 
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		10,0,0,1
	};
	pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * mat;
	phongShader.bindUniform("ProjectionViewModel", pvm);

	soulSpear.Draw();

	//// Bind the transform
	//pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * quadTransform;
	//shader.bindUniform("ProjectionViewModel", pvm);

	glUseProgram(1);
	//shader.bind();

	//quadMesh.Draw();

}
