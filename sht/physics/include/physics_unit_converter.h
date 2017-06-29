#pragma once
#ifndef __SHT_PHYSICS_UNIT_CONVERTER_H__
#define __SHT_PHYSICS_UNIT_CONVERTER_H__

#include "math/vector.h"
#include "math/matrix.h"

namespace sht {
	namespace physics {

		typedef void (* ConversionFunction)(float *);

		struct UnitConversion {
			ConversionFunction linear_to;
			ConversionFunction linear_from;
			ConversionFunction mass_to;
			ConversionFunction mass_from;
		};

	} // namespace physics
} // namespace sht

#endif