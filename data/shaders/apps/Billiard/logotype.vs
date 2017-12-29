#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_projection_view;
uniform mat4 u_model;

out DATA
{
	vec3 position;
	vec3 normal;
} vs_out;

void main()
{
	vec4 position_world = u_model * vec4(a_position, 1.0);

	vs_out.position = vec3(position_world);
	vs_out.normal = mat3(u_model) * a_normal;

	gl_Position = u_projection_view * position_world;
}