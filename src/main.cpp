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
GLuint FBOTexture;
GLuint FBODepthBuffer;
GLuint frameBufferObject;
GLuint fullScreenVAO;
GLuint fullScreenVBO;
GLuint fullScreenShaderProgram;

#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0
#define SHADOW_MAP_RATIO 2
GLuint FBO = 0;
GLuint depthTexture;
GLuint shadowMapProgram;
mat4 lightSpaceMatrix;
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
vec3 cameraPos = vec3(32, 20, 0);
vec3 cameraLookAt = vec3(2, 0, -10);
vec3 lightPos = vec3(3, 20, 0);
vec3 lightLookAt = vec3(0, 0, -5);
float lightMovementRadius = 30.0f;
GLhandleARB shadowShaderId;
GLuint shadowMapUniform;

const int FRAME_BUFFER_WIDTH = 640;
const int FRAME_BUFFER_HEIGHT = 480;

MeshData currentMesh;

Uint32 oldTime;
Uint32 currentTime;
float ftime;
float totalTime = 0;

//Shader Variables
mat4 MVPMatrix;
mat4 modelMatrix;
vec3 lightDirection = vec3(0.0f, 0.0f, 1.0f);
vec4 ambientMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
vec4 ambientLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 diffuseMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
vec4 diffuseLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec3 cameraPosition = vec3(15.0f, 50.0f, 50.0f);
vec4 specularMaterialColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
GLfloat specularPower = 100;
vec4 specularLightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

//Quad Coords
float vertices[] =
{
	-1, -1,
	1, -1,
	-1, 1,
	1, 1,
};
void CreateFrameBuffer()
{
	//Create Texture Object
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &FBOTexture);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);

	//GLenum target = GL_COLOR_ATTACHMENT0;
	//glDrawColors(1, &target);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	//Create Depth Buffer
	glGenRenderbuffers(1, &FBODepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, FBODepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, FRAME_BUFFER_WIDTH,
		FRAME_BUFFER_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//Create Framebuffer
	glGenFramebuffers(1, &frameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		FBOTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
		FBODepthBuffer);

	//Check Status of the buffer
	GLenum status;
	if ((status = glCheckFramebufferStatus(
		GL_FRAMEBUFFER)) !=
		GL_FRAMEBUFFER_COMPLETE)	{
		cout << "Issue	with	Framebuffers" << endl;
	}
}

void CreateFrameBufferDepth()
{
	//Create framebuffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, FBO);

	//Depth Texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	//We dont want to draw any color
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthTexture, 0);


	//Check Status of the buffer
	GLenum status;
	if ((status = glCheckFramebufferStatus(
		GL_FRAMEBUFFER)) !=
		GL_FRAMEBUFFER_COMPLETE)	{
		cout << "Issue	with	Framebuffers" << endl;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void MVPLightPoV()
{
	GLfloat nearPlane = 1.0f;
	GLfloat farPlane = 7.5f;

	mat4 lightProjection = ortho<float>(-10, 10, -10, 10, nearPlane, farPlane);
	mat4 lightView = lookAt(vec3(-2.0f, 4.0f, -1.0f), vec3(0, 0, 0), vec3(1.0f));
	lightSpaceMatrix = lightProjection * lightView;

}

void setTextureMatrix(void)
{
	static double modelView[16];
	static double projection[16];

	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0 };

	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);


	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);

	glLoadIdentity();
	glLoadMatrixd(bias);

	// concatating all matrices into one.
	glMultMatrixd(projection);
	glMultMatrixd(modelView);

	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void setupMatrices(float position_x, float position_y, float position_z, float lookAt_x, float lookAt_y, float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, RENDER_WIDTH / RENDER_HEIGHT, 10, 40000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x, position_y, position_z, lookAt_x, lookAt_y, lookAt_z, 0, 1, 0);
}

void Update(void)
{
	lightPos.x = lightMovementRadius * cos(ftime);
	lightPos.z = lightMovementRadius * sin(ftime);
}

void startTranslate(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);

	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	glPushMatrix();
	glTranslatef(x, y, z);
}

