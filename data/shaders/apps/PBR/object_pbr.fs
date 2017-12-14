#version 330 core

uniform vec3 u_camera_position; // world space camera position
uniform vec3 u_light_direction; // world space light direction
//uniform vec3 u_light_color;

uniform samplerCube u_env_sampler;
uniform sampler2D u_albedo_sampler;
//uniform sampler2D u_normal_sampler;
uniform sampler2D u_roughness_sampler;
uniform sampler2D u_metal_sampler;

out vec4 out_color;

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

const float PI = 3.141592653589793;

float ComputeFresnelTerm(float zero, vec3 V, vec3 N)
{
	float base_value = 1.0 - dot(V, N);
	float exponential = pow(base_value, 5.0) ;
	float fresnel = exponential + zero * (1.0 - exponential) ;

	return fresnel ;
}

float ChiGGX(float f)
{
	return f > 0.0 ? 1.0 : 0.0 ;
}

float ComputeGGXDistribution(vec3 N, vec3 L, float fRoughness)
{
	float fNormalDotLight = clamp(dot(N, L), 0.0, 1.0) ;
	float fNormalDotLightSquared = fNormalDotLight * fNormalDotLight ;
	float fRoughnessSquared = fRoughness * fRoughness ;
	float fDen = fNormalDotLightSquared * fRoughnessSquared + (1 - fNormalDotLightSquared);

	return clamp((ChiGGX(fNormalDotLight) * fRoughnessSquared) / (PI * fDen * fDen), 0.0, 1.0);
}

float ComputeGGXPartialGeometryTerm(vec3 V, vec3 N, vec3 H, float fRoughness)
{
	float fViewerDotLightViewHalf = clamp(dot(V, H), 0.0, 1.0) ;
	float fChi = ChiGGX(fViewerDotLightViewHalf / clamp(dot(V, N), 0.0, 1.0));
	fViewerDotLightViewHalf *= fViewerDotLightViewHalf;
	float fTan2 = (1.0 - fViewerDotLightViewHalf) / fViewerDotLightViewHalf;

	return (fChi * 2.0) / (1.0 + sqrt(1.0 + fRoughness * fRoughness * fTan2)) ;
}

void main()
{
	vec3 N = normalize(v_normal);
	vec3 V = normalize(u_camera_position - v_position);	// Vector from surface point to camera
	vec3 L = normalize(u_light_direction);				// Vector from surface point to light
	vec3 H = normalize(L + V);							// Half vector between both l and v
	vec3 reflection = normalize(reflect(V, N));

	float fLightIntensity = max(dot(L, N), 0.0) ;

	float fMetalness = texture(u_metal_sampler, v_texcoord).r ;
	float fRoughness = max(0.001, texture(u_roughness_sampler, v_texcoord).r ) ;

	float distributionMicroFacet = ComputeGGXDistribution(N, L, fRoughness) ;
	float geometryMicroFacet = ComputeGGXPartialGeometryTerm(V, N, H, fRoughness) ;
	float microFacetContribution = distributionMicroFacet * geometryMicroFacet ;

	float fLightSourceFresnelTerm = ComputeFresnelTerm(0.5, V, N) ;

	vec3 rgbAlbedo = texture(u_albedo_sampler, v_texcoord).rgb;

	vec3 rgbFragment = rgbAlbedo * (1.0 - fMetalness);

	vec3 rgbSourceReflection = textureLod(u_env_sampler, reflection, 9.0 * fRoughness).rgb ;
	vec3 rgbReflection = rgbSourceReflection ;
	rgbReflection *= rgbAlbedo * fMetalness ;
	rgbReflection *= fLightSourceFresnelTerm ;
	rgbReflection = min(rgbReflection, rgbSourceReflection) ; // conservation of energy

	vec3 rgbSpecular = vec3(0.0) ;

	if (fLightIntensity > 0.0)
	{
		rgbSpecular = vec3(1.0) ;
		rgbSpecular *= microFacetContribution * fLightSourceFresnelTerm ;
		rgbSpecular = min(vec3(1.0), rgbSpecular) ; // conservation of energy
	}
	rgbFragment += rgbSpecular ;
	rgbFragment *= fLightIntensity ;
	rgbFragment += rgbReflection ;

	out_color = vec4(rgbFragment, 1.0);
}