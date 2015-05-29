#include "../../include/stream/log_stream.h"

namespace sht {
	namespace system {

		LogStream::LogStream(const char *filename)
		{
			opened_ = Open(filename, StreamAccess::kAppend);
		}
		LogStream::~LogStream()
		{
			if (opened_)
				Close();
		}

	} // namespace system
} // namespace sht