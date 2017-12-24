#version 330 core

layout(location = 0) out vec4 out_color;

in DATA
{
	vec3 position;
	mat3 tbn;
	vec2 uv;
	//vec4 shadow_coord;
} fs_in;

struct Camera
{
	vec3 position; //!< world space camera position
};

struct Light
{
	vec3 color;
	vec3 direction;
};

uniform Camera u_camera;
uniform Light u_light;

// PBR Inputs
uniform samplerCube u_diffuse_env_sampler;
uniform samplerCube u_specular_env_sampler;
uniform sampler2D u_preintegrated_fg_sampler;

// PBR Map Inputs
uniform sampler2D u_albedo_sampler;
uniform sampler2D u_normal_sampler;
uniform sampler2D u_roughness_sampler;
uniform sampler2D u_metal_sampler;

#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2

const float prefilterLODLevel = 4.0;
const vec3 materialF0 = vec3(0.04);//vec3(1.0f, 0.72f, 0.29f);

vec3 SrgbToLinear(vec3 srgb_color)
{
	return pow(srgb_color, vec3(GAMMA));
}
vec3 GetNormal()
{
	// Retrieve the tangent space matrix
	mat3 tbn = fs_in.tbn;
	vec3 n = texture(u_normal_sampler, fs_in.uv).rgb;
	n = tbn * (2.0 * n - 1.0);
	return n;
}
float saturate(float f)
{
	return clamp(f, 0.0f, 1.0f);
}


vec2 getSphericalCoord(vec3 normalCoord)
{
	float phi = acos(-normalCoord.y);
	float theta = atan(1.0f * normalCoord.x, -normalCoord.z) + PI;

	return vec2(theta / (2.0f * PI), phi / PI);
}


float Fd90(float NoL, float roughness)
{
	return (2.0f * NoL * roughness) + 0.4f;
}


float KDisneyTerm(float NoL, float NoV, float roughness)
{
	return (1.0f + Fd90(NoL, roughness) * pow(1.0f - NoL, 5.0f)) * (1.0f + Fd90(NoV, roughness) * pow(1.0f - NoV, 5.0f));
}


vec3 computeFresnelSchlick(float NdotV, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);
}


vec3 computeFresnelSchlickRoughness(float NdotV, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - NdotV, 5.0f);
}


float computeDistributionGGX(vec3 N, vec3 H, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;

	float NdotH = saturate(dot(N, H));
	float NdotH2 = NdotH * NdotH;

	return (alpha2) / (PI * (NdotH2 * (alpha2 - 1.0f) + 1.0f) * (NdotH2 * (alpha2 - 1.0f) + 1.0f));
}


float computeGeometryAttenuationGGXSmith(float NdotL, float NdotV, float roughness)
{
	float NdotL2 = NdotL * NdotL;
	float NdotV2 = NdotV * NdotV;
	float kRough2 = roughness * roughness + 0.0001f;

	float ggxL = (2.0f * NdotL) / (NdotL + sqrt(NdotL2 + kRough2 * (1.0f - NdotL2)));
	float ggxV = (2.0f * NdotV) / (NdotV + sqrt(NdotV2 + kRough2 * (1.0f - NdotV2)));

	return ggxL * ggxV;
}

void main()
{
	vec3 albedo = SrgbToLinear(texture(u_albedo_sampler, fs_in.uv).rgb);
	vec3 normal = GetNormal();
	float roughness = texture(u_roughness_sampler, fs_in.uv).r;
	float metalness = texture(u_metal_sampler, fs_in.uv).r;

	vec3 color = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);

	vec3 V = normalize(u_camera.position - fs_in.position);
	vec3 N = normalize(normal);
	vec3 R = reflect(-V, N);

	float NdotV = max(dot(N, V), 0.0001f);

	// Fresnel (Schlick) computation (F term)
	vec3 F0 = mix(materialF0, albedo, metalness);
	vec3 F = computeFresnelSchlick(NdotV, F0);

	// Energy conservation
	vec3 kS = F;
	vec3 kD = vec3(1.0f) - kS;
	kD *= 1.0f - metalness;

	// Directional lights
	//---
	vec3 L = normalize(- u_light.direction); // vector from surface point to light source
	vec3 H = normalize(L + V);

	vec3 lightColor = SrgbToLinear(u_light.color.rgb);

	// Light source dependent BRDF term(s)
	float NdotL = saturate(dot(N, L));

	// Diffuse component computation
	diffuse = albedo / PI;

	// Distribution (GGX) computation (D term)
	float D = computeDistributionGGX(N, H, roughness);

	// Geometry attenuation (GGX-Smith) computation (G term)
	float G = computeGeometryAttenuationGGXSmith(NdotL, NdotV, roughness);

	// Specular component computation
	specular = (F * D * G) / (4.0f * NdotL * NdotV + 0.0001f);

	color += (diffuse * kD + specular) * lightColor * NdotL;
	//---

	F = computeFresnelSchlickRoughness(NdotV, F0, roughness);

	kS = F;
	kD = vec3(1.0f) - kS;
	kD *= 1.0f - metalness;

	// Diffuse irradiance computation
	vec3 diffuseIrradiance = texture(u_diffuse_env_sampler, N).rgb;
	diffuseIrradiance *= albedo.rgb;

	// Specular radiance computation
	vec3 specularRadiance = textureLod(u_specular_env_sampler, R, roughness * prefilterLODLevel).rgb;
	vec2 brdfSampling = texture(u_preintegrated_fg_sampler, vec2(NdotV, roughness)).rg;
	specularRadiance *= F * brdfSampling.x + brdfSampling.y;

	vec3 ambientIBL = (diffuseIrradiance * kD) + specularRadiance;

	color += ambientIBL;

	out_color = vec4(pow(color, vec3(1.0/GAMMA)), 1.0);
}