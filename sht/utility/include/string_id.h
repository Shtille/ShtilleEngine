#pragma once
#ifndef __SHT_SYSTEM_STRING_ID_H__
#define __SHT_SYSTEM_STRING_ID_H__

#include <cstring>
#include <cstdint>

namespace sht {
	namespace utility {

		typedef uint32_t StringId;

		// Universal simple string hashing function
		uint32_t SimpleHash(const char* buffer);

		// Generate CRC lookup table
		template <uint32_t C, int K = 8>
		struct CrcHelper : CrcHelper<((C & 1) ? 0xedb88320 : 0) ^ (C >> 1), K - 1> {};
		template <uint32_t C>
		struct CrcHelper<C, 0> { enum {value = C}; };

		#define CRC_A(x) CRC_B(x) CRC_B(x + 128)
		#define CRC_B(x) CRC_C(x) CRC_C(x + 64)
		#define CRC_C(x) CRC_D(x) CRC_D(x + 32)
		#define CRC_D(x) CRC_E(x) CRC_E(x + 16)
		#define CRC_E(x) CRC_F(x) CRC_F(x + 8)
		#define CRC_F(x) CRC_G(x) CRC_G(x + 4)
		#define CRC_G(x) CRC_H(x) CRC_H(x + 2)
		#define CRC_H(x) CRC_I(x) CRC_I(x + 1)
		#define CRC_I(x) CrcHelper<x>::value ,

		static constexpr uint32_t crc_table[] = { CRC_A(0) };

		// Constexpr implementation
		constexpr uint32_t crc32_impl(const uint8_t* p, size_t len, uint32_t crc)
		{
			return len ? crc32_impl(p+1, len-1, (crc << 8) ^ crc_table[((crc >> 24) ^ *p) & 0xFF]) : crc;
		}
		constexpr uint32_t crc32(const uint8_t* data, size_t length)
		{
			return crc32_impl(data, length, 0U);
		}
		constexpr size_t strlen_c(const char* str)
		{
			return *str ? 1 + strlen_c(str + 1) : 0;
		}

		//! Function for runtime CRC32 sum generation
		uint32_t crc32_runtime(const char* str);
	
	} // namespace utility
} // namespace sht

//! Function for compile time string ID generation
constexpr uint32_t SID(const char* str)
{
	return sht::utility::crc32((uint8_t*)str, sht::utility::strlen_c(str));
}

// Runtime string ID generation
#define MakeStringId(x) (sht::utility::crc32_runtime(x))

#endif