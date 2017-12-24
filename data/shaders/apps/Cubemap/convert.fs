#version 330 core

uniform sampler2D u_texture;
uniform float u_lod;

out vec4 out_color;

smooth in vec3 v_position;

const float PI = 3.14159265359;

vec2 CubeToUV(vec3 cube_vector)
{
	float angle_x = (atan(cube_vector.z, cube_vector.x) * 0.5 / PI) + 0.5; // [-Pi; Pi] -> [0; 1]
	float angle_y = (asin(cube_vector.y) / PI) + 0.5; // [-Pi/2; Pi/2] -> [0; 1]
	return vec2(angle_x, angle_y);
}
vec3 ComputeColor()
{
	vec3 N = normalize(v_position);
	vec3 up = abs(N.z) < 0.999f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 right = cross(up, N);
	up = cross(N, right);

	vec3 accumulation = vec3(0.0);
	const float sample_offset = 0.025f;
	float sample_count = 0.0f;
	float cube_size = 512.0 / pow(2.0, u_lod);
	float theta_max = atan(2.0 / cube_size);

	for (float phi = 0.0f; phi < 2.0f * PI; phi += sample_offset)
	{
		for (float theta = 0.0f; theta < theta_max; theta += sample_offset)
		{
			vec3 sample_tangent = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			vec3 sample_vector = sample_tangent.x * right + sample_tangent.y * up + sample_tangent.z * N;
			vec2 uv = CubeToUV(sample_vector);

			accumulation += texture(u_texture, uv).rgb * cos(theta);
			sample_count++;
		}
	}
	accumulation = accumulation * (1.0f / float(sample_count));
	return accumulation;
}

void main()
{
	vec3 cube_vector = normalize(v_position);
	vec2 uv = CubeToUV(cube_vector);
	out_color = textureLod(u_texture, uv, u_lod);

	// vec3 color = ComputeColor();
	// out_color = vec4(color, 1.0);
}