#include "vertical_profile.h"
#include "segment.h"

namespace sht {
	namespace math {

		VerticalProfile::VerticalProfile(const vec3& a, const vec3& b, float hmin, float hmax)
	        : hmin(hmin), hmax(hmax)
		{
			vec2 ba(b.x - a.x, b.z - a.z);
			ba.Normalize();
			plane.normal = vec3(-ba.y, 0.0f, ba.x);
			plane.offset = -(plane.normal & a);
		}
		vec3 VerticalProfile::GetAnyPoint()
		{
			vec3 p;
			p.y = hmin;
			if (plane.normal.x != 0.0f) // use x
			{
				p.x = 0.0f;
				p.z = -plane.offset/plane.normal.z;
			}
			else // use z
			{
				p.z = 0.0f;
				p.x = -plane.offset/plane.normal.x;
			}
			return p;
		}
		bool VerticalProfile::InRange(const Segment& segment) const
		{
			return (segment.begin.y > hmin && segment.begin.y < hmax) ||
				   (segment.end.y   > hmin && segment.end.y   < hmax) ||
				   (segment.begin.y < hmin && segment.end.y   > hmax) ||
				   (segment.end.y   < hmin && segment.begin.y > hmax);
		}

	} // namespace math
} // namespace sht