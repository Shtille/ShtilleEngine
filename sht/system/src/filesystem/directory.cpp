#include "../../include/filesystem/directory.h"

#include "../../../common/platform.h"

#ifndef TARGET_WINDOWS
#include <sys/stat.h>
#include <unistd.h>
#endif // !TARGET_WINDOWS

namespace sht {
	namespace system {

		const char GetPathDelimeter()
		{
#ifdef TARGET_WINDOWS
			return '\\';
#else
			return '/';
#endif // TARGET_WINDOWS
		}
		bool CreateDirectory(const char* path)
		{
#ifdef TARGET_WINDOWS
			return CreateDirectoryA(path, NULL) != 0;
#else
			return mkdir(path, 0755) == 0;
#endif // TARGET_WINDOWS
		}
		bool RemoveDirectory(const char* path)
		{
#ifdef TARGET_WINDOWS
			return RemoveDirectoryA(path) != 0;
#else
			return rmdir(path) == 0;
#endif // TARGET_WINDOWS
		}

	} // namespace system
} // namespace sht