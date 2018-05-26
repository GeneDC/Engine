#pragma once

struct GLFWwindow;

class Application
{
public:
	Application() : window(nullptr), shouldExit(false), width(100), height(100)
		, clearColour{0, 0.15f, 0.15f, 10.f}, skipIfNotFocus(true), skipIfMinimized(true) {};
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

protected:
	
	// Sets up the application 
	// Creates a glfw window with the title, with and height
	bool Create(const char* title, int width, int height);
	// Shuts down the application destroying the necessary things
	void Destroy();

	// Clears the screen for the next frame to be rendered
	void ClearScreen();

	// The glfw window for this application
	GLFWwindow * window;

	// Width of the screen in pixels
	int width;
	// Height of the screen in pixels
	int height;
	// If the application should exit
	bool shouldExit;
private:
	// The amount of updates this frame
	unsigned int fps;
	// The render clear colour
	float clearColour[4];
	// The time since last frame
	double deltaTime;
	// If the application should not update if it is not the focus
	bool skipIfNotFocus;
	// If the application should not update if it is minimized
	bool skipIfMinimized;
};

