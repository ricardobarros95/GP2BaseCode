#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "Common.h"

GLuint  LoadTextureFromFile(const string &filename);

GLuint ConvertSDLSurfaceToTexture(SDL_Surface * surface);

#endif