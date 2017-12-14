#version 330 core

#extension GL_EXT_shader_texture_lod: enable
#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform vec3 u_LightDirection;
uniform vec3 u_LightColor;

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

float saturate(in float value)
{
	return clamp(value, 0.0, 1.0);
}

// Phong (Lambertian) diffuse term
float Phong_Diffuse()
{
	return (1.0 / PI);
}

// compute fresnel specular factor for given base specular and product
// product could be NdV or VdH depending on used technique
vec3 Fresnel_Factor(in vec3 f0, in float product)
{
	return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
}

float D_BlinnPhong(in float roughness, in float NdH)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float d = (NdH * NdH) * (a2 - 1.0) + 1.0;
	return a2 / PI * (d * d);
}
float G_Schlick(in float roughness, in float NdV, in float NdL)
{
	float k = roughness * roughness * 0.5;
	float V = NdV * (1.0 - k) + k;
	float L = NdL * (1.0 - k) + k;
	return 0.25 / (V * L);
}
// cook-torrance specular calculation                      
vec3 CookTorrance_Specular(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
{
	float D = D_BlinnPhong(roughness, NdH);
	float G = G_Schlick(roughness, NdV, NdL);
	float rim = 1.0;//mix(1.0 - roughness * material.w * 0.9, 1.0, NdV);
	return (1.0 / rim) * specular * G * D;
}

float3 ImportanceSampleGGX( float2 Xi, float Roughness, float3 N )
{
	float a = Roughness * Roughness;

	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	float3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
	float3 TangentX = normalize( cross( UpVector, N ) );
	float3 TangentY = cross( N, TangentX );

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 SpecularIBL( float3 SpecularColor, float Roughness, float3 N, float3 V )
{
	float3 SpecularLighting = 0;
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 Xi = Hammersley( i, NumSamples );
		float3 H = ImportanceSampleGGX( Xi, Roughness, N );
		float3 L = 2 * dot( V, H ) * H - V;

		float NoV = saturate( dot( N, V ) );
		float NoL = saturate( dot( N, L ) );
		float NoH = saturate( dot( N, H ) );
		float VoH = saturate( dot( V, H ) );

		if ( NoL > 0 )
		{
			float3 SampleColor = EnvMap.SampleLevel( EnvMapSampler, L, 0 ).rgb;
			float G = G_Smith( Roughness, NoV, NoL );
			float Fc = pow( 1 - VoH, 5 );
			float3 F = (1 - Fc) * SpecularColor + Fc;
			// Incident light = SampleColor * NoL
			// Microfacet specular = D*G*F / (4*NoL*NoV)
			// pdf = D * NoH / (4 * VoH)
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}
	return SpecularLighting / NumSamples;
}

float3 PrefilterEnvMap( float Roughness, float3 R )
{
	float3 N = R; float3 V = R;
	float3 PrefilteredColor = 0;
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 Xi = Hammersley( i, NumSamples );
		float3 H = ImportanceSampleGGX( Xi, Roughness, N );
		float3 L = 2 * dot( V, H ) * H - V;

		float NoL = saturate( dot( N, L ) );
		if( NoL > 0 )
		{
			PrefilteredColor += EnvMap.SampleLevel( EnvMapSampler, L, 0 ).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

float2 IntegrateBRDF( float Roughness, float NoV )
{
	float3 V;
	V.x = sqrt( 1.0f - NoV * NoV ); // sin V.y = 0;
	V.z = NoV; // cos

	float A = 0;
	float B = 0;
	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		float2 Xi = Hammersley( i, NumSamples );
		float3 H = ImportanceSampleGGX( Xi, Roughness, N );
		float3 L = 2 * dot( V, H ) * H - V;

		float NoL = saturate( L.z );
		float NoH = saturate( H.z );
		float VoH = saturate( dot( V, H ) );
		if( NoL > 0 )
		{
			float G = G_Smith( Roughness, NoV, NoL );
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow( 1 - VoH, 5 );
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2( A, B ) / NumSamples;
}

float3 ApproximateSpecularIBL( float3 SpecularColor, float Roughness, float3 N, float3 V )
{
	float NoV = saturate( dot( N, V ) );
	float3 R = 2 * dot( V, N ) * N - V;

	float3 PrefilteredColor = PrefilterEnvMap( Roughness, R );
	float2 EnvBRDF = IntegrateBRDF( Roughness, NoV );

	return PrefilteredColor * ( SpecularColor * EnvBRDF.x + EnvBRDF.y );
}

vec3 GetNormal()
{
	return normalize(v_normal);
}

void main()
{
	vec3 N = GetNormal();
	vec3 V = normalize(u_camera_position - v_position);	// Vector from surface point to camera
    vec3 L = normalize(u_light_direction);				// Vector from surface point to light
    vec3 H = normalize(L + V);							// Half vector between both l and v

	float NdotL = clamp(dot(N, L), 0.001, 1.0);
    float NdotV = abs(dot(N, V)) + 0.001;
    float NdotH = clamp(dot(N, H), 0.0, 1.0);
    float LdotH = clamp(dot(L, H), 0.0, 1.0);
    float VdotH = clamp(dot(V, H), 0.0, 1.0);

    // albedo/specular base
    vec3 base = texture(u_albedo_sampler, v_texcoord).xyz;
    float roughness = texture(u_roughness_sampler, v_texcoord).x;
    float metallic = texture(u_metal_sampler, v_texcoord).x;

    // mix between metal and non-metal material, for non-metal
	// constant base specular factor of 0.04 grey is used
	vec3 specular = mix(vec3(0.04), base, metallic);

	vec3 specfresnel = Fresnel_Factor(specular, HdV);
    vec3 specref = CookTorrance_Specular(NdL, NdV, NdH, specfresnel, roughness);
    specref *= vec3(NdL);

    // diffuse is common for any model
    vec3 diffref = (vec3(1.0) - specfresnel) * Phong_Diffuse() * NdL;

    
    // compute lighting
    vec3 reflected_light = vec3(0.0);
    vec3 diffuse_light = vec3(0.0); // initial value == constant ambient light

    // point light
    float A = 1.0;
    vec3 light_color = vec3(1.0) * A;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // IBL lighting
    vec2 brdf = texture2D(iblbrdf, vec2(roughness, 1.0 - NdV)).xy;
    vec3 iblspec = min(vec3(0.99), fresnel_factor(specular, NdV) * brdf.x + brdf.y);
    reflected_light += iblspec * envspec;
    diffuse_light += envdiff * (1.0 / PI);

    reflected_light += ApproximateSpecularIBL(Fresnel_Factor(specular, NdV), roughness, N, V);

    // final result
    vec3 result =
        diffuse_light * mix(base, vec3(0.0), metallic) +
        reflected_light;

    out_color = vec4(result, 1);



	// specular reflectance with COOK-TORRANCE
	vec3 F = Fresnel_Factor(specular, HdV);
	float G = G_Schlick(roughness, NdV, NdL);
	float D = D_BlinnPhong(roughness, NdH);

	// Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    vec3 color = NdotL * u_LightColor * (diffuseContrib + specContrib);

    color += IBLContribution();

	// diffuse is common for any model
	vec3 diffref = (vec3(1.0) - specfresnel) * Phong_Diffuse() * NdL;


    vec4 baseColor = SRGBtoLINEAR(texture2D(u_BaseColorSampler, v_UV)) * u_BaseColorFactor;

    // Calculate the shading terms for the microfacet specular shading model
    vec3 F = specularReflection(pbrInputs);
    float G = geometricOcclusion(pbrInputs);
    float D = microfacetDistribution(pbrInputs);

    // Calculation of analytical lighting contribution
    vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
    vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
    vec3 color = NdotL * u_LightColor * (diffuseContrib + specContrib);

	out_color = diffuse;
}