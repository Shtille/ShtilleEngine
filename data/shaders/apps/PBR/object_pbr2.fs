#version 330 core

layout(location = 0) out vec4 out_color;

in DATA
{
	vec3 position;
	vec3 normal;
	vec2 uv;
	vec3 binormal;
	vec3 tangent;
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
	vec3 lightVector;
	float intensity;
};

struct Material
{
	vec4 albedo;
	vec3 specular;
	float roughness;
};

struct Attributes
{
	vec3 normal;
};

uniform Camera u_camera;
uniform Light u_light;

// PBR Inputs
uniform sampler2D u_preintegrated_fg_sampler;
uniform samplerCube u_environment_sampler;

// PBR Map Inputs
uniform sampler2D u_albedo_sampler;
uniform sampler2D u_normal_sampler;
uniform sampler2D u_roughness_sampler;
//uniform sampler2D u_metal_sampler;

//uniform sampler2DShadow u_shadow_map;

// PBR Static Inputs
uniform vec4 u_albedo_color;
uniform vec3 u_specular_color;
uniform float u_roughness_color;
//uniform float u_metalness_color;

// PBR Modes
uniform float u_using_albedo_map;
uniform bool u_using_normal_map;
uniform float u_using_roughness_map;
//uniform float u_using_metal_map;

#define PI 3.1415926535897932384626433832795
#define GAMMA 2.2

/*
// Used for shadows
vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);
*/

Attributes g_attributes;

vec4 GammaCorrectTexture(sampler2D tex, vec2 uv)
{
	vec4 samp = texture(tex, uv);
	return vec4(pow(samp.rgb, vec3(GAMMA)), samp.a);
}

vec3 GammaCorrectTextureRGB(sampler2D tex, vec2 uv)
{
	vec4 samp = texture(tex, uv);
	return vec3(pow(samp.rgb, vec3(GAMMA)));
}

vec4 GetAlbedo()
{
	return (1.0 - u_using_albedo_map) * u_albedo_color + u_using_albedo_map * GammaCorrectTexture(u_albedo_sampler, fs_in.uv);
}

vec3 GetSpecular()
{
	//return u_specular_color;
	//return (1.0 - u_using_specular_map) * u_specular_color + u_using_specular_map * GammaCorrectTextureRGB(u_SpecularMap, fs_in.uv);
	return (1.0 - u_using_albedo_map) * u_specular_color + u_using_albedo_map * GammaCorrectTextureRGB(u_albedo_sampler, fs_in.uv);
}

float GetRoughness()
{
	return (1.0 - u_using_roughness_map) * u_roughness_color + u_using_roughness_map * GammaCorrectTextureRGB(u_roughness_sampler, fs_in.uv).r;
}

// float GetMetalness()
// {
// 	return (1.0 - u_using_metal_map) * u_metalness_color + u_using_metal_map * GammaCorrectTextureRGB(u_metal_sampler, fs_in.uv).r;
// }

vec3 FinalGamma(vec3 color)
{
	return pow(color, vec3(1.0 / GAMMA));
}

float FresnelSchlick(float f0, float fd90, float view)
{
	return f0 + (fd90 - f0) * pow(max(1.0 - view, 0.1), 5.0);
}

float Disney(Light light, Material material, vec3 eye)
{
	vec3 halfVector = normalize(light.lightVector + eye);

	float NdotL = max(dot(g_attributes.normal, light.lightVector), 0.0);
	float LdotH = max(dot(light.lightVector, halfVector), 0.0);
	float NdotV = max(dot(g_attributes.normal, eye), 0.0);

	float energyBias = mix(0.0, 0.5, material.roughness);
	float energyFactor = mix(1.0, 1.0 / 1.51, material.roughness);
	float fd90 = energyBias + 2.0 * (LdotH * LdotH) * material.roughness;
	float f0 = 1.0;

	float lightScatter = FresnelSchlick(f0, fd90, NdotL);
	float viewScatter = FresnelSchlick(f0, fd90, NdotV);

	return lightScatter * viewScatter * energyFactor;
}

