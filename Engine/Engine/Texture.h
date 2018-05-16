#pragma once

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

	// load a jpg, bmp, png or tga
	static bool Load(const char* filename, Texture& texture);

	static Texture Create(const unsigned int& a_width, const unsigned int a_height, 
		const Format a_format, const unsigned char* pixels);
	
	void Bind(const unsigned int& slot) const; 

	/*
	// Returns the opengl texture handle
	unsigned int GetHandle() const { return glHandle; }
	*/

private:
	unsigned int width;
	unsigned int height;
	unsigned int glHandle;
	unsigned int format;
	unsigned char* loadedPixels;
};

