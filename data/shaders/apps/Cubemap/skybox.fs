#version 330 core

uniform samplerCube u_texture;
uniform float u_lod;

out vec4 out_color;

smooth in vec3 v_position;

void main()
{
	out_color = textureLod(u_texture, v_position, u_lod);
}