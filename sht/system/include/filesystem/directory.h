#pragma once
#ifndef __SHT_SYSTEM_DIRECTORY_H__
#define __SHT_SYSTEM_DIRECTORY_H__

namespace sht {
	namespace system {

		const char GetPathDelimeter();
		bool CreateDirectory(const char* path);
		bool RemoveDirectory(const char* path);

	} // namespace system
} // namespace sht

#endif