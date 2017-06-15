#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_projection_view;
uniform mat4 u_model;

out vec3 v_position_world;
out vec3 v_normal_world;

void main()
{
	vec4 position_world = u_model * vec4(a_position, 1.0);
	v_position_world = vec3(position_world);
	v_normal_world = vec3(u_model * vec4(a_normal, 0.0));
    gl_Position = u_projection_view * position_world;
}