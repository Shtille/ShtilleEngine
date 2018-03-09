#version 330 compatibility

#ifdef GL_core_profile
out vec4 out_color;
#endif

in DATA
{
	vec2 uv;
} fs_in;

struct Material {
    vec3 color;	// diffuse color
    float f0;	// specular color (monochrome)
    int type;
};
struct Ray {
    vec3 origin;
    vec3 direction;
};
struct Hit {
	vec3 normal;
    float t;	// solution to p=o+t*d
    bool exists;
    Material material;
};
struct Plane {
	vec3 normal; // (a,b,c)
    float d;	// solution to dot(n,p)+d=0
};
struct Sphere {
	vec3 position;
	float radius;
};
struct Box {
	vec3 min;
	vec3 max;
};
struct DirectionalLight {
	vec3 color;
    vec3 direction;
};
struct TracedRay {
    Ray incident;
    vec3 attenuation;
    int depth;
};

// Defines
#define PI 3.14159265358979
#define MAX_DEPTH 2
#define MAX_TRACED_RAYS 16 // 2 ^ (MAX_DEPTH + 2) - 1 <= MAX_TRACED_RAYS
#define GAMMA 2.2

#define MAX_PLANES 1
#define MAX_SPHERES 1
#define MAX_BOXES 1

uniform vec3 u_eye;
uniform vec3 u_ray00;
uniform vec3 u_ray10;
uniform vec3 u_ray01;
uniform vec3 u_ray11;

uniform DirectionalLight u_light;

uniform Plane u_planes[MAX_PLANES];
uniform Sphere u_spheres[MAX_SPHERES];
uniform Box u_boxes[MAX_BOXES];
uniform int u_num_planes;
uniform int u_num_spheres;
uniform int u_num_boxes;

const float kEpsilon = 0.01;
const Material kPlaneMaterial = Material(vec3(0.5, 0.4, 0.3), 0.02, 0);
const Material kSphereMaterial = Material(vec3(0.5), 0.02, 1);
const Hit kNoHit = Hit(vec3(0.0), 1e5, false, Material(vec3(0.0), 0.0, 0));

// Indices of refraction
const float n_air = 1.0;
const float n_glass = 1.51714;
// Fresnel reflectance at normal incidence
const float R0 = ((n_air - n_glass) * (n_air - n_glass)) / ((n_air + n_glass) * (n_air + n_glass));

float pow5(float x)
{
	float x2 = x * x;
	return x2 * x2 * x;
}
// Schlick approximation
float fresnel(vec3 h, vec3 v, float f0)
{
    return pow5(1.0 - clamp(dot(h, v), 0.0, 1.0)) * (1.0 - f0) + f0;
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
Hit IntersectPlane(const Plane plane, const Ray ray, const Material material)
{
    float dotnd = dot(plane.normal, ray.direction);
    if (dotnd > 0.0) return kNoHit;

    float t = -(dot(ray.origin, plane.normal) + plane.d) / dotnd;
    return Hit(plane.normal, t, true, material);
}
Hit IntersectSphere(const Sphere sphere, const Ray ray, const Material material)
{
	vec3 op = sphere.position - ray.origin;
    float b = dot(op, ray.direction);
    float det = b * b - dot(op, op) + sphere.radius * sphere.radius;
    if (det < 0.) return kNoHit;

    det = sqrt(det);
    float t = b - det;
    if (t < 0.) t = b + det;
    if (t < 0.) return kNoHit;

    vec3 hit_normal = (ray.origin + t * ray.direction - sphere.position) / sphere.radius;
    return Hit(hit_normal, t, true, material);
}
Hit IntersectBox(const Box box, const Ray ray, const Material material)
{
	vec3 tMin = (box.min - ray.origin) / ray.direction;
	vec3 tMax = (box.max - ray.origin) / ray.direction;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);
	if (tNear < tFar)
	{
        float t = (tNear > 0.0) ? tNear : tFar;
        vec3 hit_pos = ray.origin + ray.direction * t;
		vec3 hit_normal = BoxNormal(box, hit_pos);
		return Hit(hit_normal, t, true, material);
	}
	else
		return kNoHit;
}

