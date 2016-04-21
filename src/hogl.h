#ifndef __HOGL_HPP
#define __HOGL_HPP

// #include <OpenGL/opengl.h>
// #include <OpenGL/gl3.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <fstream>
#include <assert.h>

#define MAX_SHADER_VARIABLE_NAME_LENGTH 128
#define MAX_SHADER_UNIFORMS 128

typedef struct {
	GLuint id;
	GLint position;

	char name[MAX_SHADER_VARIABLE_NAME_LENGTH];

	bool is_dirty;
	union {
		GLfloat   float_value;
		GLint     int_value;
		GLuint    uint_value;
		GLfloat  vec2_value[2];
		GLfloat  vec3_value[3];
		GLfloat  vec4_value[4];
		GLfloat  mat3_value[9];
		GLfloat  mat4_value[16];
	};
} st_shader_value;

typedef struct  {
	GLuint id;

	st_shader_value uniforms[MAX_SHADER_UNIFORMS];
	int uniforms_count;

} st_shader;

void GetOrCreateUniform(st_shader *shader, char *name, st_shader_value *value);
void GetOrCreateVariable(st_shader_value *variables, int *variables_count, char *name, st_shader_value *value);

void SetShaderUniformFloat(st_shader *shader, char *name, float value);
void SetShaderUniformInt(st_shader *shader, char *name, int value);
void SetShaderUniformUnsignedInt(st_shader *shader, char *name, unsigned int value);
void SetShaderUniformVec2(st_shader *shader, char *name, float value[2]);
void SetShaderUniformVec3(st_shader *shader, char *name, float value[3]);
void SetShaderUniformVec4(st_shader *shader, char *name, float value[4]);
void SetShaderUniformMatrix3(st_shader *shader, char *name, float value[9]);
void SetShaderUniformMatrix4(st_shader *shader, char *name, float value[16]);


void ReadFile(char **fileContents, char *filename);
void CompileShader(GLuint *shader, char *shaderSource);
void CreateShaderProgram(st_shader *shader, char *vertexShader, char *fragmentShader);
void CreateTexture(GLuint *textureId, char *texturePath);

#endif