void endTranslate()
{
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawObjects(void)
{
	// Ground
	glColor4f(0.3f, 0.3f, 0.3f, 1);
	glBegin(GL_QUADS);
	glVertex3f(-35, 2, -35);
	glVertex3f(-35, 2, 15);
	glVertex3f(15, 2, 15);
	glVertex3f(15, 2, -35);
	glEnd();

	glColor4f(0.9f, 0.9f, 0.9f, 1);
}

void renderScene(void)
{
	Update();

	//First step: Render from the light POV to a FBO, story depth values only
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);	//Rendering offscreen

	//Using the fixed pipeline to render to the depthbuffer
	glUseProgramObjectARB(0);

	// In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
	glViewport(0, 0, RENDER_WIDTH * SHADOW_MAP_RATIO, RENDER_HEIGHT* SHADOW_MAP_RATIO);

	// Clear previous frame values
	glClear(GL_DEPTH_BUFFER_BIT);

	//Disable color rendering, we only want to write to the Z-Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	setupMatrices(lightPos.x, lightPos.y, lightPos.z, lightLookAt.x, lightLookAt.y, lightLookAt.z);

	// Culling switching, rendering only backface, this is done to avoid self-shadowing
	glCullFace(GL_FRONT);
	drawObjects();

	//Save modelview/projection matrice into texture7, also add a biais
	setTextureMatrix();


	// Now rendering from the camera POV, using the FBO to generate shadows
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);

	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);
	glUniform1iARB(shadowMapUniform, 7);
	glActiveTextureARB(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthTexture);






	setupMatrices(cameraPos.x, cameraPos.y, cameraPos.z, cameraLookAt.x, cameraLookAt.y, cameraLookAt.z);

	glCullFace(GL_BACK);
	drawObjects();

	// DEBUG only. this piece of code draw the depth buffer onscreen
	/*
	glUseProgramObjectARB(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-RENDER_WIDTH/2,RENDER_WIDTH/2,-RENDER_HEIGHT/2,RENDER_HEIGHT/2,1,20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4f(1,1,1,1);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depthTextureId);
	glEnable(GL_TEXTURE_2D);
	glTranslated(0,0,-1);
	glBegin(GL_QUADS);
	glTexCoord2d(0,0);glVertex3f(0,0,0);
	glTexCoord2d(1,0);glVertex3f(RENDER_WIDTH/2,0,0);
	glTexCoord2d(1,1);glVertex3f(RENDER_WIDTH/2,RENDER_HEIGHT/2,0);
	glTexCoord2d(0,1);glVertex3f(0,RENDER_HEIGHT/2,0);


	glEnd();
	glDisable(GL_TEXTURE_2D);
	*/

}


//void RenderShadowMapScene()
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
//	glViewport(0, 0, 1024, 1024);
//
//	glUseProgram(shadowMapProgram);
//
//	GLuint depthMVPLocation = glGetUniformLocation(shadowMapProgram, "depthMVP");
//
//	
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, depthTexture);
//	glUniformMatrix4fv(depthMVPLocation, 1, GL_FALSE, &depthMVP[0][0]);
//
//}

void initScene()
{
	CreateFrameBuffer();
	//CreateFrameBufferDepth();
	//Generate and bind Vertex Array Object
	glGenVertexArrays(1, &fullScreenVAO);
	glBindVertexArray(fullScreenVAO);
	//Generate, beinf and fill Vertex Buffer Object
	glGenBuffers(1, &fullScreenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, fullScreenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Enable Vertex Attibute Array and define Attribute Pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, NULL);

	//Load and Compile simplePostProcessVS.glsl
	GLuint postProcessVertexShaderProgram = 0;
	string postProcessVSPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessVS.glsl";
	postProcessVertexShaderProgram = loadShaderFromFile(postProcessVSPath, VERTEX_SHADER);
	checkForCompilerErrors(postProcessVertexShaderProgram);

	//Load and Compile simplePostProcessFS.glsl
	GLuint postProcessFragmentShaderProgram = 0;
	string postProcessFSPath = ASSET_PATH + SHADER_PATH + "/simplePostProcessFS.glsl";
	postProcessFragmentShaderProgram = loadShaderFromFile(postProcessFSPath, FRAGMENT_SHADER);
	checkForCompilerErrors(postProcessFragmentShaderProgram);

	//Create and link fullScreenShaderProgram
	fullScreenShaderProgram = glCreateProgram();
	glAttachShader(fullScreenShaderProgram, postProcessVertexShaderProgram);
	glAttachShader(fullScreenShaderProgram, postProcessFragmentShaderProgram);
	glBindAttribLocation(0, fullScreenShaderProgram, "vertexPosition");
	glLinkProgram(fullScreenShaderProgram);
	checkForLinkErrors(fullScreenShaderProgram);

	glDeleteShader(postProcessVertexShaderProgram);
	glDeleteShader(postProcessFragmentShaderProgram);

	/****************************************
	ShadowMap Shader Begin
	*****************************************/

	//Load and Compile shadowMapVS.glsl
	GLuint shadowMapVertexShaderProgram = 0;
	string shadowMapVSPath = ASSET_PATH + SHADER_PATH + "/shadowMapVS.glsl";
	shadowMapVertexShaderProgram = loadShaderFromFile(shadowMapVSPath, VERTEX_SHADER);
	checkForCompilerErrors(shadowMapVertexShaderProgram);

	//Load and Compile shadowMapFS.glsl
	GLuint shadowMapFragmentShaderProgram = 0;
	string shadowMapFSPath = ASSET_PATH + SHADER_PATH + "/shadowMapFS.glsl";
	shadowMapFragmentShaderProgram = loadShaderFromFile(shadowMapFSPath, FRAGMENT_SHADER);
	checkForCompilerErrors(shadowMapFragmentShaderProgram);

	//Create and link shadowMapProgram
	shadowMapProgram = glCreateProgram();
	glAttachShader(shadowMapProgram, shadowMapVertexShaderProgram);
	glAttachShader(shadowMapProgram, shadowMapFragmentShaderProgram);
	glBindAttribLocation(0, shadowMapProgram, "depthMVP");
	glLinkProgram(shadowMapProgram);
	checkForLinkErrors(shadowMapProgram);

	glDeleteShader(shadowMapVertexShaderProgram);
	glDeleteShader(shadowMapFragmentShaderProgram);

	/****************************************
	ShadowMap Shader End
	*****************************************/

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void RenderScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

	//old imediate mode!
	//Set the clear colour(background)
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	//clear the colour and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glUseProgram(shaderProgram);
	glUseProgram(shadowMapProgram);

	GLuint shadowLocation = glGetUniformLocation(shadowMapProgram, "lightSpaceMatrix");
	glUniformMatrix4fv(shadowLocation, 1, GL_FALSE, value_ptr(lightSpaceMatrix));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	

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

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, currentMesh.getNumIndices(), GL_UNSIGNED_INT, 0);

}

