#pragma once

#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#pragma warning(pop)

class Camera
{
public:
	Camera() : worldTransform(glm::mat4(1)), projection(glm::mat4(1)), hasSetProjectionMatrix(false) {};
	virtual ~Camera() = default;

	//virtual void Update() = 0;

	// Adds to the position of the camera
	void Translate(glm::vec4 a_vec) { worldTransform[3] += a_vec; };

#pragma region Setters & Getters

	// Sets the position of the camera
	void SetPos(glm::vec4 a_position) { worldTransform[3] = a_position; };
	// Returns the position of the camera
	glm::vec4 GetPosition() const { return worldTransform[3]; };
	// Returns the world transform of the camera
	const glm::mat4& GetTransform() const { return worldTransform; }
	// Returns the world transform of the camera
	glm::mat4& GetTransform() { return worldTransform; }
	// Returns the view matrix of the camera
	glm::mat4 GetViewMatrix() const { return glm::inverse(worldTransform); }
	// Sets the projection matrix
	void SetProjectionMatrix(const glm::mat4& projectionMatrix);

	const glm::mat4& GetProjectionMatrix() const;
	//glm::mat4& GetProjectionMatrix() { return projection; };

#pragma endregion

private:
	// The transform of this camera
	glm::mat4 worldTransform;
	// The projection matrix for the camera
	glm::mat4 projection;
	// If the projection matrix has been set
	bool hasSetProjectionMatrix;

};

