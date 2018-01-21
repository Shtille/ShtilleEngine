#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_projection_view;
uniform mat4 u_model;

struct Camera
{
	vec3 position; //!< world space camera position
};

uniform Camera u_camera;

out DATA
{
	vec3 reflection;
	vec3 refraction;
	float fresnel;
} vs_out;

// Indices of refraction
const float Air = 1.0;
const float Glass = 1.51714;

// Air to glass ratio of the indices of refraction (Eta)
const float Eta = Air / Glass;
 
// see http://en.wikipedia.org/wiki/Refractive_index Reflectivity
const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));

void main()
{
	vec4 position_world = u_model * vec4(a_position, 1.0);
	mat3 model = mat3(u_model);
	vec3 normal = model * a_normal;
	vec3 incident = normalize(position_world.xyz - u_camera.position);

	vs_out.reflection = reflect(incident, normal);
	vs_out.refraction = refract(incident, normal, Eta);
	vs_out.fresnel = R0 + (1.0 - R0) * pow((1.0 - dot(-incident, normal)), 5.0);

	gl_Position = u_projection_view * position_world;
}