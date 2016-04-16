#ifndef __SHARED_HPP
#define __SHARED_HPP

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

enum COLLISION_NODE_TYPE {
	SPHERE = 1,
	RECTANGLE,
	BOX,
	COLLISION_NODE_TYPES_COUNT
};

typedef struct {

} st_controller;

typedef struct { 
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
} st_camera;

struct st_sphere {
	glm::vec3 position;
	float radius;
};

struct st_box {
	glm::vec3 origin;
	glm::vec3 size;
};

struct st_collision_type {
	glm::vec3 origin;
};

typedef struct{
	COLLISION_NODE_TYPE type;
	struct {
		glm::vec3 origin;
		float radius;
	} sphere;

	struct {
		glm::vec3 origin;
		glm::vec3 size;
	} box;
} st_collision_node;

typedef struct {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 st;
} st_model_vertex;

typedef struct {
	st_model_vertex *vertices;
	int vertice_count;
	int texture_id;

	GLuint shader_id;
	GLenum draw_method;

	GLuint VAO;
	GLuint VBO;
} st_entity_model;

typedef struct { 
	glm::vec3 position;
	st_collision_node collision;
	st_entity_model *model;
} st_entity;

typedef struct { 
	st_entity *entities;
	int entity_count;
} st_scene;

void CreateModel(st_entity_model *model, GLfloat *vertices, glm::vec3 color1, GLuint shader_id);

void CreateCollisionNodeSphere(st_collision_node *node, float radius, glm::vec3 position);
void CreateCollisionNodeRectangle(st_collision_node *node, glm::vec3 origin, glm::vec3 size);

void GetBoundingBox(st_entity *entity);
void CheckCollision(st_entity *a, st_entity *b);

#endif // __SHARED_HPP