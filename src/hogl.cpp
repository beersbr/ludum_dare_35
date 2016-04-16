#include "hogl.h"

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


void CreateShaderProgram(GLuint *shaderProgram, char *vertexShaderSource, char *fragmentShaderSource) {
	#ifdef DEBUG
	assert(shaderProgram);
	assert(vertexShaderSource);
	assert(fragmentShaderSource);
	#endif

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER),
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	CompileShader(&vertexShader, vertexShaderSource);
	CompileShader(&fragmentShader, fragmentShaderSource);

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);

	glLinkProgram(*shaderProgram);

	GLint isLinked;
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &isLinked);
	if(false == isLinked)
	{
		int logLength = 0;
		glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		char *infoLog = (char *)malloc(sizeof(char) * logLength);

		std::cerr << "Shader linker error: " << std::endl << infoLog << std::endl;
		free(infoLog);

		*shaderProgram = 0;
		return;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}


void CreateTexture(GLuint *textureId, char *texturePath) {
	
}