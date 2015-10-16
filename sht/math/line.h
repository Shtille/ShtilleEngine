#pragma once
#ifndef __SHT_MATH_LINE_H__
#define __SHT_MATH_LINE_H__

#include "vector.h"

namespace sht {
	namespace math {

	/** Standard mathematical line definition */
    struct Line {
        vec3 origin;
        vec3 direction;
    };

	} // namespace math
} // namespace sht

#endif