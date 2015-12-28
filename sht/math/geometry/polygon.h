#pragma once
#ifndef __SHT_MATH_GEOMETRY_POLYGON_H__
#define __SHT_MATH_GEOMETRY_POLYGON_H__

#include "point.h"

#include <vector>

namespace sht {
namespace math {
namespace geometry {

	class Polygon {
    public:
		std::vector<Point> points;

		int Winding() const;
		void Append(const Point& point);
		void Swap(Polygon& other);

		/*! This works only if all of the following are true:
		 1. poly has no colinear edges;
		 2. poly has no duplicate vertices;
		 3. poly has at least three vertices;
		 4. poly is convex (implying 3).
		*/
		void Clip(const Polygon& frame, Polygon* out);
        
    private:
        void EdgeClip(const Point& x0, const Point& x1, int left, Polygon * res);
	};

} // namespace geometry
} // namespace math
} // namespace sht

#endif