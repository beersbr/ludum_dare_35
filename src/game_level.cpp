
#include "game_level.h"

void CreateWall(st_entity *entity, st_entity_model *model, bool collidable, glm::vec3 p0, glm::vec3 p1) {
	glm::vec3 center = p0 + ((p1 - p0) / 2.f);
	float size = glm::length(center - p0);

	entity->model    = model;
	entity->scale    = glm::vec3{size*2.f, 10.f, 10.f};
	entity->position = center;

	entity->position.y = 5.f;

	float angle = acos(glm::dot(glm::vec3{1.f, 0.f, 0.f}, p1 - p0));
	entity->rotation = glm::vec3{0.f, angle, 0.f};
	CreateCollisionLine(entity, false, p0, p1);

}