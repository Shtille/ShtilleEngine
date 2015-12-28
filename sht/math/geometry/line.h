#pragma once
#ifndef __SHT_MATH_GEOMETRY_LINE_H__
#define __SHT_MATH_GEOMETRY_LINE_H__

#include "point.h"

namespace sht {
namespace math {
namespace geometry {

	struct Line {
		Point origin;
		Vector direction;
	};
	
	struct Ray {
		Point origin;
		Vector direction;
	};

} // namespace geometry
} // namespace math
} // namespace sht

#endif