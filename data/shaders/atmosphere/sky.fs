#version 330 core

uniform vec3 v3LightPos;
uniform float g;
uniform float g2;

out vec4 color;

in vec3 v_mie_color;
in vec3 v_rayleigh_color;
in vec3 v_to_camera;

void main (void)
{
	float fCos = dot(v3LightPos, v_to_camera) / length(v_to_camera);
	float fRayleighPhase = 1.0 + fCos * fCos;
	float fMiePhase = (1.0 - g2) / (2.0 + g2) * (1.0 + fCos * fCos) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
	color = vec4(1.0 - exp(-1.5 * (fRayleighPhase * v_rayleigh_color + fMiePhase * v_mie_color)), 1.0);
}

