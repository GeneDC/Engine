#pragma once
#include "Application.h"

class Application3D : public Application
{
public:
	Application3D();
	virtual ~Application3D();

	virtual bool Startup();
	virtual void Shutdown();
	virtual void Update(const float& deltaTime);
	virtual void Draw();

};