void RenderPostProcessing()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(fullScreenShaderProgram);

	GLuint texture0location = glGetUniformLocation(fullScreenShaderProgram, "texture0");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOTexture);
	glUniform1i(texture0location, 0);

	glBindVertexArray(fullScreenVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


}

void CleanUpFrameBuffer()
{
	glDeleteTextures(1, &FBOTexture);
	glDeleteBuffers(1, &FBODepthBuffer);
	glDeleteBuffers(1, &frameBufferObject);
	glDeleteBuffers(1, &fullScreenVAO);
	glDeleteBuffers(1, &fullScreenVBO);
	glDeleteProgram(fullScreenShaderProgram);
}

void cleanUp()
{
  CleanUpFrameBuffer();
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

 // modelMatrix = modelMatrix * mat4(0.1f);
  MVPLightPoV();
}

void render()
{
	//Render to Depth texture
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		MVPLightPoV();
		RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Render scene has normal with shadow mapping
	glViewport(0, 0, 640, 480);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MVPLightPoV();
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	RenderScene();
	RenderPostProcessing();
}

void LoadShadowShader()
{
	//Load and Compile shadowMapVS.glsl
	GLuint shadowMapVertexShaderProgram = 0;
	string shadowMapVSPath = ASSET_PATH + SHADER_PATH + "/shadowMapVS.glsl";
	shadowMapVertexShaderProgram = loadShaderFromFile(shadowMapVSPath, VERTEX_SHADER);
	checkForCompilerErrors(shadowMapVertexShaderProgram);

	//Load and Compile shadowMapFS.glsl
	GLuint shadowMapFragmentShaderProgram = 0;
	string shadowMapFSPath = ASSET_PATH + SHADER_PATH + "/shadowMapFS.glsl";
	shadowMapFragmentShaderProgram = loadShaderFromFile(shadowMapFSPath, FRAGMENT_SHADER);
	checkForCompilerErrors(shadowMapFragmentShaderProgram);

	//Create and link shadowMapProgram
	shadowShaderId = glCreateProgram();
	glAttachShader(shadowShaderId, shadowMapVertexShaderProgram);
	glAttachShader(shadowShaderId, shadowMapFragmentShaderProgram);
	glBindAttribLocation(0, shadowShaderId, "ShadowMap");
	glLinkProgram(shadowShaderId);
	checkForLinkErrors(shadowShaderId);

	shadowMapUniform = glGetUniformLocation(shadowShaderId, "ShadowMap");

	glDeleteShader(shadowMapVertexShaderProgram);
	glDeleteShader(shadowMapFragmentShaderProgram);
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

	currentTime = SDL_GetTicks();


    //Game Loop
    while(run)
    {
		oldTime = currentTime;
		currentTime = SDL_GetTicks();
		ftime = (currentTime - oldTime) / 1000.0f;
		totalTime += ftime;
        //While we still have events in the queue

		CreateFrameBufferDepth();
		LoadShadowShader();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 1.0f);

		glEnable(GL_CULL_FACE);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


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
		//if (err != GL_NO_ERROR)
		//	printf("OGL error: %s \n", gluErrorString(err));
        //init Scene
        //update();
		renderScene();
		//err = glGetError();
		//if (err != GL_NO_ERROR)
		//	printf("OGL error: %s \n", gluErrorString(err));
  //      //render
		//render();
		//err = glGetError();
		//if (err != GL_NO_ERROR)
		//	printf("OGL error: %s \n", gluErrorString(err));
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
