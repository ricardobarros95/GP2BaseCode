#include "Common.h"
#include "Graphics.h"
#include "Vertices.h"
#include "Shader.h"
#include "Texture.h"
#include "FileSystem.h"
#include "FBXLoader.h"
#include "Mesh.h"


//matrices
mat4 viewMatrix;
mat4 projMatrix;
mat4 worldMatrix;


GLuint VBO;
GLuint EBO;
GLuint VAO;
GLuint shaderProgram;
GLuint textureMap;
GLuint fontTexture;

MeshData currentMesh;



//Shader Variables
mat4 MVPMatrix;
mat4 modelMatrix;
vec3 lightDirection = vec3(0.0f, 0.0f, 1.0f);
vec4 ambientMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
vec4 ambientLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
vec4 diffuseLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec3 cameraPosition = vec3(5.0f, 35.0f, 50.0f);
vec4 specularMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
GLfloat specularPower = 100;
vec4 specularLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

void initScene()
{
	string modelPath = ASSET_PATH + MODEL_PATH + "/Utah-Teapot.fbx";
	loadFBXFromFile(modelPath, &currentMesh);

  //load texture and bind
	string texturePath = ASSET_PATH + TEXTURE_PATH + "/Tank1DF.png";
	textureMap = LoadTextureFromFile(texturePath);

	glBindTexture(GL_TEXTURE_2D, textureMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);


	string fontPath = ASSET_PATH + FONT_PATH + "/OratorStd.otf";
	fontTexture = loadTextureFromFont(fontPath, 18, "Hello World");

	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


  //Generate Vertex Array
  glGenVertexArrays(1,&VAO);
  glBindVertexArray( VAO );
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, currentMesh.getNumVerts()*sizeof(Vertex), &currentMesh.vertices[0], GL_STATIC_DRAW);

  //create buffer
  glGenBuffers(1, &EBO);
  //Make the EBO active
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  //Copy Index data to the EBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh.getNumIndices()*sizeof(int), &currentMesh.indices[0], GL_STATIC_DRAW);
  //Tell the shader that 0 is the position element
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)(sizeof(vec3)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)(sizeof(vec3)+sizeof(vec4)));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void**)(sizeof(vec3) + sizeof(vec4) + sizeof(vec2)));

  GLuint vertexShaderProgram=0;
  string vsPath = ASSET_PATH + SHADER_PATH + "/specularVS.glsl";
  vertexShaderProgram = loadShaderFromFile(vsPath, VERTEX_SHADER);
  checkForCompilerErrors(vertexShaderProgram);

  GLuint fragmentShaderProgram=0;
  string fsPath = ASSET_PATH + SHADER_PATH + "/specularFS.glsl";
  fragmentShaderProgram = loadShaderFromFile(fsPath, FRAGMENT_SHADER);
  checkForCompilerErrors(fragmentShaderProgram);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShaderProgram);
  glAttachShader(shaderProgram, fragmentShaderProgram);

  //Link attributes
  glBindAttribLocation(shaderProgram, 0, "vertexPosition");
  glBindAttribLocation(shaderProgram, 1, "vertexColour");
  glBindAttribLocation(shaderProgram, 2, "vertexTexCoords");
  glBindAttribLocation(shaderProgram, 3, "vertexNormal");

  glLinkProgram(shaderProgram);
  checkForLinkErrors(shaderProgram);
  //now we can delete the VS & FS Programs
  glDeleteShader(vertexShaderProgram);
  glDeleteShader(fragmentShaderProgram);
}

void cleanUp()
{
  glDeleteTextures(1, &textureMap);
  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1,&VAO);
  glDeleteTextures(1, &fontTexture);
}

void update()
{
  projMatrix = glm::perspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

  viewMatrix = glm::lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

  worldMatrix= glm::translate(mat4(1.0f), vec3(0.0f,0.0f,0.0f));

  MVPMatrix=projMatrix*viewMatrix*worldMatrix;
}

