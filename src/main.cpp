
#include "shared.h"
#include "shared.cpp"

#include "hogl.h"
#include "hogl.cpp"

#include "net/net.h"
#include "net/net.cpp"

#include "game_level.h"
#include "game_level.cpp"

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

	std::list<st_entity> playerEntities;
	st_entity_info* net_entities = 0;
	unsigned int net_ent_count = 0;

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


	//Init winsock
	init_game_winsock();


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
	wallBottom.info.scale     = glm::vec3{160.f, 10.f, 10.f};
	wallBottom.info.position  = glm::vec3{0.f, 5.f, 80.f};
	CreateCollisionLine(&wallBottom, false, glm::vec3{-80.f, 0.f, 80.f}, glm::vec3{80.f, 0.f, 80.f});

	st_entity wallTop = {};
	wallTop.model     = &boxModelOrange;
	wallTop.info.scale     = glm::vec3{160.f, 10.f, 10.f};
	wallTop.info.position  = glm::vec3{0.f, 5.f, -80.f};
	CreateCollisionLine(&wallTop, false, glm::vec3{-80.f, 0.f, -80.f}, glm::vec3{80.f, 0.f, -80.f});

	st_entity grid = {};
	grid.model     = &gridModel;
	grid.info.scale     = glm::vec3{40.f, 40.f, 40.f};

	//Get player entities from server
	int join_status = JoinGame(&net_entities, &net_ent_count);
	if(!join_status)
	{
		//Couldn't join a game, we can control what we want here, but quit for now
		std::cout << "Netcode error from JoinGame: Couldn't join a game" << std::endl;
		return 1;
	}
	//Set up our player entities from the list.
	/*
	st_entity player = {};
	player.model     = &boxModel;
	player.scale     = glm::vec3{20.f, 20.f, 20.f};
	player.position  = glm::vec3{0.f, 10.f, 0.f};
	*/
	//We need better serialization, but for now lets just assume they're all boxes
	st_entity_info* tmp_ent = net_entities;

	for(int i = 0; i < net_ent_count; i++)
	{
		//Create entities based on what we got
		st_entity tmp = {};
		tmp.model = &boxModel;
		tmp.info.scale = tmp_ent->scale;
		tmp.info.position = tmp_ent->position;
		CreateCollisionSphere(&tmp, true, 10.f, glm::vec3{});
		tmp_ent++;
		playerEntities.push_back(tmp);
	}

	//For now, assume player is the one and only entity (for net testing)
	st_entity player = playerEntities.front();

	st_entity wallRight = {} ;
	CreateWall(&wallRight, &boxModelOrange, true, glm::vec3{80.f, 0.f, 80.f}, glm::vec3{80.f, 0.f, -80.f});
	
	st_scene scene;
	PrepareScene(&scene, 1024);

	st_entity entities[1024];
	int entity_count = 0;

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-80.f, 0.f, 80.f}, glm::vec3{-80.f, 0.f, 560.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-80.f, 0.f, -80.f}, glm::vec3{-80.f, 0.f, -560.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-80.f, 0.f, -560.f}, glm::vec3{-80.f, 0.f, -80.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-880.f, 0.f, -560.f}, glm::vec3{-80.f, 0.f, -560.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-880.f, 0.f, -560.f}, glm::vec3{-880.f, 0.f, 560.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	CreateWall(&entities[entity_count], &boxModelOrange, true, glm::vec3{-880.f, 0.f,  560.f}, glm::vec3{-80.f, 0.f, 560.f}); 
	AddToScene(&scene, &entities[entity_count++]);

	AddToScene(&scene, &wallRight);
	AddToScene(&scene, &wallBottom);
	AddToScene(&scene, &wallTop);
	AddToScene(&scene, &grid);
	AddToScene(&scene, &player);

	float power       = 0.f;
	float powerChange = 100.f; // per second

	glm::vec3 cameraMotion    = {};
	glm::vec3 cameraDirection = camera.lookat - camera.eye;
	float turnMotion          = 0.f;
	glm::vec2 motion          = {};

	glm::vec3 playerVelocity = {};
	float forwardPower = 0.f;
	float lateralPower = 0.f;

	float playerRotation = 0.f;


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
								lateralPower = event.jaxis.value/(float)0x7FFF;
								if(fabs(lateralPower) < 0.2f) lateralPower = 0.f;
								lateralPower *= 8.f;
								break;

							case SDL_CONTROLLER_AXIS_LEFTY:
							{
								forwardPower = event.jaxis.value/(float)0x7FFF;
								if(fabs(forwardPower) < 0.2f) forwardPower = 0.f;
								forwardPower *= 40.f;
								break;
							}
							case SDL_CONTROLLER_AXIS_RIGHTX:

								turnMotion = event.jaxis.value/(float)0x7FFF;
								if(fabs(turnMotion) < 0.2f) turnMotion = 0.f;

								turnMotion *= 90.f;
								turnMotion *= frameTime;
								break;

							case SDL_CONTROLLER_AXIS_RIGHTY:
								// motion.y = event.jaxis.value/(float)0x7FFF;
								// if(fabs(motion.y) < 0.2f) motion.y = 0.f;
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

			std::cout << "\rpos: " << glm::to_string(player.info.position) << "  motion: " << glm::to_string(motion) << "   FrameTime: " << frameTime;

			// this needs to be in the scene update
			cameraMotion.x = player.info.position.x - camera.lookat.x;
			cameraMotion.z = player.info.position.z - camera.lookat.z;

			cameraMotion /= 5.f;

			camera.lookat += cameraMotion;
			camera.eye += cameraMotion;

			glm::vec3 acceleration = -glm::normalize(glm::vec3{cameraDirection.x, 0.f, cameraDirection.z}) * (forwardPower * frameTime);
			acceleration += -glm::normalize(glm::rotate(glm::vec3{cameraDirection.x, 0.f, cameraDirection.z}, Radians(90.f), glm::vec3{0.f, 1.f, 0.f})) * (lateralPower * frameTime);
			playerVelocity += acceleration;
			player.info.position += playerVelocity;
			player.info.rotation = glm::vec3{0.f, -Radians(playerRotation += turnMotion), 0.f};

			playerVelocity *= 0.92f;


			LIGHT_POSITION = glm::normalize(camera.eye - camera.lookat) * 50.f + camera.lookat;

			cameraDirection = glm::rotate(cameraDirection, Radians(-turnMotion), glm::vec3{0.f, 1.f, 0.f});

			camera.eye = camera.lookat - cameraDirection;
			VIEW = glm::lookAt(camera.eye, camera.lookat, camera.up);

			// std::cout << "\rcamera position " << glm::to_string(LIGHT_POSITION) << std::endl;

			glClearColor(0.3, 0.0, 0.3, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			UpdateAndRenderScene(&scene, PROJECTION, VIEW);

			SDL_GL_SwapWindow(mainWindow);

			frameTime = frameTime - targetFrameTimeS;
			if(frameTime > targetFrameTimeS) frameTime = targetFrameTimeS;
		}
	}


	return 0;
}