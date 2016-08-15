#pragma once
#ifndef __SHT_SYSTEM_STREAM_STREAM_H__
#define __SHT_SYSTEM_STREAM_STREAM_H__

#include "../../../common/non_copyable.h"

#include <stddef.h>

namespace sht {
	namespace system {

		enum class StreamAccess : int {
			kText = 0x001,
            kBinary = 0x002,
			kRead = 0x010,
			kWrite = 0x020,
			kReadWrite = 0x030,
			kAppend = 0x080,
			kReadText = 0x011,
			kWriteText = 0x021,
            kReadBinary = 0x012,
            kWriteBinary = 0x022
		};
		enum class StreamOffsetOrigin : int {
			kBeginning = 0,
			kCurrent = 1,
			kEnd = 2
		};

		//! Stream base class
		class Stream : public NonCopyable {
		public:
			virtual ~Stream();

			virtual bool Write(const void *buffer, size_t size) = 0;
			virtual bool Read(void *buffer, size_t size) = 0;
			virtual bool ReadString(void *buffer, size_t max_size) = 0; //!< size in bytes

			// Operations with values
			template <class T>
			void WriteValue(const T& value);
			template <class T>
			void ReadValue(T& value);

			// Stream positioning
			virtual bool Eof() = 0;
			virtual void Seek(long offset, StreamOffsetOrigin origin) = 0;
			virtual long Tell() = 0;
			virtual void Rewind() = 0;
			virtual size_t Length() = 0;

			// Text operations
			bool WriteText(const char *text);
			bool WriteLine(const char *text);
			bool ReadLine(char string[256]); //!< reading line of text
			bool ReadCodeLine(char string[256]); //!< reading line of text, ignoring comments '//'
			bool PrintString(const char *string, ...); //!< writes formatted string to stream
			bool ScanString(const char *string, ...); //!< reads formatted string from line of stream
		};

		template <class T>
		void Stream::WriteValue(const T& value)
		{
			Write(&value, sizeof(value));
		}
		template <class T>
		void Stream::ReadValue(T& value)
		{
			Read(&value, sizeof(value));
		}

	} // namespace system
} // namespace sht

#endif