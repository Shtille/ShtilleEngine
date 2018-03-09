#version 330 core

out vec4 out_color;

in DATA
{
	vec2 uv;
} fs_in;

// Defines
#define PI 3.14159265358979
#define MAX_BOUNCES 5
#define NUM_BOXES 1
#define GAMMA 2.2

// uniform vec3 u_eye;
// uniform vec3 u_ray00;
// uniform vec3 u_ray10;
// uniform vec3 u_ray01;
// uniform vec3 u_ray11;
uniform vec2 iResolution;

struct Material
{
    vec3 c;		// diffuse color
    float f0;	// specular color (monochrome)
    int type;
};

struct Ray
{
    vec3 o;		// origin
    vec3 d;		// direction
};

struct Hit
{
    float t;	// solution to p=o+t*d
    vec3 n;		// normal
    Material m;	// material
};
const Hit noHit = Hit(1e10, vec3(0.), Material(vec3(-1.), -1., -1));

struct Plane
{
	vec3 normal; // (a,b,c)
    float d;	// solution to dot(n,p)+d=0
    Material material;
};

struct Sphere
{
	vec3 position;
	float radius;
    Material material;
};

struct Box
{
	vec3 min;
	vec3 max;
};

struct DirectionalLight
{
    vec3 d;		// Direction
    vec3 c;		// Color
};

float hash(float x)
{
	return fract(sin(x) * 43758.5453);
}
float pow5(float x)
{
	float x2 = x * x;
	return x2 * x2 * x;
}

float seed = 0.;
float rand() { return fract(sin(seed++)*43758.5453123); }

// Schlick approximation
float fresnel(vec3 h, vec3 v, float f0)
{
    return pow5(1. - clamp(dot(h, v), 0., 1.)) * (1. - f0) + f0;
}

Hit intersectPlane(Plane p, Ray r)
{
    float dotnd = dot(p.normal, r.d);
    if (dotnd > 0.) return noHit;

    float t = -(dot(r.o, p.normal) + p.d) / dotnd;
    return Hit(t, p.normal, p.material);
}

Hit intersectSphere(Sphere s, Ray r)
{
	vec3 op = s.position - r.o;
    float b = dot(op, r.d);
    float det = b * b - dot(op, op) + s.radius * s.radius;
    if (det < 0.) return noHit;

    det = sqrt(det);
    float t = b - det;
    if (t < 0.) t = b + det;
    if (t < 0.) return noHit;

    return Hit(t, (r.o + t*r.d - s.position) / s.radius, s.material);
}

Hit intersectScene(Ray r)
{
    Sphere s = Sphere(vec3(1., 1., 0.), 1., Material(vec3(0.5), 0.02, 1));
    Plane p  = Plane(vec3(0., 1., 0.), 0., Material(vec3(0.5, 0.4, 0.3), 0.02, 0));

    Hit hit = Hit(1e5, vec3(0.), Material(vec3(-1.), -1., -1));
    Hit hitp = intersectPlane(p, r);  if (hitp.m.f0 >= 0. && hitp.t < hit.t) { hit = hitp; }
    Hit hits = intersectSphere(s, r); if (hits.m.f0 >= 0. && hits.t < hit.t) { hit = hits; }
    return hit;
}

DirectionalLight sunLight = DirectionalLight(normalize(vec3(1., .5, .5)), vec3(1e3));
vec3 skyColor(vec3 d)
{
    float transition = pow(smoothstep(0.02, .5, d.y), 0.4);

    vec3 sky = 2e2*mix(vec3(0.52, 0.77, 1), vec3(0.12, 0.43, 1), transition);
    vec3 sun = sunLight.c * pow(abs(dot(d, sunLight.d)), 5000.);
    return sky + sun;
}

float epsilon = 4e-4;

vec3 accountForDirectionalLight(vec3 p, vec3 n, DirectionalLight l)
{
    if (intersectScene(Ray(p + epsilon * l.d, l.d)).m.f0 < 0.)
    {
        return clamp(dot(n, l.d), 0., 1.) * l.c;
    }
	return vec3(0.);
}

vec3 radiance(Ray r)
{
    vec3 accum = vec3(0.);
    vec3 attenuation = vec3(1.);

    for (int i = 0; i < MAX_BOUNCES; ++i)
    {
        Hit hit = intersectScene(r);

        if (hit.m.f0 >= 0.)
        {
            if (hit.m.type == 0)
            {
                float f = fresnel(hit.n, -r.d, hit.m.f0);

                vec3 hitPos = r.o + hit.t * r.d;

                // Diffuse
                vec3 incoming = vec3(0.);
                incoming += accountForDirectionalLight(hitPos, hit.n, sunLight);

                accum += (1. - f) * attenuation * hit.m.c * incoming;

                // Specular: next bounce
                attenuation *= f;
                vec3 d = reflect(r.d, hit.n);
                r = Ray(hitPos + epsilon * d, d);
            }
            else
            {
                float a=dot(hit.n,r.d), ddn=abs(a);
                float nc=1., nt=1.5, nnt=mix(nc/nt, nt/nc, float(a>0.));
                float cos2t=1.-nnt*nnt*(1.-ddn*ddn);
                if (cos2t > 0.)
                {
                    vec3 tdir = normalize(r.d*nnt + sign(a)*hit.n*(ddn*nnt+sqrt(cos2t)));
                    float R0=(nt-nc)*(nt-nc)/((nt+nc)*(nt+nc)),
                        c = 1.-mix(ddn,dot(tdir, hit.n),float(a>0.));
                    float Re=R0+(1.-R0)*c*c*c*c*c,P=.25+.5*Re,RP=Re/P,TP=(1.-Re)/(1.-P);
                    if (rand() < P)
                    {
                        attenuation *= RP;
                        vec3 d = reflect(r.d, hit.n);
                        r = Ray(r.o + hit.t * r.d + epsilon * d, d);
                    }
                    else
                    {
                        attenuation *= hit.m.c * TP;
                        r = Ray(r.o + hit.t * r.d + epsilon * r.d, tdir);
                    }
                }
            }
        }
        else
        {
            accum += attenuation * skyColor(r.d);
            break;
        }
    }
    return accum;
}

// See: http://filmicgames.com/archives/75
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
	vec2 uv = 2. * fs_in.uv - 1.;

    float o1 = 0.25;
    float o2 = 0.75;
    vec2 msaa[4];
    msaa[0] = vec2( o1,  o2);
    msaa[1] = vec2( o2, -o1);
    msaa[2] = vec2(-o1, -o2);
    msaa[3] = vec2(-o2,  o1);

    vec3 color = vec3(0.);
    for (int i = 0; i < 4; ++i)
    {
        vec3 p0 = vec3(0., 1.1, 4.);
        vec3 p = p0;
        vec3 offset = vec3(msaa[i] / iResolution.y, 0.);
        vec3 d = normalize(vec3(iResolution.x/iResolution.y * uv.x, uv.y, -1.5) + offset);
        Ray r = Ray(p, d);
        color += radiance(r) / 4.;
    }

	out_color = vec4(Uncharted2ToneMapping(color), 1.0);
}