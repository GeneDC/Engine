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
#include <imgui.h>
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

	lightDirection = 5.2f;
	lightDirection2 = 2.5f;

	camera.SetPos({ 0, 2, 10, 1 });

	float aspectRatio = (float)width / (float)height;
	float viewAngle = glm::pi<float>() * 0.25f;
	float nearClip = 0.01f;
	float farClip = 1000.0f;
	camera.SetProjectionMatrix(glm::perspective(viewAngle, aspectRatio, nearClip, farClip));

	simpleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simpleTexture.vert");
	simpleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simpleTexture.frag");
	// Check if the shader loaded correctly
	if (simpleShader.link() == false)
	{
		std::cout << "Shader Error: " << simpleShader.getLastError() << std::endl;
		return false;
	}

	phongTexShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phongTex.vert");
	phongTexShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phongTex.frag");
	// Check if the shader loaded correctly
	if (phongTexShader.link() == false)
	{
		std::cout << "Shader Error: " << phongTexShader.getLastError() << std::endl;
		return false;
	}

	// Make sure the obj loaded correctly
	assert(Loader::LoadOBJ(soulSpear, "./assets/soulspear/soulspear.obj") == true);
	//assert(Loader::LoadOBJ(rock, "./assets/Rock_6/Rock_6.OBJ") == true);
	assert(Loader::LoadOBJ(dragon, "./assets/stanford/Dragon.obj") == true);

	quadMesh.InitialiseQuad();

	quadTransform = 
	{
		5,0,0,0,
		0,5,0,0,
		0,0,5,0,
		0,0.01f,0,1 
	};

	modelTransform = 
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		2,0,0,1
	};

	dragonTransform =
	{
		0.5f,0,0,0,
		0,0.5f,0,0,
		0,0,0.5f,0,
		-4,0,0,1
	};

	light.diffuse = { 1, 1, 0.8f };
	light.specular = { 1, 1, 0.8f };
	ambientLight = { 0.1f, 0.1f, 0.08f };

	moveSpeed = 10.0f;
	
	// Create the render target and make sure it worked
	if (renderTarget.Create(2, width,height, Texture::Format::RGB16F) == false)
	{
		printf("Render Target Error!\n");
		return false;
	}	fullScreenQuad.InitialiseFullscreenQuad();
	postShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simplePost.vert");
	postShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/simplePost.frag");
	// Check if the shader loaded correctly
	if (postShader.link() == false)
	{
		std::cout << "Shader Error: " << postShader.getLastError() << std::endl;
		return false;
	}

	blurShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/simplePost.vert");
	blurShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/gaussianBlur.frag");
	// Check if the shader loaded correctly
	if (blurShader.link() == false)
	{
		std::cout << "Shader Error: " << blurShader.getLastError() << std::endl;
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

	// Create the render target and make sure it worked
	if (blurTarget.Create(2, width, height, Texture::Format::RGB16F) == false)
	{
		printf("Render Target Error!\n");
		return false;
	}

	return true;
}

void Application3D::Shutdown()
{
	// Destroy Gizmos
	Gizmos::destroy();
}

void Application3D::Update(const float & a_deltaTime)
{
	//Input* input = Input::getInstance();

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

	//mousePos = { input->getMouseX(), input->getMouseY() };

	//glm::vec2 centreMousePos = glm::vec2{ width >> 1, height >> 1 } - mousePos;

	//auto rot = glm::angleAxis(centreMousePos.x * 0.05f * a_deltaTime, glm::vec3{ 0, 1, 0 });
	////camera.GetTransform() *= glm::mat4_cast(rot);

	//rot *= glm::angleAxis(-centreMousePos.y * 0.05f * a_deltaTime, glm::vec3{ 1, 0, 0 });
	//camera.GetTransform() *= glm::mat4_cast(rot);

	//auto& forward = cameraTransform[2];
	//auto& right	 = cameraTransform[0];
	//right.y = 0.0f;
	//right = glm::normalize(right);
	//
	//glm::vec4 newUp = { glm::cross(glm::vec3(forward), glm::vec3(right)), 0 };
	//
	//cameraTransform[0] = right;
	//cameraTransform[1] = newUp;
	//cameraTransform[2] = forward;

	// Set the mouse to be at the centre of the screen
	//glfwSetCursorPos(window, width >> 1, height >> 1);

	// rotate light
	light.direction = glm::normalize(glm::vec3(glm::cos(lightDirection), glm::sin(lightDirection), 0));
	light2.direction = glm::normalize(glm::vec3(0, glm::sin(lightDirection2), glm::cos(lightDirection2)));

	// Close the app if esc is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		shouldExit = true;
}

