#pragma once
#ifndef __SHT_SYSTEM_STREAM_LOG_STREAM_H__
#define __SHT_SYSTEM_STREAM_LOG_STREAM_H__

#include "file_stream.h"

namespace sht {
	namespace system {

		class LogStream : private FileStream {
		public:
			LogStream(const char *filename);
			~LogStream();

			// Make some functions public
			using Stream::WriteLine;

		private:
			//! Object copying is not allowed
			LogStream(const LogStream&) {}
			void operator = (const LogStream&) {}

		private:
			bool opened_;
		};

	} // namespace system
} // namespace sht

#endif