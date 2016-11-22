#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_KEY_H__
#define __SHT_SATELLITE_IMAGERY_KEY_H__

#include "raw_key.h"
#include "packed_key.h"

namespace sht {
	namespace satellite_imagery {

		// Choose our main key type
		typedef PackedKey DataKey;

	} // namespace satellite_imagery
} // namespace sht

#endif