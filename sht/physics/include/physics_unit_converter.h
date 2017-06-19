#pragma once
#ifndef __SHT_PHYSICS_UNIT_CONVERTER_H__
#define __SHT_PHYSICS_UNIT_CONVERTER_H__

#include "math/vector.h"
#include "math/matrix.h"

namespace sht {
	namespace physics {

		class UnitConverter {
		public:
			// Like game to library units conversion
			virtual void LinearScaleToStandard(float * scale) const = 0;
			// Like library to game units conversion
			virtual void LinearScaleFromStandard(float * scale) const = 0;
		};

	} // namespace physics
} // namespace sht

#endif