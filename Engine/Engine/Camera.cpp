#include "Camera.h"

void Camera::SetProjectionMatrix(const glm::mat4& projectionMatrix)
{
	hasSetProjectionMatrix = true;
	projection = projectionMatrix;
}

const glm::mat4 & Camera::GetProjectionMatrix() const 
{

	// Throw a error if the projection matrix wasn't set
	assert(hasSetProjectionMatrix == true);
	return projection;

}
