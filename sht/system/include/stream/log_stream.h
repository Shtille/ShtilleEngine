#pragma once
#ifndef __SHT_SYSTEM_STREAM_LOG_STREAM_H__
#define __SHT_SYSTEM_STREAM_LOG_STREAM_H__

#include "file_stream.h"
#include "../../../common/singleton.h"

namespace sht {
	namespace system {

        //! Simple scope class for logging
		class ScopeLogStream : private FileStream {
		public:
			ScopeLogStream(const char *filename);
			virtual ~ScopeLogStream();

			// Make some functions public
			using Stream::WriteLine;
            using Stream::PrintString;
		};
        
        //! Unique logging class
        template <class T>
        class UniqueLogStream : private FileStream, public Singleton<T> {
        public:
            // Make some functions public
            using Stream::WriteLine;
            using Stream::PrintString;
            
        protected:
            UniqueLogStream()
            {
                const char* filename = T::GetFilename();
                bool opened = Open(filename, StreamAccess::kWriteText);
                assert(opened);
            }
            virtual ~UniqueLogStream() = default;
        };
        
        //! Common logging class
        class CommonLogStream : public UniqueLogStream<CommonLogStream> {
            friend class Singleton<CommonLogStream>;
        public:
            static const char* GetFilename()
            {
                return "log.txt";
            }
        protected:
            CommonLogStream() = default;
            virtual ~CommonLogStream() = default;
        };

		//! Error logging class
		class ErrorLogStream : public UniqueLogStream<ErrorLogStream> {
			friend class Singleton<ErrorLogStream>;
		public:
			static const char* GetFilename()
			{
				return "errorlog.txt";
			}
		protected:
			ErrorLogStream() = default;
			virtual ~ErrorLogStream() = default;
		};

	} // namespace system
} // namespace sht

// Useful defines
#define LOG_INFO sht::system::CommonLogStream::GetInstance()->PrintString
#define LOG_ERROR sht::system::ErrorLogStream::GetInstance()->PrintString

#endif