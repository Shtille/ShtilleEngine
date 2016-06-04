#pragma once
#ifndef __SHT_GEO_CONSTANTS_H__
#define __SHT_GEO_CONSTANTS_H__

#include "../../math/vector.h"

namespace sht {
	namespace geo {

		const float kEarthRadius = 6371000.0f;
        const math::Vector3 kEarthPosition = math::Vector3(0.0f, 0.0f, 0.0f);

	}
}

#endif