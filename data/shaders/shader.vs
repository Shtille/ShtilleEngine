#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_normal;

void main()
{
    v_normal = a_normal;
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}