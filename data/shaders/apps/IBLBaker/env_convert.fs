#version 330 core

uniform samplerCube u_texture;

out vec4 out_color;

smooth in vec3 v_eye_direction;

void main()
{
	vec4 color = texture(u_texture, v_eye_direction);
	out_color = vec4(vec3(1.0) - color.xyz, 1.0);
}