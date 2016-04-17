
#include "shared.h"
#include "shared.cpp"

#include "hogl.h"
#include "hogl.cpp"

#include <iostream>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

glm::mat4 PROJECTION;
glm::mat4 VIEW;
glm::vec3 LIGHT_POSITION;

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	//NOTE(brett): initialize game code
	int num_joysticks = SDL_NumJoysticks();
	if(num_joysticks > 0) {
		if(SDL_IsGameController(0)){
			SDL_GameControllerOpen(0);
		}
	}

	bool running = true;
	int startTime = SDL_GetTicks();
	int currentTime = startTime;

	float targetFPS = 60.f;
	float targetFrameTimeS = 1.0f/targetFPS;

	float frameTime = targetFrameTimeS;
	std::cout << "Target Frame time: " <<  targetFrameTimeS << std::endl;

	char *vertexShader, *fragmentShader;
	ReadFile(&vertexShader, "shaders/flat_shader.vs");
	ReadFile(&fragmentShader, "shaders/flat_shader.fs");

	GLuint shader;
	CreateShaderProgram(&shader, vertexShader, fragmentShader);
	glUseProgram(shader);

	PROJECTION = glm::ortho(-WINDOW_WIDTH/2.f,
	                        WINDOW_WIDTH/2.f,
	                        -WINDOW_HEIGHT/2.f,
	                        WINDOW_HEIGHT/2.f,
	                        0.1f,
	                        1000.f);

	// PROJECTION = glm::perspectiveFov(45.f,
	//                                  (float)WINDOW_WIDTH,
	//                                  (float)WINDOW_HEIGHT,
	//                                  0.1f,
	//                                  1000.f);

	st_camera camera = {};
	camera.eye       = { 0.f, 300.f, 300.f };
	camera.lookat    = { 0.f, 0.f,  0.f };
	camera.up        = { 0.f, 1.f,  0.f };

	VIEW = glm::lookAt(camera.eye, camera.lookat, camera.up);


	st_scene scene;

	// load models
	st_entity_model gridModel = {};	
	gridModel.draw_method = GL_LINES;
	CreateModel(&gridModel, GRID_VERTICES_5, sizeof(GRID_VERTICES_5)/sizeof(GLfloat), glm::vec3{1.f, 1.f, 1.f}, shader);

	st_entity_model boxModel = {};
	boxModel.draw_method = GL_TRIANGLES;
	CreateModel(&boxModel, BOX_VERTICES, sizeof(BOX_VERTICES)/sizeof(GLfloat), glm::vec3{0.f, 0.5f, 1.0f}, shader);

	st_entity_model boxModelOrange = {};
	boxModelOrange.draw_method = GL_TRIANGLES;
	CreateModel(&boxModelOrange, BOX_VERTICES, sizeof(BOX_VERTICES)/sizeof(GLfloat), glm::vec3{1.f, 0.5f, 0.0f}, shader);

	// create entities

	st_entity wallBottom = {};
	wallBottom.model     = &boxModelOrange;
	wallBottom.scale     = glm::vec3{160.f, 10.f, 10.f};
	wallBottom.position  = glm::vec3{0.f, 5.f, 80.f};

	st_entity wallTop = {};
	wallTop.model     = &boxModelOrange;
	wallTop.scale     = glm::vec3{160.f, 10.f, 10.f};
	wallTop.position  = glm::vec3{0.f, 5.f, -80.f};

	st_entity grid = {};
	grid.model     = &gridModel;
	grid.scale     = glm::vec3{40.f, 40.f, 40.f};
	
	st_entity player = {};
	player.model     = &boxModel;
	player.scale     = glm::vec3{20.f, 20.f, 20.f};
	player.position  = glm::vec3{0.f, 10.f, 0.f};

	float power       = 0.f;
	float powerChange = 100.f; // per second

	glm::vec3 cameraMotion = {};
	glm::vec2 motion       = {};

	SDL_Event event;
	while(running) {
		int lastTime = currentTime;
		currentTime = SDL_GetTicks();

		float elapsedTimeMS = currentTime - lastTime;
		float elapsedTimeS = elapsedTimeMS/1000.f;

		frameTime += elapsedTimeS;

		if(frameTime >= targetFrameTimeS) {
			while(SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_JOYDEVICEADDED:
						std::cout << "sdl device added: " << event.jdevice.which << std::endl;
						if(SDL_IsGameController(0)){
							SDL_GameControllerOpen(0);
						}
						break;

					case SDL_CONTROLLERAXISMOTION: 
					{
						int axis = (int)event.jaxis.axis;
						switch(axis){
							case SDL_CONTROLLER_AXIS_LEFTX:
								// TODO(brett): this needs to be fixed so we arent 'leaking' over 1 or below 0
								motion.x = event.jaxis.value/(float)0x7FFF;
								if(fabs(motion.x) < 0.2f) motion.x = 0.f;
								break;

							case SDL_CONTROLLER_AXIS_LEFTY:
								motion.y = event.jaxis.value/(float)0x7FFF;

								if(fabs(motion.y) < 0.2f) motion.y = 0.f;
								break;
						}
						
						break;
					}
					//https://wiki.libsdl.org/SDL_ControllerButtonEvent
					case SDL_CONTROLLERBUTTONDOWN:
					{
						switch(event.cbutton.button) {
							case SDL_CONTROLLER_BUTTON_A:
							break;
						}
						break;
					}
					case SDL_CONTROLLERBUTTONUP:
					{
						switch(event.cbutton.button) {
							case SDL_CONTROLLER_BUTTON_A:
							break;
						}
						// SDL_Haptic *haptic = SDL_HapticOpen(0);
						// SDL_HapticRumbleInit(haptic);
						// SDL_HapticRumblePlay(haptic, 0.5f, 300);
						// SDL_HapticClose(haptic);
						// std::cout << "sdl joy button event"  << std::endl;
						break;
					}
				}
			}

			std::cout << "\rpos: " << glm::to_string(player.position) << "  motion: " << glm::to_string(motion) << "   FrameTime: " << frameTime;

			cameraMotion.x = player.position.x - camera.lookat.x;
			cameraMotion.z = player.position.z - camera.lookat.z;

			cameraMotion /= 10.f;

			camera.lookat += cameraMotion;
			camera.eye += cameraMotion;

			player.position.x += motion.x * frameTime * 250.f;
			player.position.z += motion.y * frameTime * 250.f;

			LIGHT_POSITION = glm::normalize(camera.eye - camera.lookat) * 50.f + camera.lookat;
			VIEW = glm::lookAt(camera.eye, camera.lookat, camera.up);

			// std::cout << "\rcamera position " << glm::to_string(LIGHT_POSITION) << std::endl;

			glClearColor(0.3, 0.0, 0.3, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawEntity(&grid, PROJECTION, VIEW, LIGHT_POSITION);
			DrawEntity(&wallTop, PROJECTION, VIEW, LIGHT_POSITION);
			DrawEntity(&wallBottom, PROJECTION, VIEW, LIGHT_POSITION);
			DrawEntity(&player, PROJECTION, VIEW, LIGHT_POSITION);

			SDL_GL_SwapWindow(mainWindow);

			frameTime = frameTime - targetFrameTimeS;
			if(frameTime > targetFrameTimeS) frameTime = targetFrameTimeS;
		}
	}


	return 0;
}