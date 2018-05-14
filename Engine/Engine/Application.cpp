#include "Application.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

// std
#include <iostream>

bool Application::Create(const char * title, int width, int height)
{
	// Initialize glfw and if it failed, stop the program
	if (glfwInit() == false)
		return false;

	// Create a glfw window with our settings
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
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

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	std::cout << "GL: " << major << "." << minor << std::endl;

	// Success
	return true;
}

bool Application::Run(const char * title, int width, int height)
{
	// Setup the application and return false if it failed
	if (Create(title, width, height) == false)
		return false;

	if (Startup() == false)
		return false;

	// Run until the window should be closed (e.g. pressing the x button)
	while (glfwWindowShouldClose(window) == false && shouldExit == false)
	{
		glClearColor(0, 0.15f, 0.15f, 1.0f);
		glEnable(GL_DEPTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		// Update
		// Close the app if esc is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			shouldExit = true;
		}
		// Draw

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// Clean up the application
	Destroy();
	// Finished successfully
	return true;
}

void Application::Destroy()
{
	// Destroy the window we created
	glfwDestroyWindow(window);
	// Clean up glfw
	glfwTerminate();
}
