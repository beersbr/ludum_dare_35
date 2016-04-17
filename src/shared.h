#ifndef __SHARED_HPP
#define __SHARED_HPP

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>

enum COLLISION_NODE_TYPE {
	NONE = 0,
	CIRCLE,
	LINE,
	SPHERE,
	RECTANGLE,
	BOX,
	COLLISION_NODE_TYPES_COUNT
};

GLfloat GRID_VERTICES_5[] = {
	-2.f,  0.f, -2.f,
	-2.f,  0.f,  2.f,
	-1.f,  0.f, -2.f,
	-1.f,  0.f,  2.f,
	 0.f,  0.f, -2.f,
	 0.f,  0.f,  2.f,
	 1.f,  0.f, -2.f,
	 1.f,  0.f,  2.f,
	 2.f,  0.f, -2.f,
	 2.f,  0.f,  2.f,

	-2.f,  0.f, -2.f,
	 2.f,  0.f, -2.f,
	-2.f,  0.f, -1.f,
	 2.f,  0.f, -1.f,
	-2.f,  0.f,  0.f,
	 2.f,  0.f,  0.f,
	-2.f,  0.f,  1.f,
	 2.f,  0.f,  1.f,
	-2.f,  0.f,  2.f,
	 2.f,  0.f,  2.f,
};

GLfloat BOX_VERTICES[] = {
	// back
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	// right
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,

	 // front
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,

	 // left
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,

	// top
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	 // bottom
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
};

typedef struct {

} st_controller;

typedef struct { 
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
} st_camera;

struct st_sphere {
	glm::vec3 origin;
	float radius;
};
typedef st_sphere st_circle;

struct st_box {
	glm::vec3 origin;
	glm::vec3 size;
};

struct st_line {
	glm::vec3 origin;
	glm::vec3 destination;
};

struct st_ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

struct st_collision_type {
	glm::vec3 origin;
};

typedef struct{
	COLLISION_NODE_TYPE type;
	bool is_movable;
	glm::vec3 parent_position_offset;

	st_sphere sphere;
	st_box box;
	st_line line;

} st_collision_node;

typedef struct {
	glm::vec3 position;
	glm::vec3 normal;
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
	glm::vec3 scale;
	glm::vec3 rotation;

	st_collision_node collider;
	st_entity_model *model;

} st_entity;

typedef struct { 
	st_entity **entities;
	int entity_count;
} st_scene;

void CreateModel(st_entity_model *model, GLfloat *vertices, int vertices_size, glm::vec3 color1, GLuint shader_id);
void DrawEntity(st_entity *entity, glm::mat4 projection, glm::mat4 view, glm::vec3 light_pos);

void CreateCollisionSphere(st_entity *entity, bool movable,  float radius, glm::vec3 position);
void CreateCollisionBox(st_entity *entity, bool movable,  glm::vec3 origin, glm::vec3 size);
void CreateCollisionLine(st_entity *entity, bool movable, glm::vec3 origin, glm::vec3 destination);
void UpdateCollider(st_entity* entity);
void ResolveCollision(st_entity *subject, st_entity *guest);

bool SphereLineCollision(st_collision_node *a, st_collision_node *b, glm::vec3 *out);

void GetBoundingBox(st_entity *entity);
void CheckCollision(st_entity *a, st_entity *b);

void PrepareScene(st_scene *scene, int scene_size);
void AddToScene(st_scene *scene, st_entity *entity);
void UpdateAndRenderScene(st_scene *scene, glm::mat4 projection, glm::mat4 view);

#endif // __SHARED_HPP