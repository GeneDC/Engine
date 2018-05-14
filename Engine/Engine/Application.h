#pragma once

struct GLFWwindow;

class Application
{
public:
	Application() : window(nullptr), shouldExit(false) {};
	virtual ~Application() = default;

	// Sets up the application and runs it until exited
	// Cleans itself up after exiting
	// @param title The name/title of the window
	// @param width The width in pixels of the window
	// @param height The height in pixels of the window
	// @return If the creation was successful, false if failed
	bool Run(const char * title, int width, int height);

	// These need to be implemented by a derived class
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(const float& deltaTime) = 0;
	virtual void Draw() = 0;

	// Sets shouldExit to true which then will exit the application
	void Exit() { shouldExit = true; }

private:
	
	// Sets up the application 
	// Creates a glfw window with the title, with and height
	bool Create(const char* title, int width, int height);
	// Shuts down the application destroying the necessary things
	void Destroy();

	// The glfw window for this application
	GLFWwindow * window;
	// If the application should exit
	bool shouldExit;
};

