#include "Application3D.h"
// aie
#include <aie/Gizmos.h>


Application3D::Application3D()
{
}

Application3D::~Application3D()
{
}

bool Application3D::Startup()
{
	// Create gizmos
	aie::Gizmos::create(30000, 30000, 30000, 30000);

	return true;
}

void Application3D::Shutdown()
{
	// Destroy Gizmos
	aie::Gizmos::destroy();
}

void Application3D::Update(const float & deltaTime)
{
}

void Application3D::Draw()
{
	// Clear gizmos
	aie::Gizmos::clear();
}
