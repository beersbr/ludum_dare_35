#include "hogl.h"

void GetOrCreateUniform(st_shader *shader, char *name, st_shader_value *value) {
	GetOrCreateVariable(shader->uniforms, &shader->uniforms_count, name, value);
}

void GetOrCreateVariable(st_shader_value *variables, int *variables_count, char *name, st_shader_value *value) {
	for(int i = 0; i < *variables_count; ++i) {
		if(strncmp(variables[i].name, name, MAX_SHADER_VARIABLE_NAME_LENGTH) == 0) {
			value = &(variables[i]);
			break;
		}
	}

	if(*variables_count + 1 < MAX_SHADER_UNIFORMS) {
		value = (variables + *variables_count);
		memcpy(value->name, name, MAX_SHADER_VARIABLE_NAME_LENGTH);
		*variables_count += 1;
	}
}

void SetShaderUniformFloat(st_shader *shader, char *name, float value) {

}

void SetShaderUniformInt(st_shader *shader, char *name, int value) {

}

void SetShaderUniformUnsignedInt(st_shader *shader, char *name, unsigned int value) {

}

void SetShaderUniformVec2(st_shader *shader, char *name, float value[2]) {

}

void SetShaderUniformVec3(st_shader *shader, char *name, float value[3]) {

}

void SetShaderUniformVec4(st_shader *shader, char *name, float value[4]) {

}

void SetShaderUniformMatrix3(st_shader *shader, char *name, float value[9]) {

}

void SetShaderUniformMatrix4(st_shader *shader, char *name, float value[16]) {

}

// NOTE(Brett):This function creates a block of memory!!!
void ReadFile(char **fileContents, char *filename) {
	#ifdef DEBUG
	assert(filename);
	#endif

	std::fstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

	if(file.is_open())
	{
		std::streampos size = file.tellg();
		file.seekg(0, std::ios::beg);

		*fileContents = (char *)malloc(sizeof(char) * size+1);

		file.read(*fileContents, size);
		(*fileContents)[size] = '\0';
		file.close();
	}
	else
	{
		std::cerr << "Could not open: " << filename << " for reading" << std::endl;
	}
}


void CompileShader(GLuint *shader, char *shaderSource) {
	#ifdef DEBUG
	assert(shader);
	assert(shaderSource);
	#endif

	glShaderSource(*shader, 1, (GLchar**)&shaderSource, 0);
	glCompileShader(*shader);

	GLint compileStatus;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if(false == compileStatus)
	{
		int logLength = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);

		char *infoLog = (char *)malloc(sizeof(char) * logLength);

		glGetShaderInfoLog(*shader, logLength, &logLength, infoLog);
		std::cerr << "Shader compile error: " << std::endl << infoLog << std::endl;
		free(infoLog);
		return;
	}
	else
	{
		std::cerr << "Shader compiled OK" << std::endl;
		return;
	}

	// NOTE(Brett):code should never get there...
	std::cerr << "------> ERROR probably shouldnt be here..." << std::endl;
	return;
}


void CreateShaderProgram(st_shader *shader, char *vertexShaderSource, char *fragmentShaderSource) {
	#ifdef DEBUG
	assert(shader);
	assert(vertexShaderSource);
	assert(fragmentShaderSource);
	#endif

	GLuint vertexShader   = glCreateShader(GL_VERTEX_SHADER),
		   fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	CompileShader(&vertexShader, vertexShaderSource);
	CompileShader(&fragmentShader, fragmentShaderSource);

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertexShader);
	glAttachShader(shader->id, fragmentShader);

	glLinkProgram(shader->id);

	GLint isLinked;
	glGetProgramiv(shader->id, GL_LINK_STATUS, &isLinked);
	if(false == isLinked)
	{
		int logLength = 0;
		glGetProgramiv(shader->id, GL_INFO_LOG_LENGTH, &logLength);
		char *infoLog = (char *)malloc(sizeof(char) * logLength);

		std::cerr << "Shader linker error: " << std::endl << infoLog << std::endl;
		free(infoLog);

		shader->id = 0;
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}


void CreateTexture(GLuint *textureId, char *texturePath) {
	
}