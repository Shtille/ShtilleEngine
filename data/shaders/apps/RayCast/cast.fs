#version 330 core

out vec4 out_color;

in DATA
{
	vec2 uv;
} fs_in;

// struct Sphere {
// 	vec3 position;
// 	float radius;
// };
struct Box {
	vec3 min;
	vec3 max;
};

uniform vec3 u_eye;
uniform vec3 u_ray00;
uniform vec3 u_ray10;
uniform vec3 u_ray01;
uniform vec3 u_ray11;

// There will be the only sphere in this shader
//uniform Sphere u_sphere;
#define NUM_BOXES 1
uniform Box u_boxes[NUM_BOXES];

const float kEpsilon = 0.01;

vec2 IntersectBox(vec3 origin, vec3 dir, const Box box)
{
	vec3 tMin = (box.min - origin) / dir;
	vec3 tMax = (box.max - origin) / dir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	return vec2(tNear, tFar);
}
vec3 BoxNormal(const Box box, const vec3 point)
{
	vec3 center = (box.max + box.min) * 0.5;
	vec3 size = (box.max - box.min) * 0.5;
	vec3 pc = point - center;
	// step(edge,x) : x < edge ? 0 : 1
	vec3 normal = vec3(0.0);
	normal += vec3(sign(pc.x), 0.0, 0.0) * step(abs(abs(pc.x) - size.x), kEpsilon);
	normal += vec3(0.0, sign(pc.y), 0.0) * step(abs(abs(pc.y) - size.y), kEpsilon);
	normal += vec3(0.0, 0.0, sign(pc.z)) * step(abs(abs(pc.z) - size.z), kEpsilon);
	return normalize(normal);
}

// // Indices of refraction
// const float Air = 1.0;
// const float Glass = 1.51714;

// // Eta
// const float Eta = Air / Glass;

// const float R0 = ((Air - Glass) * (Air - Glass)) / ((Air + Glass) * (Air + Glass));

void main()
{
	// 1. Get ray from UV
	vec3 ray = mix(mix(u_ray00, u_ray01, fs_in.uv.y), mix(u_ray10, u_ray11, fs_in.uv.y), fs_in.uv.x);
	ray = normalize(ray);
	// 2. Test objects for intersection
	vec2 lambda = IntersectBox(u_eye, ray, u_boxes[0]);
	bool hit = lambda.x > 0.0 && lambda.x < lambda.y;
	if (hit)
	{
		vec3 point = u_eye + ray * lambda.x;
		vec3 normal = BoxNormal(u_boxes[0], point);
		float lambertian = max(dot(normal, vec3(1.0f, 0.0f, 0.0f)), 0.0);
		out_color = vec4(1.0, 1.0, 1.0, 1.0) * lambertian;
	}
	else
		out_color = vec4(0.0, 0.5, 1.0, 1.0);
	// TEST stuff here
	// vec3 refraction = refract(ray, normal, Eta);
	// vec3 reflection = reflect(ray, normal);
	// float fresnel = R0 + (1.0 - R0) * pow(1.0 - dot(-ray, normal), 5.0);

}