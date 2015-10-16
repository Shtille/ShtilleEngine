#include "segment.h"

namespace sht {
	namespace math {

		Segment::Segment()
		{
		}
		Segment::Segment(const vec3& begin, const vec3& end)
			: begin(begin), end(end)
		{
		}

	} // namespace math
} // namespace sht