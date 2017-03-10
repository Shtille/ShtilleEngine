#pragma once
#ifndef __SHT_SYSTEM_ENDIANNESS_H__
#define __SHT_SYSTEM_ENDIANNESS_H__

#include "common/types.h"

namespace sht {
	namespace system {

		bool IsLittleEndian();

		inline u16 SwapU16(u16 value)
		{
			return ((value & 0x00FF) << 8)
				 | ((value & 0xFF00) >> 8);
		}
		inline u32 SwapU32(u32 value)
		{
			return ((value & 0x000000FF) << 24)
				 | ((value & 0x0000FF00) << 8)
				 | ((value & 0x00FF0000) >> 8)
				 | ((value & 0xFF000000) >> 24);
		}

		union U32F32
		{
			u32 as_u32;
			f32 as_f32;
		};
		inline f32 SwapF32(f32 value)
		{
			U32F32 u;
			u.as_f32 = value;
			u.as_u32 = SwapU32(u.as_u32);
			return u.as_f32;
		}

	} // namespace system
} // namespace sht

#endif