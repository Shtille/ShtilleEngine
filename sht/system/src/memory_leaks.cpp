#include "../include/memory_leaks.h"

namespace sht {
	namespace system {

		void EnableMemoryLeaksChecking()
		{
#ifdef _DEBUG

#ifdef _WIN32
			// Get current flag
			int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

			// Turn on leak-checking bit.
			tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

			// Turn off CRT block checking bit.
			tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

			// Set flag to the new value.
			_CrtSetDbgFlag(tmpFlag);
#endif

#endif
		}

	} // namespace system
} // namespace sht