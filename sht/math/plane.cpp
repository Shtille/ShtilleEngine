#include "plane.h"
#include "line.h"
#include "segment.h"
#include "vertical_profile.h"

#include <cmath>

namespace sht {
	namespace math {

		static float MatrixDeterminant(const vec3& c1, const vec3& c2, const vec3& c3)
		{
			return (c1.x * c2.y * c3.z + c1.z * c2.x * c3.y + c1.y * c2.z * c3.x)
				 - (c1.z * c2.y * c3.x + c1.y * c2.x * c3.z + c1.x * c2.z * c3.y);
		}
		bool PlanesIntersection(const Plane& plane1, const Plane& plane2, const Plane& plane3, vec3& p)
		{
			float det = MatrixDeterminant(plane1.normal, plane2.normal, plane3.normal);
			if (fabs(det) < 0.001f) // parallel planes
				return false;
			p = ((plane2.normal ^ plane3.normal)*-plane1.offset +
				 (plane3.normal ^ plane1.normal)*-plane2.offset + 
				 (plane1.normal ^ plane2.normal)*-plane3.offset ) / det ;
			return true;
		}
		Plane::Plane()
		{
		}
		Plane::Plane(float x, float y, float z, float w)
        {
            normal = vec3(x,y,z);
            float invLen = 1.0f / normal.Length();
            normal *= invLen;
            offset = w * invLen;
        }
		float Plane::Distance(const vec3& point) const
		{
			return (normal & point) + offset;
		}
		bool Plane::IntersectsLine(const vec3& a, const vec3& b, vec3& r) const
		{
			vec3 ba = b - a;
			float n_dot_ba = normal & ba;
			if (fabs(n_dot_ba) < 0.001f)
				return false; // line is parallel to plane
			float n_dot_a = normal & a;
			r = a + (((-offset - n_dot_a)/n_dot_ba) * ba);
			return true;
		}
		bool Plane::IntersectsLine(const Line& line, vec3& r) const
		{
			float n_dot_ba = normal & line.direction;
			if (fabs(n_dot_ba) < 0.001f)
				return false; // line is parallel to plane
			float n_dot_a = normal & line.origin;
			r = line.origin + (((-offset - n_dot_a)/n_dot_ba) * line.direction);
			return true;
		}
		bool Plane::IntersectsSegment(const vec3& a, const vec3& b, vec3& r) const
		{
			vec3 ba = b - a;
			float n_dot_ba = normal & ba;
			if (n_dot_ba == 0.0f)
				return false; // line is parallel to plane
			float n_dot_a = normal & a;
			float t = (-offset - n_dot_a) / n_dot_ba; // factor for result point
			if (t < 0.0f || t > 1.0f)
				return false; // segment doesn't intersect plane
			r = a + (t * ba);
			return true;
		}
		bool Plane::IntersectsPlane(const Plane& plane, Line& line) const
		{
			const vec3& n1 = normal;
			const vec3& n2 = plane.normal;

			line.direction = n1 ^ n2;
			float len = line.direction.Length();
			if (len < 0.00001f) // parallel planes
				return false;
			line.direction /= len;

			const float h1 = -offset;
			const float h2 = -plane.offset;
			float dot = n1 & n2;
			float den = 1.0f - dot*dot;
			float c1 = (h1 - h2*dot)/den;
			float c2 = (h2 - h1*dot)/den;
			line.origin = c1 * n1 + c2 * n2;

			return true;
		}
		bool Plane::IntersectsProfile(const VerticalProfile& profile, Segment& segment) const
		{
			// Find vector parallel to the line of intersection
			vec3 v = normal ^ profile.plane.normal;
			float len = v.Length();
			if (len < 0.00001f) // parallel planes
				return false;
			v /= len;
			if (fabs(v.y) < 0.001f) // line will be parallel to horizontal planes
				return false;

			float dot = normal & profile.plane.normal;
			float den = 1.0f - dot*dot;
			float h1 = -offset;
			float h2 = -profile.plane.offset;
			float c1 = (h1 - h2*dot)/den;
			float c2 = (h2 - h1*dot)/den;
			vec3 p = c1 * normal + c2 * profile.plane.normal;

			// Our intersection line has equation: L = p + t*v
			// Find its intersection with planes y = hmin and y = hmax
			segment.begin = p + (((profile.hmin - p.y)/v.y) * v);
			segment.end   = p + (((profile.hmax - p.y)/v.y) * v);
			return true;
		}
		void Plane::CropSegment(vec3& a, vec3& b) const
		{
			vec3 ba = b - a;
			float n_dot_ba = normal & ba;
			if (n_dot_ba == 0.0f)
				return; // line is parallel to plane
			float n_dot_a = normal & a;
			float t = (-offset - n_dot_a) / n_dot_ba; // factor for result point
			if (t < 0.0f || t > 1.0f)
				return; // segment doesn't intersect plane
			vec3 r = a + (t * ba);
			if (n_dot_ba > 0.0f)
				a = r;
			else
				b = r;
		}

	} // namespace math
} // namespace sht