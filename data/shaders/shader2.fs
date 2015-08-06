#version 330 core

out vec4 color;

void main()
{
    const vec3 base_color = vec3(1.0, 1.0, 0.5);
    color.xyz = base_color;
    color.a = 1.0;
}