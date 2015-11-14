#ifndef _GBUFFER_
#define _GBUFFER_

#include "Common.h"

class GBuffer
{
public:
	enum GBUFFER_TEXTURE_TYPE
	{
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();
	bool Init(unsigned int windowWidth, unsigned int windowHeight);
	void BindForWritting();
	void BindForReading();

private:
	GLuint FBO;
	GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthTexture;
};

#endif