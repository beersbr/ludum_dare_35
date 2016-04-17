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

void main()
{
	vec3 lightPosition = vec3((model * vec4(position, 1.0)).xyz) - light;
	vec3 normalPosition = vec3((model * vec4(normal, 1.0)).xyz);

	float weight = dot(normalize(lightPosition), normalize(normal));
	weight = max(weight, 0.0);
	f_color =  weight * color;

	gl_Position = projection * view * model * vec4(position, 1.0);
}