#pragma once
#ifndef __SHT_SYSTEM_STREAM_FILE_STREAM_H__
#define __SHT_SYSTEM_STREAM_FILE_STREAM_H__

#include "stream.h"

#include <stdio.h> // for FILE

namespace sht {
	namespace system {

		class FileStream : public Stream {
		public:
			FileStream();
			virtual ~FileStream();

			bool Open(const char *filename, StreamAccess mode);
			void Close();

			bool Write(const void *buffer, size_t size);
			bool Read(void *buffer, size_t size);
			bool ReadString(void *buffer, size_t max_size);

			bool Eof();
			void Seek(long offset, StreamOffsetOrigin origin);
			long Tell();
			void Rewind();
			size_t Length(); //!< Obtain file size, up to 2Gb (long max)
            
            FILE * GetFilePointer();

		protected:
			FILE * file_;
		};

	} // namespace system
} // namespace sht

#endif