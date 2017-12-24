#version 330 core

uniform sampler2D u_texture;
uniform float u_roughness;

out vec4 out_color;

smooth in vec3 v_position;

const float PI = 3.14159265359f;
const uint NUM_SAMPLES = 1024u;

float saturate(float f)
{
	return clamp(f, 0.0f, 1.0f);
}
float RadicalInverse_VdC(uint bits) // In place of bitfieldreverse()
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

	return float(bits) * 2.3283064365386963e-10;
}
vec2 ComputeHammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
vec3 ComputeImportanceSampleGGX(vec2 Xi, float roughness, vec3 N)
{
	float alpha = roughness * roughness;

	float phi = 2 * PI * Xi.x;
	float cos_theta = sqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
	float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

	vec3 H;
	H.x = sin_theta * cos(phi);
	H.y = sin_theta * sin(phi);
	H.z = cos_theta;

	vec3 up = abs(N.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tan_x = normalize(cross(up, N));
	vec3 tan_y = cross(N, tan_x);

	return normalize(tan_x * H.x + tan_y * H.y + N * H.z);
}
vec2 CubeToUV(vec3 cube_vector)
{
	float angle_x = (atan(cube_vector.z, cube_vector.x) * 0.5 / PI) + 0.5; // [-Pi; Pi] -> [0; 1]
	float angle_y = (asin(cube_vector.y) / PI) + 0.5; // [-Pi/2; Pi/2] -> [0; 1]
	return vec2(angle_x, angle_y);
}

void main()
{
	vec3 N = normalize(v_position);

	// Unreal Engine 4 "Real Shading" approximation
	vec3 R = N;
	vec3 V = R;

	vec3 prefiltered_accumulation = vec3(0.0f);
	float total_sample_weight = 0.0f;

	for (uint i = 0u; i < NUM_SAMPLES; ++i)
	{
		// Hammersley + Importance Sampling so that we biased the sample vector direction
		vec2 Xi = ComputeHammersley(i, NUM_SAMPLES);
		vec3 H = ComputeImportanceSampleGGX(Xi, u_roughness, N);
		vec3 L = normalize(2.0f * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0f);

		if (NdotL > 0.0f)
		{
			vec2 uv = CubeToUV(L);
			prefiltered_accumulation += textureLod(u_texture, uv, 0.0f).rgb * NdotL;
			total_sample_weight += NdotL;
		}
	}

	prefiltered_accumulation = prefiltered_accumulation / total_sample_weight;

	out_color = vec4(prefiltered_accumulation, 1.0f);
}