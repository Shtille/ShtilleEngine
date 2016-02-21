#pragma once
#ifndef __SHT_UTILITY_CRYPT_H__
#define __SHT_UTILITY_CRYPT_H__

#include <cstddef>

namespace sht {
	namespace utility {
	
		//! Simple encryption/decryption function
		void Encrypt(char* buffer_out, const char* buffer_in, size_t length, const char* key);
	
	} // namespace utility
} // namespace sht

#endif