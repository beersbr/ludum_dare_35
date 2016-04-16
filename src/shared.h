#ifndef __SHARED_HPP
#define __SHARED_HPP

#include <glm/glm.hpp>

enum COLLISION_NODE_TYPE {
	SPHERE = 1,
	RECTANGLE,
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
} st_entity_model;

typedef struct { 
	glm::vec3 position;
	st_collision_node collision;
	st_entity_model model;
} st_entity;

typedef struct { 
	//TODO(brett): make a scene to move stuff around faster
} st_scene; 

void CreateCollisionNodeSphere(st_collision_node *node, float radius, glm::vec3 position);
void CreateCollisionNodeRectangle(st_collision_node *node, glm::vec3 origin, glm::vec3 size);

#endif // __SHARED_HPP