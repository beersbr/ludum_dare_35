#ifndef __SHARED_HPP
#define __SHARED_HPP

#include <glm/glm.hpp>

enum COLLISION_NODE_TYPE {
	SPHERE = 1,
	RECTANGLE,
	COLLISION_NODE_TYPES_COUNT
};

typedef struct {

} controller;

struct st_sphere {
	glm::vec3 position;
	float radius;
};

struct st_box {
	glm::vec3 origin;
	glm::vec3 size;
};

struct collision_type {
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
} collision_node;

typedef struct {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 st;
} model_vertex;

typedef struct {	
	model_vertex *vertices;
	int vertice_count;
	int texture_id;
} entity_model;

typedef struct { 
	glm::vec3 position;
	collision_node collision;
	entity_model model;
} entity;

void CreateCollisionNodeSphere(collision_node *node, float radius, glm::vec3 position);
void CreateCollisionNodeRectangle(collision_node *node, glm::vec3 origin, glm::vec3 size);

#endif // __SHARED_HPP