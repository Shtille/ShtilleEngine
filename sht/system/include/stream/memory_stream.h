#pragma once
#ifndef __SHT_SYSTEM_STREAM_MEMORY_STREAM_H__
#define __SHT_SYSTEM_STREAM_MEMORY_STREAM_H__

#include "stream.h"

namespace sht {
	namespace system {

		class MemoryStream : public Stream {
		public:
			MemoryStream();
			~MemoryStream();

			bool Open(size_t size, StreamAccess mode);
			void Close();

			bool Write(const void *buffer, size_t size);
			bool Read(void *buffer, size_t size);
			bool ReadString(void *buffer, size_t max_size);

			bool Eof();
			void Seek(long offset, StreamOffsetOrigin origin);
			long Tell();
			void Rewind();
			size_t Length(); //!< Obtain file size, up to 2Gb (long max)

		protected:
			size_t size_;	//!< buffer size
			char *buffer_;	//!< beginning of the buffer
			char *ptr_;		//!< pointer to the current position
			StreamAccess mode_; //!< store mode
		};

	} // namespace system
} // namespace sht

#endif