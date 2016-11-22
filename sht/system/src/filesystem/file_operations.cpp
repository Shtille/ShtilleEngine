#include "../../include/filesystem/file_operations.h"

#include <stdio.h>

namespace sht {
	namespace system {

		bool RemoveFile(const char* filename)
		{
			return ::remove(filename) == 0;
		}
		bool RenameFile(const char* old_name, const char* new_name)
		{
			return ::rename(old_name, new_name) == 0;
		}
		u64 ObtainFileSize(const char* filename)
		{
			u64 size(0ULL);
			FILE * file;
			file = fopen(filename, "rb");
			if (file)
			{
				fseek(file, 0, SEEK_END);
				size = static_cast<u64>(ftell(file));
				fclose(file);
			}
			return size;
		}

	} // namespace system
} // namespace sht