void render()
{
    //old imediate mode!
    //Set the clear colour(background)
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    //clear the colour and depth buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);

    GLuint MVPLocation = glGetUniformLocation(shaderProgram, "MVP");

	GLuint texture0Location = glGetUniformLocation(shaderProgram, "texture0");

	GLuint ambMatColorLocation = glGetUniformLocation(shaderProgram, "ambientMaterialColour");

	GLuint ambLightColorLocation = glGetUniformLocation(shaderProgram, "ambientLightColour");

	GLuint modelLocation = glGetUniformLocation(shaderProgram, "Model");

	GLuint lightDirectionLocation = glGetUniformLocation(shaderProgram, "lightDirection");

	GLuint diffMatColorLocation = glGetUniformLocation(shaderProgram, "diffuseMaterialColour");

	GLuint diffLightColorLocation = glGetUniformLocation(shaderProgram, "diffuseLightColour");

	GLuint specMatColorLocation = glGetUniformLocation(shaderProgram, "specularMaterialColour");

	GLuint specPowerLocation = glGetUniformLocation(shaderProgram, "specularPower");

	GLuint specLightColorLocation = glGetUniformLocation(shaderProgram, "specularLightColour");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMap);

    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
	glUniform1i(texture0Location, 0);
	glUniform4fv(ambMatColorLocation, 1, glm::value_ptr(ambientMaterialColor));
	glUniform4fv(ambLightColorLocation, 1, glm::value_ptr(ambientLightColor));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform3fv(lightDirectionLocation, 1, glm::value_ptr(lightDirection));
	glUniform4fv(diffLightColorLocation, 1, glm::value_ptr(diffuseLightColor));
	glUniform4fv(diffMatColorLocation, 1, glm::value_ptr(diffuseMaterialColor));
	glUniform4fv(specMatColorLocation, 1, glm::value_ptr(specularMaterialColor));
	glUniform1f(specPowerLocation, specularPower);
	glUniform4fv(specLightColorLocation, 1, glm::value_ptr(specularLightColor));

    glBindVertexArray( VAO );

	glDrawElements(GL_TRIANGLES, currentMesh.getNumIndices(), GL_UNSIGNED_INT, 0);
}

int main(int argc, char * arg[])
{
	ChangeWorkingDirectory();
    //Controls the game loop
    bool run=true;

    // init everyting - SDL, if it is nonzero we have a problem
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "ERROR SDL_Init " <<SDL_GetError()<< std::endl;

        return -1;
	}

	//Request opengl 4.1 context, Core Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	int imageInitFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	int returnInitFlags = IMG_Init(imageInitFlags);
	if (((returnInitFlags)& (imageInitFlags)) != imageInitFlags)
	{
		cout << "ERROR SDL_Image Init" << IMG_GetError() << endl;
	}

	if (TTF_Init() == -1)
	{
		std::cout << "ERROR TTF_Init: " << TTF_GetError();
	}

    //Create a window
    SDL_Window * window = SDL_CreateWindow(
                                           "SDL",             // window title
                                           SDL_WINDOWPOS_CENTERED,     // x position, centered
                                           SDL_WINDOWPOS_CENTERED,     // y position, centered
                                           640,                        // width, in pixels
                                           480,                        // height, in pixels
                                           SDL_WINDOW_OPENGL           // flags
                                           );

    // Create an OpenGL context associated with the window.
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    //Call our InitOpenGL Function
    initOpenGL();
    //Set our viewport
    setViewport(640,480);

    initScene();
	GLenum err = glGetError();
    //Value to hold the event generated by SDL
    SDL_Event event;
    //Game Loop
    while(run)
    {
        //While we still have events in the queue
        while (SDL_PollEvent(&event)) {
            //Get event type
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                //set our boolean which controls the loop to false
                run = false;
            }
            if (event.type==SDL_KEYDOWN){
                  switch( event.key.keysym.sym )
                  {
                    case SDLK_LEFT:
                      break;
                      case SDLK_RIGHT:
                      break;
                      case SDLK_UP:
                      break;
                      case SDLK_DOWN:
                      break;
                    default:
                      break;
                  }
            }
        }
		err = glGetError();
		if (err != GL_NO_ERROR)
			printf("OGL error: %s \n", gluErrorString(err));
        //init Scene
        update();
		err = glGetError();
		if (err != GL_NO_ERROR)
			printf("OGL error: %s \n", gluErrorString(err));
        //render
        render();
		err = glGetError();
		if (err != GL_NO_ERROR)
			printf("OGL error: %s \n", gluErrorString(err));
        //Call swap so that our GL back buffer is displayed
        SDL_GL_SwapWindow(window);

    }

    // clean up, reverse order!!!
    cleanUp();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
	IMG_Quit();
	TTF_Quit();
    SDL_Quit();

    return 0;
}
