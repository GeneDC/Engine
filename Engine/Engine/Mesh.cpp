#include "Mesh.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>


Mesh::~Mesh()
{
	// Delete the arrays and buffers
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void Mesh::Initialise(const unsigned int & vertexCount, const Vertex * verts, const unsigned int & indexCount, const unsigned int * indices)
{
	// Insure that this mesh hasn't been initialized yet
	assert(vao == 0);

	// Generate buffers
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	// Bind vertex array aka a mesh wrapper
	glBindVertexArray(vao);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Enable third element as texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);

	// Bind indices if there are any
	if (indexCount != 0)
	{
		glGenBuffers(1, &ibo);
		// Bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		// Fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Vertex), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else
		triCount = vertexCount / 3;

	// Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::InitialiseQuad()
{
	// Define 6 vertices for 2 tris
	Vertex verts[6];
	verts[0].position = { -0.5f, 0, 0.5f, 1.0f };
	verts[1].position = { 0.5f, 0, 0.5f, 1.0f };
	verts[2].position = { -0.5f, 0, -0.5f, 1.0f };
	verts[3].position = { 0.5f, 0, -0.5f, 1.0f };

	// Set the texture coords for the Quad
	verts[0].texCoord = { 0, 1 }; // bottom left
	verts[1].texCoord = { 1, 1 }; // bottom right
	verts[2].texCoord = { 0, 0 }; // top left
	verts[3].texCoord = { 1, 0 }; // top right

	// Set the indices for the order of the verts to be drawn in
	unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };

	Initialise(4, verts, 6, indices);
}

void Mesh::Draw()
{
	glBindVertexArray(vao);
	// Using indices or just vertices?
	if (ibo != 0)
	{
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
}
