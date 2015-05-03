#pragma once
#ifndef __SHT_SYSTEM_STREAM_FILE_STREAM_H__
#define __SHT_SYSTEM_STREAM_FILE_STREAM_H__

#include "stream.h"
#include "../../../common/platform.h"
#include <stdio.h> // for FILE

namespace sht {
	namespace system {

		class FileStream : public Stream {
		public:
			FileStream();
			~FileStream();

			bool Open(const char *filename, StreamAccess mode);
			void Close();

			bool Write(const void *buffer, size_t size);
			bool Read(void *buffer, size_t size);
			bool ReadString(void *buffer, size_t max_size);

			bool Eof();
			void Seek(long offset, StreamOffsetOrigin origin);
			void Rewind();
			size_t Length(); //!< Obtain file size, up to 2Gb (long max)

		private:
			//! Object copying is not allowed
			FileStream(const FileStream&) {}
			void operator = (const FileStream&) {}

		private:
			FILE *file_;
		};

	} // namespace system
} // namespace sht

#endif