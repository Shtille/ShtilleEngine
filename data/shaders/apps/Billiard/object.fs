#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emissive;
	float shininess;
	float dissolve;
};

struct Light {
	vec3 position; // in world space
	vec3 color;
};

uniform Material u_material;
uniform Light u_light;
uniform vec3 u_eye_position;

out vec4 out_color;

in vec3 v_position_world;
in vec3 v_normal_world;

const float world_ambient = 0.1;
//const float screen_gamma = 2.2; // Assume that monitor is in sRGB color space

void main()
{
	// Compute emissive term
	vec3 emissive = u_material.emissive;

	// Compute ambient term
	vec3 ambient = world_ambient * u_material.ambient;

	// Compute diffuse term
	vec3 normal = normalize(v_normal_world.xyz);
	vec3 light = normalize(u_light.position - v_position_world);
	float lambertian = max(dot(light, normal), 0.0);
	vec3 diffuse = lambertian * u_light.color * u_material.diffuse;

	// Compute specular term
	vec3 specular = vec3(0.0);
	if (lambertian > 0.0)
	{
		vec3 view = normalize(u_eye_position - v_position_world);
		vec3 halfdir = normalize(light + view);
		float spec_angle = max(dot(halfdir, normal), 0.0);
		specular = pow(spec_angle, u_material.shininess) * u_light.color * u_material.specular;
	}

	vec3 color_linear = emissive + ambient + diffuse + specular;
	//vec3 color_corrected = pow(color_linear, vec3(1.0/screen_gamma));
	vec3 color_corrected = color_linear;

	out_color = vec4(color_corrected, u_material.dissolve);
}