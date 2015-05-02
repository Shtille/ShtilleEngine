#pragma once
#ifndef __SHT_SYSTEM_MEMORY_LEAKS_H__
#define __SHT_SYSTEM_MEMORY_LEAKS_H__

#if defined(_WIN32) && defined(_DEBUG)

#include <crtdbg.h>

#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_CLIENTBLOCK

#else

#define DEBUG_CLIENTBLOCK

#endif

namespace sht {
	namespace system {

		void EnableMemoryLeaksChecking();

	} // namespace system
} // namespace sht

#endif