#pragma once
#ifndef __SHT_SYSTEM_FILENAME_H__
#define __SHT_SYSTEM_FILENAME_H__

#include <string>

namespace sht {
	namespace system {

		//! Class for some path/ext routines
		class Filename {
		public:
			Filename(const char* filename);
			~Filename();
			Filename(const Filename& fn);
			Filename& operator =(const Filename& fn);
			
			std::string ExtractPath();	//!< extracts path to the file
			std::string ExtractFile();	//!< extracts file name with extension
			std::string ExtractExt();	//!< extracts lower case file extension
			
		private:
			Filename() = delete;
			
			std::string filename_;
		};

	} // namespace system
} // namespace sht

#endif