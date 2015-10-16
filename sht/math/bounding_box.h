#pragma once
#ifndef __SHT_MATH_BOUNDING_BOX_H__
#define __SHT_MATH_BOUNDING_BOX_H__

#include "vector.h"

namespace sht {
	namespace math {

	/** Bounding box definition */
    struct BoundingBox {
        vec3 center;
        vec3 extent;
    };

	} // namespace math
} // namespace sht

#endif