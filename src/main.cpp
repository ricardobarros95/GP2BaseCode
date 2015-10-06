#include "Common.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Shader.h"

GLfloat triangle1X = 0;
GLfloat triangle1Y = 0;
GLfloat triangle1Z = -6;
GLfloat triangle2X = 0;
GLfloat triangle2Y = 0;
GLfloat triangle2Z = -5;

GLfloat cubeRotationAngle = 0;
GLfloat cubeRotationX = 0;
GLfloat cubeRotationY = 0;
GLfloat cubeRotationZ = 0;

GLfloat cameraPosX = 0;
GLfloat cameraPosY = 0;
GLfloat cameraPosZ = 6.0f;

GLfloat lookAtX = 0;
GLfloat lookAtY = 0;

GLuint EBO;

GLuint shaderProgram = 0;

Vertex verts[]{
//Front		
{-0.5f, 0.5f, 0.5f, 
1.0, 0.0f, 1.0f, 1.0f},//Top Left

{ -0.5f, -0.5, 0.5f, 
1.0f, 1.0f, 0.0f, 1.0f }, //Bottom Left

{0.5f, -0.5f, 0.5f,
0.0f, 1.0f, 1.0f, 1.0f }, //Bottom Right

{0.5f,0.5f,0.5f,
1.0f,0.0f,1.0f,1.0f}, //Top Right

//Back
{-0.5f,0.5f,-0.5f,
1.0f,0.0f,1.0f,1.0f},//Top Left

{-0.5f,-0.5f,-0.5f,
1.0f,1.0f,0.0f,1.0f}, //Bottom Left

{0.5f, -0.5f, -0.5f,
0.0f, 1.0f, 1.0f, 1.0f}, //Bottom Right

{0.5f, 0.5f, -0.5f,
1.0f,0.0f,1.0f,1.0f}, //Top Right

};

GLuint indices[] = {
	//front
	0, 1, 2,
	0, 3, 2,

	//Left
	4, 5, 1,
	4, 1, 0,

	//Right
	3, 7, 2,
	7, 6, 2,

	//Bottom
	1, 5, 2,
	6, 2, 5,

	//Top
	4, 0, 7,
	0, 7, 3,

	//Back
	4, 5, 6,
	4, 7, 6
};

GLuint VBO;

void render()
{
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint),GL_UNSIGNED_INT,0);
}

void update()
{
	cubeRotationAngle += 3;
	cubeRotationX += 1;
	cubeRotationY += 1;
	cubeRotationZ += 1;
}

void InitScene()
{
	//Create buffer
	glGenBuffers(1, &VBO);
	//Make the new VBO active
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Copy text data to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	//Create buffer
	glGenBuffers(1, &EBO);
	//Make the new EBO active
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//Copy data to EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint vertexShaderProgram = 0;
	string vsPath = ASSET_PATH + SHADER_PATH + "/simpleVS.glsl";
	vertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);
	checkForCompilerErrors(vertexShaderProgram);

	GLuint fragmentShaderProgram = 0;
	string fsPath = ASSET_PATH + SHADER_PATH + "/simpleFS.glsl";
	fragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);
	checkForCompilerErrors(fragmentShaderProgram);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShaderProgram);
	glAttachShader(shaderProgram, fragmentShaderProgram);
	glLinkProgram(shaderProgram);
	checkForLinkErrors(shaderProgram);

	//now we can delete the VS and FD programs
	glDeleteShader(vertexShaderProgram);
	glDeleteShader(fragmentShaderProgram);
}

void CleanUp()
{
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
}

int main(int argc, char * arg[])
{
	bool run = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "ERROR SDL_Init" << SDL_GetError() << std::endl;
		return -1;
	}

	//Ask for OpenGL 4.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window * window = SDL_CreateWindow("SDL",  //Window title
							SDL_WINDOWPOS_CENTERED, // x position, centered
							SDL_WINDOWPOS_CENTERED, //y position, centered
							640,					//width in pixels
							480,					//height in pixels
							SDL_WINDOW_OPENGL		//flags
							);

	//Create an OpenGL context associated with the window
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	
	initOpenGL();
	InitScene();

	//SetViewPort
	setViewPort(640, 480);

	//Value to hold the event generated by SDL
	SDL_Event event;
	SDL_MouseMotionEvent mousePosEvent;
	//Game Loop
	while(run)
	{

		//While we still have events in the queue
		while (SDL_PollEvent(&event))
		{
			//Get event type
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			{
				run = false;
			}
			//if (event.type == SDL_MOUSEMOTION)
			//{
			//	lookAtX = event.motion.x;
			//	lookAtY = event.motion.y;
			//}
			//if (mousePosEvent.type == SDL_MOUSEMOTION)
			//{
			//	lookAtX = mousePosEvent.x;
			//	lookAtY = mousePosEvent.y;
			//}
			//cubeRotationAngle += 3;

			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT:
					triangle1X -= 1;
					break;
				case SDLK_RIGHT:
					triangle1X += 1;
					break;
				case SDLK_UP:
					triangle1Y += 1;
					break;
				case SDLK_DOWN:
					triangle1Y -= 1;
					break;
				case SDLK_w:
					cubeRotationAngle += 3;
					cubeRotationX += 1;
					break;
				case SDLK_s:
					cubeRotationAngle -= 3;
					cubeRotationX -= 1;
					break;
				case SDLK_d:
					cubeRotationAngle += 3;
					cubeRotationY += 1;
					break;
				case SDLK_a:
					cubeRotationAngle -= 3;
					cubeRotationY -= 1;
					break;
				case SDLK_i:
					cameraPosX -= 1;
					break;
				case SDLK_p:
					cameraPosX += 1;
					break;
				case SDLK_o:
					cameraPosY += 1;
					break;
				case SDLK_l:
					cameraPosY -= 1;
					break;
				}
				break;
			}
		}

		update();
		render();

		//Call swap so that our GL back buffer is displayed
		SDL_GL_SwapWindow(window); //what does this do?
	}
	CleanUp();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit(); //shouldnt this clear window as well?
    return 0;
}
