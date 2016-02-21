#include "../include/crypt.h"
#include <cstring>

namespace sht {
	namespace utility {
	
		void Encrypt(char* buffer_out, const char* buffer_in, size_t length, const char* key)
		{
			size_t key_size = strlen(key);
			for (size_t i = 0; i < length; ++i)
			{
				buffer_out[i] = buffer_in[i] ^ key[i % key_size];
			}
		}
	
	} // namespace utility
} // namespace sht