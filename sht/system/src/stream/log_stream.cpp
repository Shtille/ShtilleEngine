#include "../../include/stream/log_stream.h"

#include <assert.h>

namespace sht {
	namespace system {

		ScopeLogStream::ScopeLogStream(const char *filename)
		{
			bool opened = Open(filename, StreamAccess::kAppend);
            assert(opened);
		}
		ScopeLogStream::~ScopeLogStream()
		{
		}

	} // namespace system
} // namespace sht