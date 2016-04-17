#version 330


in vec3 f_color;
in vec3 f_normal;
in vec3 f_light;

out vec4 color;

void main()
{
	float weight = dot(normalize(f_light), normalize(f_normal));
	weight = max(weight, 0.0);

	color = vec4(weight * f_color, 1.0);
}