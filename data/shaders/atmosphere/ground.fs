#version 330 core

uniform sampler2D u_earth_texture;
uniform sampler2D u_clouds_texture;

out vec4 color;

in vec3 v_color;
in vec3 v_attenuate;
in vec2 v_texcoord;

void main(void)
{
	vec3 earth = texture(u_earth_texture, v_texcoord).rgb;
	vec3 clouds = texture(u_clouds_texture, v_texcoord).rrr; // 1 channel texture
	color = vec4(v_color + mix(earth, clouds, dot(vec3(0.33), clouds)) * v_attenuate, 1.0);
}