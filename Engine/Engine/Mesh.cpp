#include "Mesh.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>

#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>

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

	unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };

	Initialise(4, verts, 6, indices);
}

void Mesh::Draw()
{
	glBindVertexArray(vao);
	// Using indices or just vertices?
	if (ibo != 0)
	{
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
}

bool Mesh::LoadOBJ(Mesh & mesh, const char * path)
{
	std::ifstream inStream;
	(path);
	if (!inStream) {
		std::cout << "Cannot open file [" << path << "]" << std::endl;
		return false;
	}

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	int maxchars = 8192;                                  // Alloc enough size.
	std::vector<char> buf(static_cast<size_t>(maxchars)); // Alloc enough size.
	while (inStream.peek() != -1) {
		inStream.getline(&buf[0], maxchars);

		std::string linebuf(&buf[0]);

		// Trim newline '\r\n' or '\n'
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\n')
				linebuf.erase(linebuf.size() - 1);
		}
		if (linebuf.size() > 0) {
			if (linebuf[linebuf.size() - 1] == '\r')
				linebuf.erase(linebuf.size() - 1);
		}

		// Skip if empty line.
		if (linebuf.empty()) {
			continue;
		}

		// Skip leading space.
		const char *token = linebuf.c_str();
		token += strspn(token, " \t");

		assert(token);
		if (token[0] == '\0')
			continue; // empty line

		if (token[0] == '#')
			continue; // comment line

					  // vertex
		if (token[0] == 'v' && isSpace((token[1]))) {
			token += 2;
			float x, y, z;
			parseFloat3(x, y, z, token);
			v.push_back(x);
			v.push_back(y);
			v.push_back(z);
			continue;
		}

		// normal
		if (token[0] == 'v' && token[1] == 'n' && isSpace((token[2]))) {
			token += 3;
			float x, y, z;
			parseFloat3(x, y, z, token);
			vn.push_back(x);
			vn.push_back(y);
			vn.push_back(z);
			continue;
		}

		// texcoord
		if (token[0] == 'v' && token[1] == 't' && isSpace((token[2]))) {
			token += 3;
			float x, y;
			parseFloat2(x, y, token);
			vt.push_back(x);
			vt.push_back(y);
			continue;
		}

		// face
		if (token[0] == 'f' && isSpace((token[1]))) {
			token += 2;
			token += strspn(token, " \t");

			std::vector<vertex_index> face;
			while (!isNewLine(token[0])) {
				vertex_index vi = parseTriple(token, static_cast<int>(v.size() / 3),
					static_cast<int>(vn.size() / 3),
					static_cast<int>(vt.size() / 2));
				face.push_back(vi);
				size_t n = strspn(token, " \t\r");
				token += n;
			}

			faceGroup.push_back(face);

			continue;
		}

	return false;
}
