#pragma once
#ifndef __SHT_SYSTEM_FILE_OPERATIONS_H__
#define __SHT_SYSTEM_FILE_OPERATIONS_H__

#include "../../../common/types.h"

namespace sht {
	namespace system {

		bool RemoveFile(const char* filename);
		bool RenameFile(const char* old_name, const char* new_name);

		u64 ObtainFileSize(const char* filename);

	} // namespace system
} // namespace sht

#endif