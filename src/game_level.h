
#ifndef __GAME_LEVEL_H
#define __GAME_LEVEL_H

#include "shared.h"

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>

void CreateWall(st_entity *entity, st_entity_model *model, bool collidable, glm::vec3 p0, glm::vec3 p1);

#endif