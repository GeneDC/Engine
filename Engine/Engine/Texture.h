#pragma once
#include <string>
#include <gl_core_4_4.h>

class Texture
{
public:

	enum Format : unsigned int 
	{
		RED		= GL_RED,		// Texture storage with 1 color channel  of  8-bit floating point per channel
		RG		= GL_RG,		// Texture storage with 2 color channels of  8-bit floating point per channel
		RGB		= GL_RGB,		// Texture storage with 3 color channels of  8-bit floating point per channel
		RGBA	= GL_RGBA,		// Texture storage with 4 color channels of  8-bit floating point per channel
		R16F	= GL_R16F,		// Texture storage with 1 color channel  of 16-bit floating point per channel
		RGB16F	= GL_RGB16F,	// Texture storage with 3 color channels of 16-bit floating point per channel
		RGBA16F = GL_RGBA16F,	// Texture storage with 4 color channels of 16-bit floating point per channel
		RGB32F	= GL_RGB32F,	// Texture storage with 3 color channels of 32-bit floating point per channel
		RGBA32F = GL_RGBA32F	// Texture storage with 4 color channels of 32-bit floating point per channel
		// Refer to this link for more info: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	};

	Texture() : width(0), height(0), glHandle(0), format(0), loadedPixels(nullptr) {};
	virtual ~Texture() = default;

	// Loads a jpg, bmp, png or tga. Returns false if failed
	static bool Load(const char* filename, Texture& texture);

	// Creates a texture
	static Texture Create(const unsigned int& a_width, const unsigned int a_height, 
		const Format a_format, const unsigned char* pixels = nullptr);

	// Binds the texture to the slot specificed
	void Bind(const unsigned int& slot) const; 

	// Returns the opengl texture handle
	unsigned int GetHandle() const { return glHandle; }

	std::string name;

private:
	unsigned int width;
	unsigned int height;
	unsigned int glHandle;
	unsigned int format;
	unsigned char* loadedPixels;
};

