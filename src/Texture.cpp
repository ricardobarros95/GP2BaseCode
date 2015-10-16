#include "Texture.h"

GLuint LoadTextureFromFile(const string &filename)
{
	SDL_Surface *imageSurface = IMG_Load(filename.c_str());
	if (!imageSurface)
	{
		cout << "Cant load image " << filename << " " << IMG_GetError();
		return 0;
	}

	GLuint textureID = ConvertSDLSurfaceToTexture(imageSurface);
	SDL_FreeSurface(imageSurface);

	return textureID;
}

GLuint ConvertSDLSurfaceToTexture(SDL_Surface *surface)
{
	GLuint textureID = 0;
	GLuint nofColors = surface->format->BytesPerPixel;

	GLenum textureFormat = GL_RGB;
	GLenum internalFormat = GL_RGB8;

	if (nofColors == 4) // Contains an alpha channel
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			textureFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
		}
		else
		{
			textureFormat = GL_BGRA;
			internalFormat = GL_RGBA8;
		}
	}
	else if (nofColors == 3)// no alpha channel 
	{
		if (surface->format->Rmask == 0x000000ff)
		{
			textureFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else
		{
			textureFormat = GL_BGR;
			internalFormat = GL_RGB8;
		}
	}
	else
	{
		cout << "Warning the image is not truecolor .. this will probably break";
		return 0;
	}

	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surface->w, surface->h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	return textureID;
}