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
void CreateModel(st_entity_model *model, GLfloat *vertices, int vertices_size, glm::vec3 color1, GLuint shader_id) {

	int model_vertex_count = vertices_size/3;
	model->vertices = (st_model_vertex*)malloc(sizeof(st_model_vertex)*model_vertex_count);
	model->draw_method = GL_TRIANGLES;

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

	model->shader_id = shader_id;

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
	glUseProgram(entity->model->shader_id);

	GLint projectionLocation = glGetUniformLocation(entity->model->shader_id, "projection");
	GLint viewLocation = glGetUniformLocation(entity->model->shader_id, "view");
	GLint lightLocation = glGetUniformLocation(entity->model->shader_id, "light");

	glUniformMatrix4fv(projectionLocation, 1, false, (GLfloat*)&projection[0]);
	glUniformMatrix4fv(viewLocation, 1, false, (GLfloat*)&view[0]);
	glUniform3fv(lightLocation, 1, (GLfloat*)&light_pos[0]);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, entity->position);
	model = glm::scale(model, glm::vec3{ 20.f, 20.f, 20.f });

	GLint modelLocation = glGetUniformLocation(entity->model->shader_id, "model");
	glUniformMatrix4fv(modelLocation, 1, false, (GLfloat*)&model);

	glDrawArrays(entity->model->draw_method, 0, entity->model->vertice_count);
}

void CreateCollisionNodeSphere(st_collision_node *node, float radius, glm::vec3 position) {
	node->type = SPHERE;
	node->sphere.radius = radius;
	node->sphere.origin = position;
}

void  CreateCollisionNodeBox(st_collision_node *node, glm::vec3 origin, glm::vec3 size) {
	node->type = BOX;
	node->box.origin = origin;
	node->box.size = size;
}