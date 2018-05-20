#pragma once
#include "Mesh.h"

#include <string>
#include <map>

#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include "gl_core_4_4.h"

#define TINYOBJLOADER_IMPLEMENTATION
//#pragma warning(push)
//#pragma warning( disable : 4706)
#include "tiny_obj_loader.h"
//#pragma warning(pop)

#include <fstream>
#include <cassert>

namespace Loader
{
	void CalculateTangents(std::vector<Mesh::Vertex>& vertices, const std::vector<unsigned int>& indices) {
		unsigned int vertexCount = (unsigned int)vertices.size();
		glm::vec4* tan1 = new glm::vec4[vertexCount * 2];
		glm::vec4* tan2 = tan1 + vertexCount;
		memset(tan1, 0, vertexCount * sizeof(glm::vec4) * 2);

		unsigned int indexCount = (unsigned int)indices.size();
		for (unsigned int a = 0; a < indexCount; a += 3) {
			long i1 = indices[a];
			long i2 = indices[a + 1];
			long i3 = indices[a + 2];

			const glm::vec4& v1 = vertices[i1].position;
			const glm::vec4& v2 = vertices[i2].position;
			const glm::vec4& v3 = vertices[i3].position;

			const glm::vec2& w1 = vertices[i1].texCoord;
			const glm::vec2& w2 = vertices[i2].texCoord;
			const glm::vec2& w3 = vertices[i3].texCoord;

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			glm::vec4 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r, 0);
			glm::vec4 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r, 0);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (unsigned int a = 0; a < vertexCount; a++) {
			const glm::vec3& n = glm::vec3(vertices[a].normal);
			const glm::vec3& t = glm::vec3(tan1[a]);

			// Gram-Schmidt orthogonalize
			vertices[a].tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0);

			// Calculate handedness (direction of bitangent)
			vertices[a].tangent.w = (glm::dot(glm::cross(glm::vec3(n), glm::vec3(t)), glm::vec3(tan2[a])) < 0.0F) ? 1.0F : -1.0F;

