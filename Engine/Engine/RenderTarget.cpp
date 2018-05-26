#include "RenderTarget.h"
#include "gl_core_4_4.h"
#include <vector>
#include <assert.h>

RenderTarget::~RenderTarget()
{
	if (initialised)
	{
		delete[] targets;
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);
	}
}

bool RenderTarget::Create(const unsigned int a_targetCount, const unsigned int a_width, const unsigned int a_height)
{
	// Make sure that the values are not 0
	assert(!(a_targetCount == 0 || a_width == 0 || a_height == 0));

	// Setup our variables
	targetCount = a_targetCount;
	width = a_width;
	height = a_height;

	// Setup and bind a framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create and attach textures
	if (targetCount > 0)
	{
		targets = new Texture[targetCount];
		std::vector<GLenum> drawBuffers = {};

		for (size_t i = 0; i < targetCount; i++)
		{
			targets[i] = Texture::Create(width, height, Texture::Format::RGBA);

			drawBuffers.push_back((GLenum)(GL_COLOR_ATTACHMENT0 + i));

			glFramebufferTexture(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + i), targets[i].GetHandle(), 0);
		}

		glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
	}

	// Setup and bind a 24bit depth buffer as a render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check if successful
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// Cleanup
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		delete[] targets;
		targets = nullptr;
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &fbo);
		rbo = 0;
		fbo = 0;
		// Failed
		return false;
	}

	// Success
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	initialised = true;
	return true;
}

void RenderTarget::Bind()
{
	if (initialised)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
}

void RenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
