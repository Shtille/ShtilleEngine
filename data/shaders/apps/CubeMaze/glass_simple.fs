#version 330 core

layout(location = 0) out vec4 out_color;

in DATA
{
	vec3 reflection;
	vec3 refraction;
	float fresnel;
} fs_in;

// PBR Inputs
uniform samplerCube u_specular_env_sampler;

void main()
{
	float lod = 0.0;
	vec3 reflected_color = textureLod(u_specular_env_sampler, fs_in.reflection, lod).rgb;
	vec3 refracted_color = textureLod(u_specular_env_sampler, fs_in.refraction, lod).rgb;
	out_color = vec4(mix(refracted_color, reflected_color, fs_in.fresnel), 1.0);
}