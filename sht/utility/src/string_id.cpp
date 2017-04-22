#include "../include/string_id.h"

namespace sht {
	namespace utility {

		static uint32_t crc_table_runtime[] = { CRC_A(0) };

		uint32_t SimpleHash(const char* buffer)
		{
			uint32_t hash = 5381;
			int c;
			while ((c = *buffer++))
				hash = ((hash << 5) + hash) ^ c;
			return hash;
		}
		uint32_t crc32_runtime(const char* str)
		{
			uint32_t remainder = 0;
			for (const char* p = str; *p != '\0'; ++p)
			{
				uint8_t data = (*p) ^ (remainder >> 24);
				remainder = crc_table_runtime[data] ^ (remainder << 8);
			}
			return remainder;
		}

	} // namespace utility
} // namespace sht