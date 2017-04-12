#pragma once
#ifndef __SHT_SYSTEM_STRING_ID_H__
#define __SHT_SYSTEM_STRING_ID_H__

namespace sht {
	namespace utility {

		typedef unsigned int StringId;

		// Temporary done as an inline function, later i gonna exchange it onto conexpr function
		inline unsigned int _MakeStringHashConst(const char* buffer)
		{
			unsigned int hash = 5381;
			int c;
			while ((c = *buffer++))
				hash = ((hash << 5) + hash) ^ c;
			return hash;
		}
		unsigned int MakeStringHash(const char* buffer)
		{
			unsigned int hash = 5381;
			int c;
			while ((c = *buffer++))
				hash = ((hash << 5) + hash) ^ c;
			return hash;
		}

#define SID(char_buffer) _MakeStringHashConst(char_buffer)
	
	} // namespace utility
} // namespace sht

#endif