#pragma once

#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <glm/glm.hpp>
#pragma warning(pop)

class Mesh
{
public:
	Mesh() : triCount(0), vao(0), vbo(0), ibo(0) {};
	virtual ~Mesh();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 texCoord;
	};

	void Initialise(const unsigned int& vertexCount, const Vertex* verts,
		const unsigned int& indexCount = 0, const unsigned int* indices = nullptr);

	// Make this mesh a quad
	void InitialiseQuad();
	// Draw this mesh
	virtual void Draw();

protected:
	// How many tris this mesh has
	unsigned int triCount;
	unsigned int vao, vbo, ibo;
};