Hit IntersectScene(Ray ray)
{
	Hit hit = kNoHit;
	// Intersection with planes
	for (int i = 0; i < u_num_planes; ++i)
	{
		Hit cur_hit = IntersectPlane(u_planes[i], ray, kPlaneMaterial);
		if (cur_hit.exists && cur_hit.t < hit.t)
			hit = cur_hit;
	}
	// Intersection with spheres
	for (int i = 0; i < u_num_spheres; ++i)
	{
		Hit cur_hit = IntersectSphere(u_spheres[i], ray, kSphereMaterial);
		if (cur_hit.exists && cur_hit.t < hit.t)
			hit = cur_hit;
	}
	// Intersection with boxes
	for (int i = 0; i < u_num_boxes; ++i)
	{
		Hit cur_hit = IntersectBox(u_boxes[i], ray, kSphereMaterial);
		if (cur_hit.exists && cur_hit.t < hit.t)
			hit = cur_hit;
	}
	return hit;
}
vec3 SkyColor(vec3 d)
{
    float transition = pow(smoothstep(0.02, 0.5, d.y), 0.4);

    vec3 sky = 2e2*mix(vec3(0.52, 0.77, 1), vec3(0.12, 0.43, 1), transition);
    vec3 sun = u_light.color * pow(abs(dot(d, u_light.direction)), 5000.);
    return sky + sun;
}
vec3 AccountForDirectionalLight(vec3 position, vec3 normal)
{
    if (IntersectScene(Ray(position + kEpsilon * u_light.direction, u_light.direction)).exists == false)
    {
        return clamp(dot(normal, u_light.direction), 0.0, 1.0) * u_light.color;
    }
	return vec3(0.0);
}
vec3 Radiance(Ray ray)
{
    vec3 accumulation = vec3(0.0);
    vec3 attenuation = vec3(1.0);

    TracedRay traced_rays[MAX_TRACED_RAYS];
    int current_ray = 0;
    int used_rays = 1;
    traced_rays[0].incident = ray;
    traced_rays[0].attenuation = attenuation;
    traced_rays[0].depth = 0;

    int depth = 0;
    while (current_ray < used_rays)
    {
        // Extract ray
        ray = traced_rays[current_ray].incident;
        attenuation = traced_rays[current_ray].attenuation;
        depth = traced_rays[current_ray].depth;
        ++current_ray;
        if (depth > MAX_DEPTH)
            break;

        Hit hit = IntersectScene(ray);

        if (hit.exists)
        {
            if (hit.material.type == 0)
            {
                float f = fresnel(hit.normal, -ray.direction, hit.material.f0);

                vec3 hitPos = ray.origin + hit.t * ray.direction;

                // Diffuse
                vec3 incoming = vec3(0.0);
                incoming += AccountForDirectionalLight(hitPos, hit.normal);

                accumulation += (1.0 - f) * attenuation * hit.material.color * incoming;

                // Specular: next bounce
                if (used_rays < MAX_TRACED_RAYS - 1)
                {
                    vec3 d = reflect(ray.direction, hit.normal);
                    traced_rays[used_rays].incident = Ray(hitPos + kEpsilon * d, d);
                    traced_rays[used_rays].attenuation = attenuation * f;
                    traced_rays[used_rays].depth = depth + 1;
                    ++used_rays;
                }
            }
            else
            {
                float a = dot(hit.normal, ray.direction), ddn = abs(a);
                float nnt = mix(n_air / n_glass, n_glass / n_air, float(a>0.0));
                float cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
                if (cos2t > 0.0)
                {
                    vec3 tdir = normalize(ray.direction * nnt + sign(a) * hit.normal * (ddn * nnt + sqrt(cos2t)));
                    float c = 1.0 - mix(ddn, dot(tdir, hit.normal), float(a>0.0));
                    float Re = R0 + (1.0 - R0) * pow5(c);

                    // Store rays value
                    if (used_rays < MAX_TRACED_RAYS - 2)
                    {
                        vec3 d = reflect(ray.direction, hit.normal);
                        traced_rays[used_rays].incident = Ray(ray.origin + hit.t * ray.direction + kEpsilon * d, d);
                        traced_rays[used_rays].attenuation = attenuation * Re;
                        traced_rays[used_rays].depth = depth + 1;
                        ++used_rays;
                        traced_rays[used_rays].incident = Ray(ray.origin + (hit.t + kEpsilon) * ray.direction, tdir);
                        traced_rays[used_rays].attenuation = attenuation * (1.0 - Re);
                        traced_rays[used_rays].depth = depth + 1;
                        ++used_rays;
                    }
                }
                else
                {
                    // Total internal reflection
                    if (used_rays < MAX_TRACED_RAYS - 1)
                    {
                        vec3 d = reflect(ray.direction, hit.normal);
                        traced_rays[used_rays].incident = Ray(ray.origin + hit.t * ray.direction + kEpsilon * d, d);
                        traced_rays[used_rays].attenuation = attenuation;
                        traced_rays[used_rays].depth = depth + 1;
                        ++used_rays;
                    }
                }
            }
        }
        else
        {
            accumulation += attenuation * SkyColor(ray.direction);
        }
        if (current_ray == MAX_TRACED_RAYS)
        {
            // We shouldn't pass this condition. Added it to avoid memory corruption.
            return vec3(1e5);
        }
    }
    return accumulation;
}
vec3 Uncharted2ToneMapping(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 0.012;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1. / GAMMA));
	return color;
}

void main()
{
	// 1. Get ray from UV
	vec3 direction = mix(mix(u_ray00, u_ray01, fs_in.uv.y), mix(u_ray10, u_ray11, fs_in.uv.y), fs_in.uv.x);
	direction = normalize(direction);

	// 2. Test objects for intersection
	Ray ray = Ray(u_eye, direction);
	vec3 color = Radiance(ray);
#ifdef GL_core_profile
	out_color
#else
    gl_FragColor
#endif
        = vec4(Uncharted2ToneMapping(color), 1.0);
}