			// calculate bitangent (ignoring for our Vertex, here just for reference)
			//vertices[a].bitangent = glm::vec4(glm::cross(glm::vec3(vertices[a].normal), glm::vec3(vertices[a].tangent)) * vertices[a].tangent.w, 0);
			//vertices[a].tangent.w = 0;
		}

		delete[] tan1;
	}

	template<typename T>
	void SerializeVector(std::ofstream& out, std::vector<T>& vec)
	{
		// Make sure the file was opened
		assert(out.is_open());
		size_t size = vec.size();
		// Write the size
		out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // If the size is 0 don't write anything
		out.write(reinterpret_cast<char*>(&vec[0]), size * sizeof(T));
	}

	template<typename T>
	void DeserializeVector(std::ifstream& in, std::vector<T>& vec)
	{
		// Make sure the file was opened
		assert(in.is_open());
		size_t size = vec.size();
		// Read in the size
		in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // If the size is 0, don't read anything
		vec.resize(size); // Resize the vec to be read into
		in.read(reinterpret_cast<char*>(&vec[0]), size * sizeof(T));
	}

	void SerializeString(std::ofstream& out, std::string& str)
	{
		// Make sure the file was opened
		assert(out.is_open());
		size_t size = str.size();
		// Write the size
		out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // Don't write if the size is 0
		out.write(reinterpret_cast<char*>(&str[0]), size * sizeof(char));
	}

	void DeserializeString(std::ifstream& in, std::string& str)
	{
		// Make sure the file was opened
		assert(in.is_open());
		size_t size = str.size();
		// Read in the size
		in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // Don't read if the size is 0
		str.resize(size); // Resize string to be read into
		in.read(reinterpret_cast<char*>(&str[0]), size * sizeof(char));
	}

	void SerializeMaterial_t(std::ofstream& out, tinyobj::material_t& material)
	{
		SerializeString(out, material.name);
		// 18 floats
		out.write(reinterpret_cast<char*>(&material.ambient[0]), 18 * sizeof(float));
		// 2 ints
		out.write(reinterpret_cast<char*>(&material.illum), 2 * sizeof(int));
		// Strings
		SerializeString(out, material.ambient_texname);
		SerializeString(out, material.diffuse_texname);
		SerializeString(out, material.specular_texname);
		SerializeString(out, material.specular_highlight_texname);
		SerializeString(out, material.bump_texname);
		SerializeString(out, material.displacement_texname);
		SerializeString(out, material.alpha_texname);
	}

	void DeserializeMaterial_t(std::ifstream& in, tinyobj::material_t& material)
	{
		DeserializeString(in, material.name);
		// 18 floats
		in.read(reinterpret_cast<char*>(&material.ambient[0]), 18 * sizeof(float));
		// 2 ints
		in.read(reinterpret_cast<char*>(&material.illum), 2 * sizeof(int));
		// Strings
		DeserializeString(in, material.ambient_texname);
		DeserializeString(in, material.diffuse_texname);
		DeserializeString(in, material.specular_texname);
		DeserializeString(in, material.specular_highlight_texname);
		DeserializeString(in, material.bump_texname);
		DeserializeString(in, material.displacement_texname);
		DeserializeString(in, material.alpha_texname);

	}

	void SerializeTag_t(std::ofstream& out, tinyobj::tag_t& tag)
	{
		SerializeString(out, tag.name);
		SerializeVector(out, tag.intValues);
		SerializeVector(out, tag.floatValues);
		SerializeVector(out, tag.stringValues);
	}

	void DeserializeTag_t(std::ifstream& in, tinyobj::tag_t& tag)
	{
		DeserializeString(in, tag.name);
		DeserializeVector(in, tag.intValues);
		DeserializeVector(in, tag.floatValues);
		DeserializeVector(in, tag.stringValues);
	}

	void SerializeMesh_t(std::ofstream& out, tinyobj::mesh_t& mesh)
	{
		SerializeVector(out, mesh.positions);
		SerializeVector(out, mesh.normals);
		SerializeVector(out, mesh.texcoords);
		SerializeVector(out, mesh.indices);
		SerializeVector(out, mesh.num_vertices);
		SerializeVector(out, mesh.material_ids);
		size_t size = mesh.tags.size();
		out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // If the size is 0, don't write anything
		for (auto& t : mesh.tags)
			SerializeTag_t(out, t);
		
	}

	void DeserializeMesh_t(std::ifstream& in, tinyobj::mesh_t& mesh)
	{
		DeserializeVector(in, mesh.positions);
		DeserializeVector(in, mesh.normals);
		DeserializeVector(in, mesh.texcoords);
		DeserializeVector(in, mesh.indices);
		DeserializeVector(in, mesh.num_vertices);
		DeserializeVector(in, mesh.material_ids);
		size_t size;
		// Read in the size of the tags
		in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		if (size == 0) return; // If the size is 0, don't read in anything
		mesh.tags.resize(size);
		for (size_t i = 0; i < size; i++)
			DeserializeTag_t(in, mesh.tags[i]);
	}

	void SerializeShape_t(std::ofstream& out, tinyobj::shape_t& shape)
	{
		SerializeString(out, shape.name);
		SerializeMesh_t(out, shape.mesh);
	}

	void DeserializeShape_t(std::ifstream& in, tinyobj::shape_t& shape)
	{
		DeserializeString(in, shape.name);
		DeserializeMesh_t(in, shape.mesh);
	}

	struct SaveMesh
	{
		std::vector<tinyobj::shape_t> loadedChunks;
		std::vector<tinyobj::material_t> loadedMaterials;
		std::string folder;
	};
	
	void SerializeSaveMesh(std::ofstream& out, SaveMesh& shape)
	{
		size_t size = shape.loadedChunks.size();
		if (size == 0)
			return;
		out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		for (auto& c : shape.loadedChunks)
			SerializeShape_t(out, c);
		size = shape.loadedMaterials.size();
		if (size == 0)
			return;
		out.write(reinterpret_cast<char*>(&size), sizeof(size_t));
		for (auto& m : shape.loadedMaterials)
			SerializeMaterial_t(out, m);
		SerializeString(out, shape.folder);
	}

	void DeserializeSaveMesh(std::ifstream& in, SaveMesh& shape)
	{
		size_t size;
		in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		shape.loadedChunks.resize(size);
		for (size_t i = 0; i < size; i++)
			DeserializeShape_t(in, shape.loadedChunks[i]);
		in.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		shape.loadedMaterials.resize(size);
		for (size_t i = 0; i < size; i++)
			DeserializeMaterial_t(in, shape.loadedMaterials[i]);
		DeserializeString(in, shape.folder);
	}

	static bool LoadOBJ(Mesh& a_mesh, const char* a_path)
	{
		bool flipTextureV = true;

		if (a_mesh.Empty() == false)
		{
			printf("Mesh already initialised, can't re-initialise!\n");
			return false;
		}

		// Setup some return variables for the tiny obj loader
		std::vector<tinyobj::shape_t> loadedChunks;
		std::vector<tinyobj::material_t> loadedMaterials;
		std::string error = "";
		std::string file = a_path;
		std::string folder = file.substr(0, file.find_last_of('/') + 1);

		//bool hmm = std::is_trivially_copyable<std::vector<int>>();

		bool serializeMesh = true;
		if (serializeMesh)
		{
			SaveMesh saveMesh;

			std::ifstream in;
			std::string saveFile = std::string("./temp/");
			for (size_t i = file.find_last_of('/') + 1; i < file.find_last_of('.'); i++)
				saveFile.push_back(file[i]);
			saveFile.append(".save");
			in.open(saveFile, std::ios::binary);

			// If the file opened sucessfully, load it in
			if (in.is_open())
			{
				DeserializeSaveMesh(in, saveMesh);
				in.close();

				// Get the values out if saveMesh
				loadedChunks.swap(saveMesh.loadedChunks);
				loadedMaterials.swap(saveMesh.loadedMaterials);
				folder.swap(saveMesh.folder);
			}
			else
			{
				in.close();

				// Load the obj and make sure it worked
				if (tinyobj::LoadObj(loadedChunks, loadedMaterials, error, a_path, folder.c_str()) == false)
				{
					printf("%s\n", error.c_str());
					return false;
				}

				// Save the obj out to a binary file
				if (serializeMesh)
				{
					saveMesh.loadedChunks = loadedChunks;
					saveMesh.loadedMaterials = loadedMaterials;
					saveMesh.folder = folder;

					std::ofstream out;
					out.open(saveFile, std::ios::binary);
					assert(out.is_open());
					SerializeSaveMesh(out, saveMesh);
					out.close();
				}
			}
		}
		else
		{
			// Load the obj and make sure it worked
			if (tinyobj::LoadObj(loadedChunks, loadedMaterials, error, a_path, folder.c_str()) == false)
			{
				printf("%s\n", error.c_str());
				return false;
			}
		}


		// Copy materials
		std::vector<Material> materials;
		materials.resize(loadedMaterials.size());
		int index = 0;
		for (auto& m : loadedMaterials)
		{
			materials[index].name = m.name;

			materials[index].ambient = glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]);
			materials[index].diffuse = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
			materials[index].specular = glm::vec3(m.specular[0], m.specular[1], m.specular[2]);
			materials[index].transmittance = glm::vec3(m.transmittance[0], m.transmittance[1], m.transmittance[2]);
			materials[index].emission = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);
			materials[index].shininess = m.shininess;
			materials[index].ior = m.ior;
			materials[index].dissolve = m.dissolve;
			materials[index].illum = m.illum;

			// Load in all the textures
			assert(m.alpha_texname.size() == 0				|| Texture::Load((folder + m.alpha_texname).c_str(),				materials[index].alphaTexture));
			assert(m.ambient_texname.size() == 0			|| Texture::Load((folder + m.ambient_texname).c_str(),				materials[index].ambientTexture));
			assert(m.diffuse_texname.size() == 0			|| Texture::Load((folder + m.diffuse_texname).c_str(),				materials[index].diffuseTexture));
			assert(m.specular_texname.size() == 0			|| Texture::Load((folder + m.specular_texname).c_str(),				materials[index].specularTexture));
			assert(m.specular_highlight_texname.size() == 0 || Texture::Load((folder + m.specular_highlight_texname).c_str(),	materials[index].specularHighlightTexture));
			assert(m.bump_texname.size() == 0				|| Texture::Load((folder + m.bump_texname).c_str(),					materials[index].normalTexture));
			assert(m.displacement_texname.size() == 0		|| Texture::Load((folder + m.displacement_texname).c_str(),			materials[index].displacementTexture));

			++index;
		}

		// Copy mesh chunks
		std::vector<Mesh::Chunk> meshChunks;
		meshChunks.reserve(loadedChunks.size());
		for (auto& loadedChunk : loadedChunks) {

			Mesh::Chunk chunk;

			// generate buffers
			glGenBuffers(1, &chunk.vbo);
			glGenBuffers(1, &chunk.ibo);
			glGenVertexArrays(1, &chunk.vao);

			// bind vertex array aka a mesh wrapper
			glBindVertexArray(chunk.vao);

			// set the index buffer data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				loadedChunk.mesh.indices.size() * sizeof(unsigned int),
				loadedChunk.mesh.indices.data(), GL_STATIC_DRAW);

			// store index count for rendering
			chunk.indexCount = (unsigned int)loadedChunk.mesh.indices.size();

			// create vertex data
			std::vector<Mesh::Vertex> vertices;
			vertices.resize(loadedChunk.mesh.positions.size() / 3);
			size_t vertCount = vertices.size();

			bool hasPosition = loadedChunk.mesh.positions.empty() == false;
			bool hasNormal = loadedChunk.mesh.normals.empty() == false;
			bool hasTexture = loadedChunk.mesh.texcoords.empty() == false;

			for (size_t i = 0; i < vertCount; ++i)
			{
				if (hasPosition)
					vertices[i].position = glm::vec4(loadedChunk.mesh.positions[i * 3 + 0], loadedChunk.mesh.positions[i * 3 + 1], loadedChunk.mesh.positions[i * 3 + 2], 1);
				if (hasNormal)
					vertices[i].normal = glm::vec4(loadedChunk.mesh.normals[i * 3 + 0], loadedChunk.mesh.normals[i * 3 + 1], loadedChunk.mesh.normals[i * 3 + 2], 0);

				// flip the T / V (might not always be needed, depends on how mesh was made)
				if (hasTexture)
					vertices[i].texCoord = glm::vec2(loadedChunk.mesh.texcoords[i * 2 + 0], flipTextureV ? 1.0f - loadedChunk.mesh.texcoords[i * 2 + 1] : loadedChunk.mesh.texcoords[i * 2 + 1]);
			}

			// calculate for normal mapping
			if (hasNormal && hasTexture)
				CalculateTangents(vertices, loadedChunk.mesh.indices);

			// bind vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

			// fill vertex buffer
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Mesh::Vertex), vertices.data(), GL_STATIC_DRAW);

			// enable first element as positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), 0);

			// enable normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Mesh::Vertex), (void*)(sizeof(glm::vec4) * 1));

			// enable texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(sizeof(glm::vec4) * 2));

			// enable tangents
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(sizeof(glm::vec4) * 2 + sizeof(glm::vec2)));

			// bind 0 for safety
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// set chunk material
			chunk.materialID = loadedChunk.mesh.material_ids.empty() ? -1 : loadedChunk.mesh.material_ids[0];

			//meshChunks.push_back(chunk);
			meshChunks.emplace_back(std::move(chunk));
		}

		// Apply the loaded and copied mesh to the mesh
		a_mesh.Initialise(meshChunks, materials);

		// Successful
		return true;
	}

};