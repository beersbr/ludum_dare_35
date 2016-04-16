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


void ReadFile(char **fileContents, char *filename);
void CompileShader(GLuint *shader, char *shaderSource);
void CreateShaderProgram(GLuint *shaderProgram, char *vertexShader, char *fragmentShader);
void CreateTexture(GLuint *textureId, char *texturePath);

#endif