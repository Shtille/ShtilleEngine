#pragma once
#ifndef __SHT_MATH_GEOMETRY_SEGMENT_H__
#define __SHT_MATH_GEOMETRY_SEGMENT_H__

#include "point.h"

namespace sht {
namespace math {
namespace geometry {

	struct Segment {
		Point begin;
		Point end;
	};

} // namespace geometry
} // namespace math
} // namespace sht

#endif