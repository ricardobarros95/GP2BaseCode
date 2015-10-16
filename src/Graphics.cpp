#include "Graphics.h"
#include "Common.h"

void initOpenGL()
{
	//Smooth Shading
	glShadeModel(GL_SMOOTH);

	//Clear the background to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Clear the depth buffer to 1.0
	glClearDepth(1.0f);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);

	//The depth test to use
	glDepthFunc(GL_LEQUAL);

	//Turn on best perspective correction
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		//glewInit failed
		std::cout << "Error" << glewGetErrorString(err) << std::endl;
	}
}

void setViewPort(int width, int height)
{
	//Setup Viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}