#version 330 core

out vec4 color;

in vec3 v_normal;
in vec3 v_light;

void main()
{
    const vec3 base_color = vec3(1.0, 0.0, 1.0);
    vec3 normal = normalize(v_normal);
    vec3 light = normalize(v_light);
    color.rgb = max(dot(normal, light), 0.2) * base_color;
    color.a = 1.0;
}