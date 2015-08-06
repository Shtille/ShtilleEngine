#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat3 u_normal_matrix;
uniform vec3 u_light_pos; // in eye space

out vec3 v_normal;
out vec3 v_light;

void main()
{
    v_normal = u_normal_matrix * a_normal;
    vec4 pos_eye = u_view * u_model * vec4(a_position, 1.0);
    v_light = u_light_pos - pos_eye.xyz;
    gl_Position = u_projection * pos_eye;
}