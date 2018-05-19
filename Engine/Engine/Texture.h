#pragma once
#include <string>

class Texture
{
public:

	enum Format : unsigned int 
	{
		RED = 1,
		RG,
		RGB,
		RGBA
	};

	Texture() : width(0), height(0), glHandle(0), format(0), loadedPixels(nullptr) {};
	virtual ~Texture() = default;

	// Loads a jpg, bmp, png or tga. Returns false if failed
	static bool Load(const char* filename, Texture& texture);

	// Creates a texture
	static Texture Create(const unsigned int& a_width, const unsigned int a_height, 
		const Format a_format, const unsigned char* pixels);

	// Binds the texture to the slot specificed
	void Bind(const unsigned int& slot) const; 

	// Returns the opengl texture handle
	unsigned int GetHandle() const { return glHandle; }

private:
	std::string name;
	unsigned int width;
	unsigned int height;
	unsigned int glHandle;
	unsigned int format;
	unsigned char* loadedPixels;
};

