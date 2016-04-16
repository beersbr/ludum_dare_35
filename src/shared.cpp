#include "shared.h"

void CreateCollisionNodeSphere(st_collision_node *node, float radius, glm::vec3 position) {
	node->type = SPHERE;
	node->sphere.radius = radius;
	node->sphere.origin = position;
}

void  CreateCollisionNodeBox(st_collision_node *node, glm::vec3 origin, glm::vec3 size) {
	node->type = RECTANGLE;
	node->box.origin = origin;
	node->box.size = size;
}