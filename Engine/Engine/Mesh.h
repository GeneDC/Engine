#pragma once
#include "Material.h"

#pragma warning(push)
#pragma warning( disable : 4201)
#pragma warning( disable : 4310)
#include <glm/glm.hpp>
#pragma warning(pop)

// std
#include <vector>

class Mesh
{
public:
	Mesh() {};
	virtual ~Mesh();

	struct Vertex
	{
		glm::vec4 position;	// added to attrib location 0
		glm::vec4 normal;	// added to attrib location 1
		glm::vec2 texCoord;	// added to attrib location 2
		// normal-mapping data
		glm::vec4 tangent;	// added to attrib location 3
	};

	struct Chunk 
	{
		unsigned int	vao, vbo, ibo;
		unsigned int	indexCount;
		int				materialID;
	};

	// Initilaize the mesh with the specificed data
	void Initialise(const unsigned int& vertexCount, const Vertex* verts,
		const unsigned int& indexCount = 0, const unsigned int* indices = nullptr);

	// Initialize the mesh with mesh chunks and materials
	void Initialise(std::vector<Chunk>& meshChunks, std::vector<Material>& materials, bool copy = false);

	// Make this mesh a quad
	void InitialiseQuad();
	// Draw this mesh
	virtual void Draw();

	bool Empty() const { return meshChunks.size() == 0; }

	std::vector<Material>& GetMaterials() { return materials; }

private:
	std::string filePath;
	std::vector<Chunk> meshChunks;
	std::vector<Material> materials;
};

