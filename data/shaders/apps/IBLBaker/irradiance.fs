#version 330 core

uniform samplerCube u_texture;

out vec4 out_color;

smooth in vec3 v_eye_direction;

const float PI = 3.14159265359;

void main()
{		
	vec3 N = normalize(v_eye_direction);
	vec3 irradianceAccumulation = vec3(0.0f);
	
	vec3 upDir = vec3(0.0f, 1.0f, 0.0f);
	vec3 rightDir = cross(upDir, N);
	upDir = cross(N, rightDir);

	float sampleOffset = 0.025f;
	float sampleCount = 0.0f;

	for(float anglePhi = 0.0f; anglePhi < 2.0f * PI; anglePhi += sampleOffset)
	{
		for(float angleTheta = 0.0f; angleTheta < 0.5f * PI; angleTheta += sampleOffset)
		{
			vec3 sampleTangent = vec3(sin(angleTheta) * cos(anglePhi),  sin(angleTheta) * sin(anglePhi), cos(angleTheta));
			vec3 sampleVector = sampleTangent.x * rightDir + sampleTangent.y * upDir + sampleTangent.z * N;

			irradianceAccumulation += texture(u_texture, sampleVector).rgb * cos(angleTheta) * sin(angleTheta);
			sampleCount++;
		}
	}

	irradianceAccumulation = irradianceAccumulation * (1.0f / float(sampleCount)) * PI;
	
	out_color = vec4(irradianceAccumulation, 1.0f);
}