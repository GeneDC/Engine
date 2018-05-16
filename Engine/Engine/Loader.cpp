#include "Loader.h"

#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>

bool Loader::LoadOBJ(Mesh & mesh, const char * path)
{
	std::ifstream inStream(path);
	if (!inStream)
	{
		std::cout << "Cannot open file [" << path << "]" << std::endl;
		return false;
	}

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_verts;
	std::vector< glm::vec3 > normals;
	std::vector< glm::vec2 > uvs;
	std::vector<std::vector<vertex_index> > faceGroup;

	int maxchars = 8192;                                  // Alloc enough size.
	std::vector<char> buf(static_cast<size_t>(maxchars)); // Alloc enough size.
	while (inStream.peek() != -1)
	{
		inStream.getline(&buf[0], maxchars);

		std::string linebuf(&buf[0]);

		// Trim newline '\r\n' or '\n'
		if (linebuf.size() > 0)
			if (linebuf[linebuf.size() - 1] == '\n')
				linebuf.erase(linebuf.size() - 1);

		if (linebuf.size() > 0)
			if (linebuf[linebuf.size() - 1] == '\r')
				linebuf.erase(linebuf.size() - 1);

		// Skip if empty line.
		if (linebuf.empty()) continue;

		// Skip leading space.
		const char *token = linebuf.c_str();
		token += strspn(token, " \t");

		assert(token);
		if (token[0] == '\0')	continue; // empty line

		if (token[0] == '#')	continue; // comment line

		// vertex
		if (token[0] == 'v' && isSpace((token[1]))) {
			token += 2;
			glm::vec3 vec;
			parseVec3(vec, token);
			temp_verts.push_back(vec);
			continue;
		}

		// normal
		if (token[0] == 'v' && token[1] == 'n' && isSpace((token[2]))) {
			token += 3;
			glm::vec3 vec;
			parseVec3(vec, token);
			normals.push_back(vec);
			continue;
		}

		// texcoord
		if (token[0] == 'v' && token[1] == 't' && isSpace((token[2]))) {
			token += 3;
			glm::vec2 vec;
			parseVec2(vec, token);
			uvs.push_back(vec);
			continue;
		}

		// face
		if (token[0] == 'f' && isSpace((token[1]))) {
			token += 2;
			token += strspn(token, " \t");

			std::vector<vertex_index> face;
			while (!isNewLine(token[0]))
			{
				vertex_index vi = parseTriple(token, static_cast<int>(temp_verts.size() / 3),
					static_cast<int>(normals.size() / 3),
					static_cast<int>(uvs.size() / 2));
				face.push_back(vi);
				size_t n = strspn(token, " \t\r");
				token += n;
			}

			faceGroup.push_back(face);

			continue;
		}

		continue;
		//return false;
	}

	// Apply to mesh
	const unsigned int vertexCount = (unsigned int)temp_verts.size();
	Mesh::Vertex* verts = new Mesh::Vertex[vertexCount];

	for (unsigned int i = 0; i < vertexCount; i++)
		verts[i].position = { std::move(temp_verts[i]), 1 };


	const unsigned int indexCount = (unsigned int)faceGroup.size() * 3;
	unsigned int* indices = new unsigned int[indexCount];

	for (size_t i = 0; i < faceGroup.size(); i += 3)
	{
		indices[i] = std::move(faceGroup[i][0].v_idx--);
		indices[i + 1] = std::move(faceGroup[i][1].v_idx--);
		indices[i + 2] = std::move(faceGroup[i][2].v_idx--);
}
	
	mesh.Initialise(vertexCount, verts, indexCount, indices);

	return true;
}
