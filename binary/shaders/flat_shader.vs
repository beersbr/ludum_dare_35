#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 st;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 light;

out vec3 f_color;
out vec3 f_normal;
out vec3 f_light;

void main()
{
	f_light = vec3((model * vec4(position, 1.0)).xyz) - light;
	f_normal = (transpose(inverse(mat4(model))) * vec4(normal, 0.0)).xyz;
	f_color = color;
	gl_Position = projection * view * model * vec4(position, 1.0);
}