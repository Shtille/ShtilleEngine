#include "../../include/string/filename.h"
#include <ctype.h>

namespace sht {
	namespace system {

		Filename::Filename(const char* filename)
			: filename_(filename)
		{
			
		}
		Filename::~Filename()
		{
			
		}
		Filename::Filename(const Filename& fn)
		{
			filename_ = fn.filename_;
		}
		Filename& Filename::operator =(const Filename& fn)
		{
			filename_ = fn.filename_;
			return *this;
		}
		std::string Filename::ExtractPath()
		{
			std::size_t found = filename_.find_last_of("/\\");
			std::string path;
			if (found != std::string::npos)
				path = filename_.substr(0,found);
			return path;
		}
		std::string Filename::ExtractFile()
		{
			std::size_t found = filename_.find_last_of("/\\");
			std::string file;
			if (found != std::string::npos)
				file = filename_.substr(found+1);
			return file;
		}
		std::string Filename::ExtractExt()
		{
			std::size_t found = filename_.find_last_of(".");
			std::string ext;
			if (found != std::string::npos)
			{
				ext = filename_.substr(found+1);
				char* p = &ext[0];
				while ((*p = (char)tolower(*p))) ++p;
			}
			return ext;
		}
	
	} // namespace system
} // namespace sht