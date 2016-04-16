#include <iostream>
#include <SDL2/SDL.h>

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
		return -1;
	}

	bool running = true;

	SDL_Event event;

	while(running) {
		while(SDL_PollEvent(&event)){}
	}


	return 0;
}