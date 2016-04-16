
#include "hogl.h"
#include "hogl.cpp"

#include <iostream>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

typedef struct {

} controller;

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

	GLenum err = glewInit();

	if(!mainContext){
		std::cout << "Could not init gl" << std::endl;
	}

	SDL_GL_SetSwapInterval(1);

	int num_joysticks = SDL_NumJoysticks();

	if(num_joysticks > 0) {
		SDL_JoystickOpen(0);
	}

	bool running = true;
	int startTime = SDL_GetTicks();
	int currentTime = startTime;

	float targetFPS = 60.f;
	float targetFrameTimeS = 1.0f/targetFPS;

	float frameTime = targetFrameTimeS;

	std::cout << "Target Frame time: " <<  targetFrameTimeS << std::endl;

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLfloat triangle[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.0f,  0.5f,  0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), (GLfloat*)&triangle, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	// void glVertexAttribPointer(	GLuint index,
	// 						 	GLint size,
	// 						 	GLenum type,
	// 						 	GLboolean normalized,
	// 						 	GLsizei stride,
	// 						 	const GLvoid * pointer);
 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint shader;

	char *vertexShader, *fragmentShader;
	ReadFile(&vertexShader, "shaders/flat_shader.vs");
	ReadFile(&fragmentShader, "shaders/flat_shader.fs");

	CreateShaderProgram(&shader, vertexShader, fragmentShader);

	glUseProgram(shader);

	SDL_Event event;
	while(running) {
		int lastTime = currentTime;
		currentTime = SDL_GetTicks();

		int elapsedTimeMS = currentTime - lastTime;
		float elapsedTimeS = elapsedTimeMS/1000.f;

		// std::cout << "elapsed ms: " << elapsedTimeMS << " => " << elapsedTimeS << std::endl;
		frameTime += elapsedTimeS;

		if(frameTime >= targetFrameTimeS) {
			while(SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_JOYDEVICEADDED:
						std::cout << "sdl device added: " << event.jdevice.which << std::endl;
						SDL_JoystickOpen(0);
						break;

					case SDL_JOYAXISMOTION:
						std::cout << "sdl joy axis event"  << std::endl;
						break;

					case SDL_JOYBUTTONDOWN:
					case SDL_JOYBUTTONUP:
						std::cout << "sdl joy button event"  << std::endl;
						break;
				}
			}

			
			glClearColor(1.0, 0.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			SDL_GL_SwapWindow(mainWindow);

			frameTime = frameTime - targetFrameTimeS;
			if(frameTime > targetFrameTimeS) frameTime = targetFrameTimeS;
		}
	}


	return 0;
}