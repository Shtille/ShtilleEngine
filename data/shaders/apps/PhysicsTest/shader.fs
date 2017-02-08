#version 330 core

uniform sampler2D u_texture;

out vec4 color;

in vec3 v_normal;
in vec3 v_light;
in vec2 v_texcoord;

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 light = normalize(v_light);
    color.rgb = max(dot(normal, light), 0.2) * texture(u_texture, v_texcoord).rgb;
    color.a = 1.0;
}