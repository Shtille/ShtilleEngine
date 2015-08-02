#version 330 core

out vec4 color;

in vec3 v_normal;

void main()
{
    const vec3 light = vec3(1.0f, 0.0f, 0.0f);
    const vec3 base_color = vec3(1.0, 1.0, 0.5);
    vec3 normal = normalize(v_normal);
    color.xyz = max(dot(normal, light), 0.2) * base_color;
    color.a = 1.0;
}