vec3 GGX(Light light, Material material, vec3 eye)
{
	vec3 h = normalize(light.lightVector + eye);
	float NdotH = max(dot(g_attributes.normal, h), 0.0);

	float rough2 = max(material.roughness * material.roughness, 2.0e-3); // capped so spec highlights don't disappear
	float rough4 = rough2 * rough2;

	float d = (NdotH * rough4 - NdotH) * NdotH + 1.0;
	float D = rough4 / (PI * (d * d));

	// Fresnel
	vec3 reflectivity = material.specular;
	float fresnel = 1.0;
	float NdotL = clamp(dot(g_attributes.normal, light.lightVector), 0.0, 1.0);
	float LdotH = clamp(dot(light.lightVector, h), 0.0, 1.0);
	float NdotV = clamp(dot(g_attributes.normal, eye), 0.0, 1.0);
	vec3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);

	// geometric / visibility
	float k = rough2 * 0.5;
	float G_SmithL = NdotL * (1.0 - k) + k;
	float G_SmithV = NdotV * (1.0 - k) + k;
	float G = 0.25 / (G_SmithL * G_SmithV);

	return G * D * F;
}

vec3 RadianceIBLIntegration(float NdotV, float roughness, vec3 specular)
{
	vec2 preintegratedFG = texture(u_preintegrated_fg_sampler, vec2(NdotV, 1.0 - roughness)).rg;
	return specular * preintegratedFG.r + preintegratedFG.g;
}

vec3 IBL(Light light, Material material, vec3 eye)
{
	float NdotV = max(dot(g_attributes.normal, eye), 0.0);

	vec3 reflectionVector = normalize(reflect(eye, g_attributes.normal));
	float smoothness = 1.0 - material.roughness;
	float mipLevel = (1.0 - smoothness * smoothness) * 10.0;
	vec4 cs = textureLod(u_environment_sampler, reflectionVector, mipLevel);
	vec3 result = pow(cs.xyz, vec3(GAMMA)) * RadianceIBLIntegration(NdotV, material.roughness, material.specular);

	vec3 diffuseDominantDirection = g_attributes.normal;
	float diffuseLowMip = 9.6;
	vec3 diffuseImageLighting = textureLod(u_environment_sampler, diffuseDominantDirection, diffuseLowMip).rgb;
	diffuseImageLighting = pow(diffuseImageLighting, vec3(GAMMA));

	return result + diffuseImageLighting * material.albedo.rgb;
}

float Diffuse(Light light, Material material, vec3 eye)
{
	return Disney(light, material, eye);
}

vec3 Specular(Light light, Material material, vec3 eye)
{
	return GGX(light, material, eye);
}

// float random(vec3 seed, int i)
// {
// 	vec4 seed4 = vec4(seed, i);
// 	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
// 	return fract(sin(dot_product) * 43758.5453);
// }

vec3 NormalMap()
{
	// Normalize inputs
	vec3 tangent = normalize(fs_in.tangent);
	vec3 binormal = normalize(fs_in.binormal);
	vec3 normal = normalize(fs_in.normal);

	mat3 to_world = mat3(tangent, binormal, normal);
	vec3 normalMap;
	if (u_using_normal_map)
		normalMap = texture(u_normal_sampler, fs_in.uv).rgb * 2.0 - 1.0;
	else
		normalMap = vec3(0.0, 0.0, 1.0);
	normalMap = to_world * normalMap;
	normalMap = normalize(normalMap);
	return normalMap;
}

void main()
{
	g_attributes.normal = NormalMap();

	vec3 eye = normalize(u_camera.position - fs_in.position);

	Light light = u_light;
	light.lightVector = light.direction;

	Material material;
	material.albedo = GetAlbedo();
	material.specular = GetSpecular();
	material.roughness = GetRoughness();

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	// TODO: Multiple lights
	for (int i = 0; i < 1; i++)
	{
		float NdotL = clamp(dot(g_attributes.normal, light.lightVector), 0.0, 1.0);

		// Diffuse Calculation
		diffuse += NdotL * Diffuse(light, material, eye) * light.color * light.intensity;
		// Specular Calculation
		specular += NdotL * Specular(light, material, eye) * light.color * light.intensity;
	}
	// Shadows
	//float bias = 0.005;
	//float visibility = 1.0;
	/*
	for (int i = 0; i < 1; i++)
	{
		int index = int(16.0 * random(floor(fs_in.position * 1000.0), i)) % 16;
		visibility -= (1.0 / 4.0) * (1.0 - texture(u_shadow_map, vec3(fs_in.shadow_coord.xy + poissonDisk[index] / 700.0, (fs_in.shadow_coord.z - bias) / fs_in.shadow_coord.w)));
	}
	*/

	vec3 finalColor = material.albedo.rgb * diffuse + (specular + IBL(light, material, eye));
	//finalColor *= visibility;
	finalColor = FinalGamma(finalColor);
	out_color = vec4(finalColor, material.albedo.a);
}