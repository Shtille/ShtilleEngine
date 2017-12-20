#version 330 core

uniform sampler2D u_texture;

out vec4 out_color;

in vec2 v_texcoord;

void main()
{
	vec4 color = texture(u_texture, v_texcoord);
	float intensity = dot(color.xyz, vec3(0.7, 0.5, 0.3));
	out_color = vec4(intensity, intensity, intensity, 1.0);
}