#include "Application3D.h"

int main()
{
	Application3D* app = new Application3D();

	// Run the application
	if (app->Run("Engine", 1280, 720) == false)
		return -1;

	delete app;

	return 0;
}