#version 330 core

uniform samplerCube u_texture;

out vec4 out_color;

smooth in vec3 v_eye_direction;

void main()
{
	out_color = textureLod(u_texture, v_eye_direction, 0.0);
}