#include "Common.h"
#include "Graphics.h"
#include "Vertices.h"

Vertex verts[]={
	//Front Face
    {-0.5f, -0.5f, 0.0f},
     {0.5f, -0.5f, 0.0f},
     {0.5f,  0.5f, 0.0f},

	 { -0.5f, -0.5f, 0.0f },
	 {-0.5f, 0.5f, 0.0f},
	 { 0.5f, 0.5f, 0.0f },

	 //Back Face
	 { -0.5f, -0.5f, -0.5f },
	 { 0.5f, -0.5f, -0.5f },
	 { 0.5f, 0.5f, -0.5f },

	 { -0.5f, -0.5f, -0.5f },
	 { -0.5f, 0.5f, -0.5f },
	 { 0.5f, 0.5f, -0.5f },

	 //Left Face

	 //Right Face

	 //top Face

	 //bottom Face
   };

GLuint vertexBuffer;

void initScene()
{
  vertexBuffer=createAndFillBuffer(verts,12);
}

void cleanUp()
{
  glDeleteBuffers(1,&vertexBuffer);
}

void render()
{
    //old imediate mode!
    //Set the clear colour(background)
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    //clear the colour and depth buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //Swith to ModelView
    glMatrixMode( GL_MODELVIEW );
    //Reset using the Indentity Matrix
    glLoadIdentity( );
	setCameraProperties(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

int main(int argc, char * arg[])
{
    //Controls the game loop
    bool run=true;
    bool pause=false;
    // init everyting - SDL, if it is nonzero we have a problem
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "ERROR SDL_Init " <<SDL_GetError()<< std::endl;

        return -1;
	}    
	
	//Request opengl 4.1 context, Core Context
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
            if (event.type==SDL_WINDOWEVENT)
            {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                            pause=true;
                        break;
                    default:
                        break;
                }

            }
        }
        //init Scene
        //render
        render();
        //Call swap so that our GL back buffer is displayed
        SDL_GL_SwapWindow(window);

    }

    // clean up, reverse order!!!
    cleanUp();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
