#version 330 core

layout(location = 0) out vec4 out_color;

in DATA
{
	vec3 position;
	vec3 normal;
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
uniform vec4 u_albedo_color;
uniform float u_roughness;
uniform float u_metalness;

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
	float NdotL;                  // cos angle between normal and light direction
	float NdotV;                  // cos angle between normal and view direction
	float NdotH;                  // cos angle between normal and half vector
	float LdotH;                  // cos angle between light direction and half vector
	float VdotH;                  // cos angle between view direction and half vector
	float perceptual_roughness;    // roughness value, as authored by the model creator (input to shader)
	float metalness;              // metallic value at the surface
	vec3 reflectance0;            // full reflectance color (normal incidence angle)
	vec3 reflectance90;           // reflectance color at grazing angle
	float alpha_roughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 diffuse_color;            // color contribution from diffuse lighting
	vec3 specular_color;           // color contribution from specular lighting
};

const float PI = 3.141592653589793;
#define GAMMA 2.2

// Indices of refraction
const float Air = 1.0;
const float Glass = 1.51714;

// Air to glass ratio of the indices of refraction (Eta)
const float Eta = Air / Glass;
 
// see http://en.wikipedia.org/wiki/Refractive_index Reflectivity
const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));

vec4 SrgbToLinear(vec4 srgb_in)
{
	vec3 linear_out = pow(srgb_in.xyz, vec3(GAMMA));
	return vec4(linear_out, srgb_in.w);
}

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 GetIBLContribution(PBRInfo pbr_inputs, vec3 n, vec3 reflection)
{
	float mipCount = 4.0; // resolution of 512x512
	float lod = (pbr_inputs.perceptual_roughness * mipCount);
	// retrieve a scale and bias to F0. See [1], Figure 3
	vec3 brdf = SrgbToLinear(texture(u_preintegrated_fg_sampler, vec2(pbr_inputs.NdotV, 1.0 - pbr_inputs.perceptual_roughness))).rgb;
	vec3 diffuseLight = SrgbToLinear(texture(u_diffuse_env_sampler, n)).rgb;

	vec3 specularLight = SrgbToLinear(textureLod(u_specular_env_sampler, reflection, lod)).rgb;

	vec3 diffuse = diffuseLight * pbr_inputs.diffuse_color;
	vec3 specular = specularLight * (pbr_inputs.specular_color * brdf.x + brdf.y);

	return diffuse + specular;
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 Diffuse(PBRInfo pbr_inputs)
{
	return pbr_inputs.diffuse_color / PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 SpecularReflection(PBRInfo pbr_inputs)
{
	return pbr_inputs.reflectance0 + (pbr_inputs.reflectance90 - pbr_inputs.reflectance0) * pow(clamp(1.0 - pbr_inputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alpha_roughness as input as originally proposed in [2].
float GeometricOcclusion(PBRInfo pbr_inputs)
{
	float NdotL = pbr_inputs.NdotL;
	float NdotV = pbr_inputs.NdotV;
	float r = pbr_inputs.alpha_roughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float MicrofacetDistribution(PBRInfo pbr_inputs)
{
	float roughnessSq = pbr_inputs.alpha_roughness * pbr_inputs.alpha_roughness;
	float f = (pbr_inputs.NdotH * roughnessSq - pbr_inputs.NdotH) * pbr_inputs.NdotH + 1.0;
	return roughnessSq / (PI * f * f);
}

void main()
{
	// Roughness
	float perceptual_roughness = u_roughness;
	perceptual_roughness = clamp(perceptual_roughness, 0.04, 1.0);

	// Metallic
	float metallic = u_metalness;
	metallic = clamp(metallic, 0.0, 1.0);

	// Roughness is authored as perceptual roughness; as is convention,
	// convert to material roughness by squaring the perceptual roughness [2].
	float alpha_roughness = perceptual_roughness * perceptual_roughness;

	// The albedo may be defined from a base texture or a flat color
	vec4 base_color = SrgbToLinear(u_albedo_color);

	vec3 f0 = vec3(0.04);
	vec3 diffuse_color = base_color.rgb * (vec3(1.0) - f0);
	diffuse_color *= 1.0 - metallic;
	vec3 specular_color = mix(f0, base_color.rgb, metallic);

	// Compute reflectance.
	float reflectance = max(max(specular_color.r, specular_color.g), specular_color.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specular_color.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	vec3 n = fs_in.normal;									// normal at surface point
	vec3 v = normalize(u_camera.position - fs_in.position); // vector from surface point to camera
	vec3 l = normalize(u_light.direction);					// vector from surface point to light
	vec3 h = normalize(l + v);								// half vector between both l and v
	vec3 reflection = normalize(reflect(-v, n));
	vec3 refraction = normalize(refract(-v, n, Eta));

	float NdotL = clamp(dot(n, l), 0.001, 1.0);
	float NdotV = abs(dot(n, v)) + 0.001;
	float NdotH = clamp(dot(n, h), 0.0, 1.0);
	float LdotH = clamp(dot(l, h), 0.0, 1.0);
	float VdotH = clamp(dot(v, h), 0.0, 1.0);

	PBRInfo pbr_inputs = PBRInfo(
		NdotL,
		NdotV,
		NdotH,
		LdotH,
		VdotH,
		perceptual_roughness,
		metallic,
		specularEnvironmentR0,
		specularEnvironmentR90,
		alpha_roughness,
		diffuse_color,
		specular_color
	);

	// Calculate the shading terms for the microfacet specular shading model
	vec3 F = SpecularReflection(pbr_inputs);
	float G = GeometricOcclusion(pbr_inputs);
	float D = MicrofacetDistribution(pbr_inputs);

	// Calculation of analytical lighting contribution
	vec3 diffuse_contrib = (1.0 - F) * Diffuse(pbr_inputs);
	vec3 spec_contrib = F * G * D / (4.0 * NdotL * NdotV);
	vec3 color = NdotL * u_light.color * (diffuse_contrib + spec_contrib);

	// Calculate lighting contribution from image based lighting source (IBL)
	vec3 reflected_color = GetIBLContribution(pbr_inputs, n, reflection);
	vec3 refracted_color = GetIBLContribution(pbr_inputs, n, refraction);
	float fresnel = R0 + (1.0 - R0) * pow((1.0 - NdotV), 5.0);
	color += mix(refracted_color, reflected_color, fresnel);

	out_color = vec4(pow(color, vec3(1.0/GAMMA)), base_color.a);
}