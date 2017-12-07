#version 330 core

uniform sampler2D u_texture;

out vec4 out_color;

in vec3 v_position_world;
in vec3 v_normal_world;
in vec2 v_texcoord;

void main()
{
	vec4 diffuse = texture(u_texture, v_texcoord);

	out_color = diffuse;
}