#pragma once
#include "Texture.h"

class RenderTarget
{
public:
	RenderTarget() : initialised(false) {};
	~RenderTarget();

	// Must be called before this RenderTarget can be used
	// Sets up everything, returns false on failure
	bool Create(const unsigned int a_targetCount, const unsigned int a_width, const unsigned int a_height);

	void Bind();
	void Unbind();

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }

	unsigned int GetFrameBufferHandle() const { return fbo; }

	unsigned int GetTargetCount() const { return targetCount; }
	const Texture& GetTarget(unsigned int target) const { return targets[target]; }

private:
	bool initialised;

	unsigned int width;
	unsigned int height;

	unsigned int fbo;
	unsigned int rbo;

	unsigned int targetCount;
	Texture* targets;
};

