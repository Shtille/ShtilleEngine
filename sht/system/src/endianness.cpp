#include "../include/endianness.h"

namespace sht {
	namespace system {

		bool IsLittleEndian()
		{
			int n = 1;
			return *(char*)&n == 1;
		}

	} // namespace system
} // namespace sht