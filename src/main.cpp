#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

int main(int argc, char* argv[]) {

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SOMETHING WENT WRONG!!!" << std::endl;
	}

	SDL_Window *mainWindow = SDL_CreateWindow("ludum_dare_35",
	                                          SDL_WINDOWPOS_CENTERED,
	                                          SDL_WINDOWPOS_CENTERED,
	                                          WINDOW_WIDTH,
	                                          WINDOW_HEIGHT,
	                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if(!mainWindow) {
		std::cout << "Could not create window" << std::endl;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext mainContext = SDL_GL_CreateContext(mainWindow);


	if(!mainContext){
		std::cout << "Could not init gl" << std::endl;
	}


	SDL_GL_SetSwapInterval(1);

	bool running = true;
	SDL_Event event;
	while(running) {
		while(SDL_PollEvent(&event)) {

		}

		glClearColor(1.0, 0.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(mainWindow);
	}


	return 0;
}