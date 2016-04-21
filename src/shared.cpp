#include "shared.h"

// typedef struct {
// 	st_model_vertex *vertices;
// 	int vertice_count;
// 	int texture_id;

// 	GLuint shader_id;
// 	GLenum draw_method;

// 	GLuint VAO;
// 	GLuint VBO;
// } st_entity_model;

// NOTE(brett): should be done with triangles
void CreateModel(st_entity_model *model, GLfloat *vertices, int vertices_size, glm::vec3 color1, st_shader *shader) {

	// NOTE(brett): get vertices 
	int model_vertex_count = vertices_size/3;

	model->vertices = (st_model_vertex*)malloc(sizeof(st_model_vertex)*model_vertex_count);
	// model->draw_method = GL_TRIANGLES;

	if(model->draw_method == GL_TRIANGLES) {
		for(int i = 0; i < vertices_size; i += 9) {
			int vi = i/3;

			glm::vec3 v0 = glm::vec3{ vertices[i], vertices[i+1], vertices[i+2] };
			glm::vec3 v1 = glm::vec3{ vertices[i+3], vertices[i+4], vertices[i+5] };
			glm::vec3 v2 = glm::vec3{ vertices[i+6], vertices[i+7], vertices[i+8] };

			glm::vec3 normal = glm::normalize(glm::cross((v2 - v0), (v1 - v0)));

			model->vertices[vi] = st_model_vertex{ glm::vec3{vertices[i], vertices[i+1], vertices[i+2] },
												   normal,
												   color1,
												   glm::vec2{} };

			model->vertices[vi+1] = st_model_vertex{ glm::vec3{vertices[i+3], vertices[i+4], vertices[i+5] },
												   normal,
												   color1,
												   glm::vec2{} };

			model->vertices[vi+2] = st_model_vertex{ glm::vec3{vertices[i+6], vertices[i+7], vertices[i+8] },
												   normal,
												   color1,
												   glm::vec2{} };

		}		
	}
	else if(model->draw_method == GL_LINES) {
		for(int i = 0; i < vertices_size; i+= 3) {
			int vi = i/3;

			glm::vec3 point = glm::vec3{ vertices[i], vertices[i+1], vertices[i+2] };
			model->vertices[vi] = st_model_vertex{ point, glm::vec3{0.f, 1.f, 0.f}, color1, glm::vec2 {} };

		}
	}

	model->shader = shader;

	model->vertice_count = model_vertex_count;

	glGenVertexArrays(1, &model->VAO);
	glBindVertexArray(model->VAO);
	glGenBuffers(1, &model->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, model->VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(st_model_vertex)*model_vertex_count, (GLfloat*)&model->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(st_model_vertex), (void*)offsetof(st_model_vertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(st_model_vertex), (void*)offsetof(st_model_vertex, normal));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(st_model_vertex), (void*)offsetof(st_model_vertex, color));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(st_model_vertex), (void*)offsetof(st_model_vertex, st));
}

void DrawEntity(st_entity *entity, glm::mat4 projection, glm::mat4 view,  glm::vec3 light_pos) {

	glBindVertexArray(entity->model->VAO);
	glUseProgram(entity->model->shader->id);

	GLint projectionLocation = glGetUniformLocation(entity->model->shader->id, "projection");
	GLint viewLocation = glGetUniformLocation(entity->model->shader->id, "view");
	GLint lightLocation = glGetUniformLocation(entity->model->shader->id, "light");

	glUniformMatrix4fv(projectionLocation, 1, false, (GLfloat*)&projection[0]);
	glUniformMatrix4fv(viewLocation, 1, false, (GLfloat*)&view[0]);
	glUniform3fv(lightLocation, 1, (GLfloat*)&light_pos[0]);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, entity->info.position);

	model = glm::rotate(model, entity->info.rotation.x, glm::vec3{1.f, 0.f, 0.f});
	model = glm::rotate(model, entity->info.rotation.y, glm::vec3{0.f, 1.f, 0.f});
	model = glm::rotate(model, entity->info.rotation.z, glm::vec3{0.f, 0.f, 1.f});

	model = glm::scale(model, entity->info.scale);

	GLint modelLocation = glGetUniformLocation(entity->model->shader->id, "model");
	glUniformMatrix4fv(modelLocation, 1, false, (GLfloat*)&model);

	glDrawArrays(entity->model->draw_method, 0, entity->model->vertice_count);
}

