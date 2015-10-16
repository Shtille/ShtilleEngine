#pragma once
#ifndef __SHT_MATH_SEGMENT_H__
#define __SHT_MATH_SEGMENT_H__

#include "vector.h"

namespace sht {
	namespace math {

	/** Standard mathematical segment definition */
    struct Segment {
        Segment();
        Segment(const vec3& begin, const vec3& end);

        vec3 begin;
        vec3 end;
    };

	} // namespace math
} // namespace sht

#endif