void Application3D::Draw()
{
	// Bind the render target
	renderTarget.Bind();
	// Make sure you clear screen after you bind the render target
	ClearScreen();

	// Bind the phong shader
	phongTexShader.bind();
	// Bind transforms for lighting
	phongTexShader.bindUniform("NormalMatrix",	glm::inverseTranspose(glm::mat3(modelTransform)));

	// Bind light
	phongTexShader.bindUniform("Ia", ambientLight);
	phongTexShader.bindUniform("Id", light.diffuse);
	phongTexShader.bindUniform("Is", light.specular);
	phongTexShader.bindUniform("LightDirection", light.direction);
	phongTexShader.bindUniform("LightDirection2", light2.direction);
	// Bind the camera position
	phongTexShader.bindUniform("cameraPosition", glm::vec3(camera.GetPosition()));

	// Bind the transform
	auto pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::mat4(1);
	phongTexShader.bindUniform("ProjectionViewModel", pvm);
	// Draw the spear to the render target
	soulSpear.Draw();

	//// Bind the transform
	//pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * modelTransform;
	//phongTexShader.bindUniform("ProjectionViewModel", pvm);
	//soulSpear.Draw();

	glm::mat4 mat = 
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		2,0,0,1
	};
	pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * mat;
	phongTexShader.bindUniform("ProjectionViewModel", pvm);
	
	rock.Draw();
	
	quadMesh.GetMaterials()[0] = soulSpear.GetMaterials()[0];
	//quadMesh.GetMaterials()[0].diffuseTexture = renderTarget.GetTarget(0);

	// Bind the phong shader
	phongShader.bind();
	// Bind transforms for lighting
	phongShader.bindUniform("NormalMatrix", glm::inverseTranspose(glm::mat3(modelTransform)));

	static glm::vec3 dragonColour = glm::vec3(1, 0, 0);
	phongShader.bindUniform("colour", dragonColour);

	// Bind light
	phongShader.bindUniform("Ia", ambientLight);
	phongShader.bindUniform("Id", light.diffuse);
	phongShader.bindUniform("Is", light.specular);
	phongShader.bindUniform("LightDirection", light.direction);
	phongShader.bindUniform("LightDirection2", light2.direction);
	// Bind the camera position
	phongShader.bindUniform("cameraPosition", glm::vec3(camera.GetPosition()));

	// Bind the transform
	pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * dragonTransform;
	phongShader.bindUniform("ProjectionViewModel", pvm);

	dragon.Draw();

	//// Now use the simple shader
	//simpleShader.bind();
	//
	//// Bind the transform
	//pvm = camera.GetProjectionMatrix() * camera.GetViewMatrix() * quadTransform;
	//simpleShader.bindUniform("ProjectionViewModel", pvm);
	//
	//quadMesh.Draw();
	//
	//// Gizmos stuff
	//{ 
	//	// Clear gizmos
	//	Gizmos::clear();
	//
	//	glm::mat4 tempTransform = camera.GetTransform();
	//	tempTransform[3] = { 0, 0, 0, 1 };
	//	Gizmos::addTransform(tempTransform, 3.0f);
	//
	//	glm::vec4 white(0.5f, 0.5f, 0.5f, 1);
	//	glm::vec4 black(0.2f, 0.2f, 0.2f, 1);
	//	for (int i = 0; i < 21; ++i)
	//	{
	//		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
	//			glm::vec3(-10 + i, 0, -10),
	//			i == 10 ? white : black);
	//		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
	//			glm::vec3(-10, 0, -10 + i),
	//			i == 10 ? white : black);
	//	}
	//
	//	Gizmos::draw(camera.GetProjectionMatrix() * camera.GetViewMatrix());
	//}

	// Unbind the render target to return to backbuffer
	renderTarget.Unbind();
	// Clear the backbuffer after unbinding the shader
	ClearScreen();

	bool horizontal = true, first_iteration = true;
	static int amount = 5;
	// Bind the blur render target
	blurTarget.Bind();
	// Make sure you clear screen after you bind the render target
	ClearScreen();

	blurShader.bind();
	blurShader.bindUniform("dist", amount);
	for (int i = 0; i < amount; i++)
	{
		blurShader.bindUniform("horizontal", horizontal);
		//glBindTexture(GL_TEXTURE_2D, first_iteration ? renderTarget.GetTarget(1).GetHandle() : blurTarget.GetTarget(!horizontal).GetHandle());
		fullScreenQuad.GetMaterials()[0].diffuseTexture = first_iteration ? renderTarget.GetTarget(1) : blurTarget.GetTarget(!horizontal);
		fullScreenQuad.Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	// Unbind the blur render target to return to backbuffer
	blurTarget.Unbind();
	// Clear the backbuffer after unbinding the shader
	ClearScreen();

	// Start post processing
	// Bind post shader
	postShader.bind();
	static float exposure = 10.0f;
	postShader.bindUniform("exposure", exposure);
	static float gamma = 0.5f;
	postShader.bindUniform("gamma", gamma);

	static int target = 0;
	//postShader.bindUniform("colourTarget", 0);
	fullScreenQuad.GetMaterials()[0].diffuseTexture = renderTarget.GetTarget(target);
	static bool blur = true;
	if (blur)
		fullScreenQuad.GetMaterials()[0].specularTexture = blurTarget.GetTarget(1);
	else
		fullScreenQuad.GetMaterials()[0].specularTexture = Texture();
	//fullScreenQuad.GetMaterials()[0].diffuseTexture = soulSpear.GetMaterials()[0].diffuseTexture;
	//renderTarget.GetTarget(0).Bind(0);
	fullScreenQuad.Draw();

	ImGui::Begin("Settings");
	ImGui::Text("Use W, A, S, D, Left Shift, and Space to move the camera");
	ImGui::SliderFloat("Exposure", &exposure, 0.01f, 20.0f);
	ImGui::SliderFloat("Gamma", &gamma, 0.4f, 3.0f);
	ImGui::InputInt("Render Target", &target, 1, 1);
	if (target > 1) target = 1;
	if (target < 0) target = 0;
	ImGui::InputInt("Blur amount", &amount, 1, 1);
	if (amount > 8) amount = 8;
	if (amount < 1) amount = 1;
	ImGui::SliderFloat("Light Direction", &lightDirection, 0, glm::pi<float>() * 2);
	ImGui::SliderFloat("Light 2 Direction", &lightDirection2, 0, glm::pi<float>() * 2);
	ImGui::Checkbox("Use Bloom", &blur);
	ImGui::ColorEdit3("Dragon colour", &dragonColour[0]);
	////auto tex = soulSpear.GetMaterials()[0].diffuseTexture.GetHandle();
	//auto tex = renderTarget.GetTarget(1).GetHandle();
	//// Ask ImGui to draw it as an image:
	//// Under OpenGL the ImGUI image type is GLuint
	//// So make sure to use "(void *)tex" but not "&tex"
	//ImGui::GetWindowDrawList()->AddImage(
	//	(void *)tex, ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y),
	//	ImVec2((float)(width), (float)(height)), ImVec2(0, 1), ImVec2(1, 0));
	
	ImGui::End();

}
