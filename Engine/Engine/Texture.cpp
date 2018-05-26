#include "Texture.h"

// Make sure you allways include gl core first
#include <gl_core_4_4.h>

#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning( disable : 4244)
#include <stb_image.h>
#pragma warning(pop)

bool Texture::Load(const char * filename, Texture & texture)
{
	// If the texture already has info, reset it
	if (texture.glHandle != 0)
	{
		glDeleteTextures(1, &texture.glHandle);
		texture.glHandle = 0;
		texture.width = 0;
		texture.height = 0;
	}

	int x = 0, y = 0, comp = 0;
	texture.loadedPixels = stbi_load(filename, &x, &y, &comp, STBI_default);

	// Check if the pixels loaded in correctly
	if (texture.loadedPixels == nullptr)
		return false;

	glGenTextures(1, &texture.glHandle);
	glBindTexture(GL_TEXTURE_2D, texture.glHandle);
	switch (comp)
	{
	case STBI_grey:
		texture.format = RED;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y,
			0, GL_RED, GL_UNSIGNED_BYTE, texture.loadedPixels);
		break;
	case STBI_grey_alpha:
		texture.format = RG;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, x, y,
			0, GL_RG, GL_UNSIGNED_BYTE, texture.loadedPixels);
		break;
	case STBI_rgb:
		texture.format = RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y,
			0, GL_RGB, GL_UNSIGNED_BYTE, texture.loadedPixels);
		break;
	case STBI_rgb_alpha:
		texture.format = RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y,
			0, GL_RGBA, GL_UNSIGNED_BYTE, texture.loadedPixels);
		break;
	default:
		break;
	};
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	texture.width = (unsigned int)x;
	texture.height = (unsigned int)y;

	texture.name = filename;
	texture.name = texture.name.substr(texture.name.find_last_of('/') + 1);


	return true;
}

Texture Texture::Create(const unsigned int & a_width, const unsigned int a_height,
	const Format a_format, const unsigned char * pixels)
{
	Texture texture;

	texture.width = a_width;
	texture.height = a_height;
	texture.format = a_format;
	texture.loadedPixels = new unsigned char[a_width * a_height];
	if (pixels != nullptr)
		for (size_t i = 0; i < a_width * a_height; i++)
			texture.loadedPixels[i] = pixels[i];


	glGenTextures(1, &texture.glHandle);
	glBindTexture(GL_TEXTURE_2D, texture.glHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	switch (texture.format)
	{
	case RED:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture.width, texture.height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		break;
	case RG:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, texture.width, texture.height, 0, GL_RG, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		break;
	case RGBA:
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		break;
	};

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void Texture::Bind(const unsigned int& slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, glHandle);
}
