#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "Common.h"

GLuint LoadTextureFromFile(const string &filename);

GLuint loadTextureFromFont(const string &fontFilename, int pointSize, const string &text);

GLuint ConvertSDLSurfaceToTexture(SDL_Surface * surface);

#endif