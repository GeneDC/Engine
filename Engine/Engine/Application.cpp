#include "Application.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

// std
#include <iostream>

#include <aie/Input.h>

bool Application::Create(const char * title, int a_width, int a_height)
{
	width = a_width;
	height = a_height;

	// Initialize glfw and if it failed, stop the program
	if (glfwInit() == false)
		return false;

	// Create a glfw window with our settings
	window = glfwCreateWindow(a_width, a_height, title, nullptr, nullptr);
	// Check if the window was created correctly
	if (window == false)
	{
		glfwTerminate(); // Make sure to terminate first
		return false;
	}

	// Tells opengl & glfw that this is the window to use
	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	// Enable Face culling
	glEnable(GL_CULL_FACE);

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	aie::Input::create();

	// Success
	return true;
}

bool Application::Run(const char * title, int a_width, int a_height)
{
	// Setup the application and return false if it failed
	if (Create(title, a_width, a_height) == false)
		return false;

	// Startup and check if it worked
	if (Startup() == false)
		return false;

	// Variables for timing
	double prevTime = glfwGetTime();
	double currTime = 0;
	deltaTime = 0;
	unsigned int frames = 0;
	double fpsInterval = 0;

	// Run until the window should be closed (e.g. pressing the x button)
	while (glfwWindowShouldClose(window) == false && shouldExit == false)
	{
		// Get the current time
		currTime = glfwGetTime();
		// Set deltaTime to the change in time
		deltaTime = currTime - prevTime;
		// Set the previous time
		prevTime = currTime;

		// clear input
		aie::Input::getInstance()->clearStatus();
		glfwPollEvents();

		// Set the clear colour
		glClearColor(0, 0.15f, 0.15f, 1.0f);
		glEnable(GL_DEPTH);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Update fps every second
		frames++;
		fpsInterval += deltaTime;
		if (fpsInterval >= 1.0f) 
		{
			fps = frames;
			frames = 0;
			fpsInterval -= 1.0f;
		}

		// Call Update
		Update((float)deltaTime);
		// Call Draw
		Draw();

		glfwSwapBuffers(window);
	}

	// Clean up the application
	Destroy();
	// Finished successfully
	return true;
}

void Application::Destroy()
{
	aie::Input::destroy();

	// Destroy the window we created
	glfwDestroyWindow(window);
	// Clean up glfw
	glfwTerminate();
}
