#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <string>

#include <sstream>
#include <fstream>

using namespace std;

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

const string ASSET_PATH = "assets";
const string SHADER_PATH = "/shaders";

#ifdef _APPLE_
#include <OpenGL/glu.h>
#elif (_WIN64 || WIN32)
#include <gl\GLU.h>
#endif


#endif
