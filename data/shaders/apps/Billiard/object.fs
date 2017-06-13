#version 330 core

uniform vec4 u_color;

out vec4 color;

in vec3 v_normal;
in vec3 v_light;

void main()
{
	vec3 normal = normalize(v_normal);
	vec3 light = normalize(v_light);
	color = vec4(max(dot(normal, light), 0.2) * u_color.rgb, u_color.a);
}