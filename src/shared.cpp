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

void CreateModel(st_entity_model *model, GLfloat *vertices, glm::vec3 color1, GLuint shader) {

	glGenVertexArrays(1, &model->VAO);
	glGenBuffers(1, &model->VBO);

	model->vertices = (st_model_vertex*)malloc(sizeof(st_model_vertex) * sizeof(vertices));
	model->draw_method = GL_TRIANGLES;

	for(int i = 0; i < sizeof(vertices); i += 3) {
		int vert_index = i/3;

	}
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