#include "Mesh.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>

Mesh::~Mesh()
{
	for (auto& chunk : meshChunks)
	{
		// Delete the arrays and buffers
		glDeleteVertexArrays(1, &chunk.vao);
		glDeleteBuffers(1, &chunk.vbo);
		glDeleteBuffers(1, &chunk.ibo);
	}
}

void Mesh::Initialise(const unsigned int & vertexCount, const Vertex * verts, const unsigned int & indexCount, const unsigned int * indices)
{
	// Insure that this mesh hasn't been initialized yet
	assert(meshChunks.size() == 0);

	Chunk newChunk;

	newChunk.materialID = 0;

	// Generate buffers
	glGenBuffers(1, &newChunk.vbo);
	glGenVertexArrays(1, &newChunk.vao);

	// Bind vertex array aka a mesh wrapper
	glBindVertexArray(newChunk.vao);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, newChunk.vbo);

	// Fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// Enable second element as normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 1));

	// Enable third element as texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 2));

	// Enable fourth element as tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 2 + sizeof(glm::vec2)));

	// Bind indices if there are any
	if (indexCount != 0)
	{
		glGenBuffers(1, &newChunk.ibo);
		// Bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newChunk.ibo);
		// Fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Vertex), indices, GL_STATIC_DRAW);

		newChunk.indexCount = indexCount / 3;
	}
	else
		newChunk.indexCount = vertexCount / 3;

	// Set chunk material
	newChunk.materialID = 0;

	// Put the new chunk in the list of chunks
	meshChunks.emplace_back(std::move(newChunk));

	// Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create a 2x2 RGB texture
	unsigned char texelData[16] =
	{
		255, 0, 255, 255,	255, 255, 255, 255,
		255, 255, 255, 255,	255, 0, 255, 255
	};
	Material newMat;
	newMat.name = "default";
	Texture tex = Texture::Create(2, 2, Texture::RGBA, texelData);
	tex.name = "default";
	newMat.diffuseTexture = tex;
	materials.emplace_back(std::move(newMat));

}

void Mesh::Initialise(std::vector<Chunk>& a_meshChunks, std::vector<Material>& a_materials, bool copy /*= false*/)
{
	if (copy)
	{
		meshChunks = a_meshChunks;
		materials = a_materials;
	}
	else
	{
		meshChunks.swap(a_meshChunks);
		a_meshChunks.clear();
		materials.swap(a_materials);
		a_materials.clear();

		if (materials.size() == 1 && materials[0].diffuseTexture.GetHandle() == 0)
		{
			// Create a 2x2 RGB texture
			unsigned char texelData[16] =
			{
				255, 0, 255, 255,	255, 255, 255, 255,
				255, 255, 255, 255,	255, 0, 255, 255
			};
			Texture tex = Texture::Create(2, 2, Texture::RGBA, texelData);
			materials[0].diffuseTexture = tex;
			materials[0].normalTexture = tex;
			materials[0].specularTexture = tex;
		}
	}
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

	// Set the normal values
	verts[0].normal = { 0, 1, 0, 0 };
	verts[1].normal = { 0, 1, 0, 0 };
	verts[2].normal = { 0, 1, 0, 0 };
	verts[3].normal = { 0, 1, 0, 0 };
	// Set the tangent values
	verts[0].tangent = { 1, 0, 1, 0 };
	verts[1].tangent = { 1, 0, 1, 0 };
	verts[2].tangent = { 1, 0, 1, 0 };
	verts[3].tangent = { 1, 0, 1, 0 };

	// Set the indices for the order of the verts to be drawn in
	unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };

	Initialise(4, verts, 6, indices);
}

void Mesh::Draw()
{
	bool usePatches = false;

	int program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (program == -1)
	{
		printf("No shader bound!\n");
		return;
	}

	// Pull uniforms from the shader
	int kaUniform = glGetUniformLocation(program, "Ka");
	int kdUniform = glGetUniformLocation(program, "Kd");
	int ksUniform = glGetUniformLocation(program, "Ks");
	int keUniform = glGetUniformLocation(program, "Ke");
	int opacityUniform = glGetUniformLocation(program, "opacity");
	int specPowUniform = glGetUniformLocation(program, "specularPower");

	int alphaTexUniform = glGetUniformLocation(program, "alphaTexture");
	int ambientTexUniform = glGetUniformLocation(program, "ambientTexture");
	int diffuseTexUniform = glGetUniformLocation(program, "diffuseTexture");
	int specTexUniform = glGetUniformLocation(program, "specularTexture");
	int specHighlightTexUniform = glGetUniformLocation(program, "specularHighlightTexture");
	int normalTexUniform = glGetUniformLocation(program, "normalTexture");
	int dispTexUniform = glGetUniformLocation(program, "displacementTexture");

	// Set texture slots (these don't change per material)
	if (diffuseTexUniform >= 0)
		glUniform1i(diffuseTexUniform, 0);
	if (alphaTexUniform >= 0)
		glUniform1i(alphaTexUniform, 1);
	if (ambientTexUniform >= 0)
		glUniform1i(ambientTexUniform, 2);
	if (specTexUniform >= 0)
		glUniform1i(specTexUniform, 3);
	if (specHighlightTexUniform >= 0)
		glUniform1i(specHighlightTexUniform, 4);
	if (normalTexUniform >= 0)
		glUniform1i(normalTexUniform, 5);
	if (dispTexUniform >= 0)
		glUniform1i(dispTexUniform, 6);

	int currentMaterial = -1;

	// Draw the mesh chunks
	for (auto& chunk : meshChunks) {

		// Bind material
		if (currentMaterial != chunk.materialID)
		{
			currentMaterial = chunk.materialID;
			if (kaUniform >= 0)
				glUniform3fv(kaUniform, 1, &materials[currentMaterial].ambient[0]);
			if (kdUniform >= 0)
				glUniform3fv(kdUniform, 1, &materials[currentMaterial].diffuse[0]);
			if (ksUniform >= 0)
				glUniform3fv(ksUniform, 1, &materials[currentMaterial].specular[0]);
			if (keUniform >= 0)
				glUniform3fv(keUniform, 1, &materials[currentMaterial].emission[0]);
			if (opacityUniform >= 0)
				glUniform1f(opacityUniform, materials[currentMaterial].dissolve);
			if (specPowUniform >= 0)
				glUniform1f(specPowUniform, materials[currentMaterial].shininess);

			glActiveTexture(GL_TEXTURE0);
			if (materials[currentMaterial].diffuseTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].diffuseTexture.GetHandle());
			else if (diffuseTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE1);
			if (materials[currentMaterial].alphaTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].alphaTexture.GetHandle());
			else if (alphaTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE2);
			if (materials[currentMaterial].ambientTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].ambientTexture.GetHandle());
			else if (ambientTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE3);
			if (materials[currentMaterial].specularTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].specularTexture.GetHandle());
			else if (specTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE4);
			if (materials[currentMaterial].specularHighlightTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].specularHighlightTexture.GetHandle());
			else if (specHighlightTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE5);
			if (materials[currentMaterial].normalTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].normalTexture.GetHandle());
			else if (normalTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE6);
			if (materials[currentMaterial].displacementTexture.GetHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, materials[currentMaterial].displacementTexture.GetHandle());
			else if (dispTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Bind and draw geometry
		glBindVertexArray(chunk.vao);
		auto drawType = usePatches ? GL_PATCHES : GL_TRIANGLES;
		glDrawElements(drawType, chunk.indexCount, GL_UNSIGNED_INT, 0);
	}
}
