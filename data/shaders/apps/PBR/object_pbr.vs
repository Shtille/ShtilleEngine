#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_binormal;

uniform mat4 u_projection_view;
uniform mat4 u_model;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_binormal;
out vec2 v_texcoord;

void main()
{
	vec4 position_world = u_model * vec4(a_position, 1.0);

	v_position = vec3(position_world);
	mat3 model = mat3(u_model);
	v_normal = model * a_normal;
	v_tangent = model * a_tangent;
	v_binormal = model * a_binormal;
	v_texcoord = a_texcoord;

	gl_Position = u_projection_view * position_world;
}