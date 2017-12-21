#version 330 core

uniform samplerCube u_texture;
uniform float u_roughness;
uniform float u_cube_resolution_width;
uniform float u_cube_resolution_height;

out vec4 out_color;

smooth in vec3 v_eye_direction;

const float PI = 3.14159265359f;
const uint numSamples = 1024u;

float saturate(float f);
float radicalInverse_VdC(uint bits);
vec3 computeImportanceSampleGGX(vec2 Xi, float roughness, vec3 N);
vec2 computeHammersley(uint i, uint N);
float computeDistributionGGX(vec3 N, vec3 H, float roughness);


void main()
{
	vec3 N = normalize(v_eye_direction);

	// Unreal Engine 4 "Real Shading" approximation
	vec3 R = N;
	vec3 V = R;

	vec3 prefilteredAccumulation = vec3(0.0f);
	float totalSampleWeight = 0.0f;

	for(uint i = 0u; i < numSamples; ++i)
	{
		// Hammersley + Importance Sampling so that we biased the sample vector direction
		vec2 Xi = computeHammersley(i, numSamples);
		vec3 H = computeImportanceSampleGGX(Xi, u_roughness, N);
		vec3 L = normalize(2.0f * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0f);

		if(NdotL > 0.0f)
		{
			float D = computeDistributionGGX(N, H, u_roughness);
			float NdotH = max(dot(N, H), 0.0f);
			float HdotV = max(dot(H, V), 0.0f);
			float probaDistribFunction = D * NdotH / (4.0f * HdotV) + 0.0001f;

			// Trick by Chetan Jags in order to avoid dots artifacting  by sampling a mip level of the envMap
			// instead of directly sampling the envMap
			float saTexel  = 4.0f * PI / (6.0f * u_cube_resolution_width * u_cube_resolution_height);
			float saSample = 1.0f / (float(numSamples) * probaDistribFunction + 0.0001f);
			float mipLevel = u_roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);

			prefilteredAccumulation += textureLod(u_texture, L, mipLevel).rgb * NdotL;
			totalSampleWeight += NdotL;
		}
	}

	prefilteredAccumulation = prefilteredAccumulation / totalSampleWeight;

	out_color = vec4(prefilteredAccumulation, 1.0f);
}



float saturate(float f)
{
	return clamp(f, 0.0f, 1.0f);
}


float radicalInverse_VdC(uint bits) // In place of bitfieldreverse()
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

	return float(bits) * 2.3283064365386963e-10;
}


vec2 computeHammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), radicalInverse_VdC(i));
}


vec3 computeImportanceSampleGGX(vec2 Xi, float roughness, vec3 N)
{
	float alpha = roughness * roughness;

	float anglePhi = 2 * PI * Xi.x;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	vec3 H;
	H.x = sinTheta * cos(anglePhi);
	H.y = sinTheta * sin(anglePhi);
	H.z = cosTheta;

	vec3 upDir = abs(N.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tanX = normalize(cross(upDir, N));
	vec3 tanY = cross(N, tanX);

	return normalize(tanX * H.x + tanY * H.y + N * H.z);
}


float computeDistributionGGX(vec3 N, vec3 H, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;

	float NdotH = saturate(dot(N, H));
	float NdotH2 = NdotH * NdotH;

	return (alpha2) / (PI * (NdotH2 * (alpha2 - 1.0f) + 1.0f) * (NdotH2 * (alpha2 - 1.0f) + 1.0f));
}