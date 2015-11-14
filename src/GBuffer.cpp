#include "GBuffer.h"

bool GBuffer::Init(unsigned int windowWidth, unsigned int windowHeight)
{
	//Create FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

	//Create the g-buffer textures
	glGenTextures(4, textures);
	glGenTextures(1, &depthTexture);

	for (unsigned int i = 0; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB32F,
			windowWidth,
			windowHeight,
			0,
			GL_RGB,
			GL_FLOAT,
			NULL);
		glFramebufferTexture2D(
			GL_DRAW_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			GL_TEXTURE_2D,
			textures[i],
			0);
	}

	//depth
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT32F,
		windowWidth,
		windowHeight,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		depthTexture,
		0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(sizeof(DrawBuffers), DrawBuffers);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error, status 0x%x\n", Status);
		return false;
	}

	//restore default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}