void CreateCollisionSphere(st_entity *entity, bool movable, float radius, glm::vec3 position) {
	st_collision_node *node = &(entity->info.collider);
	node->type = SPHERE;
	node->is_movable = movable;
	node->parent_position_offset = (position - entity->info.position);

	node->sphere.radius = radius;
	node->sphere.origin = position;
}

void CreateCollisionBox(st_entity *entity, bool movable, glm::vec3 origin, glm::vec3 size) {
	st_collision_node *node = &(entity->info.collider);
	node->type = BOX;
	node->parent_position_offset = (entity->info.position - origin);
	node->box.origin = origin;
	node->box.size = size;
}

void CreateCollisionLine(st_entity *entity, bool movable, glm::vec3 origin, glm::vec3 destination) {
	st_collision_node *node = &(entity->info.collider);
	node->type = LINE;
	node->parent_position_offset = (entity->info.position - origin);

	node->line.origin = origin;
	node->line.destination = destination;
}

void UpdateCollider(st_entity* entity) {
	st_collision_node *collider = &entity->info.collider;
	if(collider->is_movable) {
		switch(collider->type) {
			case LINE:
			{
				glm::vec3 delta = collider->line.destination - collider->line.origin;
				collider->line.origin = entity->info.position + collider->parent_position_offset;
				collider->line.destination = collider->line.origin + delta;
				break;
			}
			case SPHERE:
			{
				collider->sphere.origin = entity->info.position + collider->parent_position_offset;
				break;
			}
		}	
	}
}

bool SphereLineCollision(st_collision_node *sphere, st_collision_node *line, glm::vec3 *out) {
	glm::vec3 floor = line->line.destination - line->line.origin;
	glm::vec3 hypot = sphere->sphere.origin - line->line.origin;

	glm::vec3 projected = glm::proj(hypot, floor);

	float projection_length = glm::length(projected);

	if(glm::normalize(floor) * projection_length != projected)
		return false;

	if(projection_length < (glm::length(floor) + sphere->sphere.radius)) {
		glm::vec3 delta = hypot - projected;
		float delta_length = glm::length(delta);

		if(delta_length < sphere->sphere.radius) {
			*out = (delta_length - sphere->sphere.radius) * glm::normalize(delta);
			return true;
		}
	}

	return false;
}


void ResolveCollision(st_entity *subject, st_entity *guest) {
	if (!subject->info.collider.is_movable)
		return;

	bool didCollide = false;
	glm::vec3 shouldMove = {};

	switch(subject->info.collider.type) {
		case SPHERE:
		{
			switch(guest->info.collider.type) {
				case LINE:
				{
					didCollide = SphereLineCollision(&subject->info.collider, &guest->info.collider, &shouldMove);
				}
				break;
			}
			break;
		}
		case LINE:
		{
			switch(guest->info.collider.type) {
				case SPHERE:
				{
					didCollide = SphereLineCollision(&guest->info.collider, &subject->info.collider, &shouldMove);
				}
				break;
			}
			break;
		}
	}

	if (didCollide) {
		subject->info.position -= shouldMove;
	}

}

void PrepareScene(st_scene *scene, int scene_size) {
	scene->entities = (st_entity**)malloc(sizeof(st_entity*) * scene_size);
	scene->entity_count = 0;
}

void AddToScene(st_scene *scene, st_entity *entity) {
	scene->entities[scene->entity_count] = entity;
	scene->entity_count += 1;
}

// this is mostly just collision and rendering
void UpdateAndRenderScene(st_scene *scene, glm::mat4 projection, glm::mat4 view) {
	glm::vec3 light_pos = glm::vec3{0.f, 140.f, 0.f};

	for(int i = 0; i < scene->entity_count; i++) {

		st_entity *current_entity = scene->entities[i];

		if(current_entity->info.collider.type != NONE){
			UpdateCollider(current_entity);
			// NOTE(brett): naive collision check
			for(int e = 0; e < scene->entity_count; e++) {
				if(current_entity == scene->entities[e]) continue;
				ResolveCollision(current_entity, scene->entities[e]);
			}
		}
		
		DrawEntity(scene->entities[i], projection, view, light_pos);
	}
}