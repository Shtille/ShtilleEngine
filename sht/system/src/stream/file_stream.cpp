#include "../../include/stream/file_stream.h"
#include "../../../common/sht_string.h"

namespace sht {
	namespace system {

		inline bool stream_has_mode(StreamAccess mode, StreamAccess checked_mode)
		{
			return ((int)mode & (int)checked_mode) == (int)checked_mode;
		}
		FileStream::FileStream() : file_(nullptr)
		{
		}
		FileStream::~FileStream()
		{
			Close();
		}
		bool FileStream::Open(const char *filename, StreamAccess mode)
		{
			char mode_str[10] = {};
			// basic modes
			if (stream_has_mode(mode, StreamAccess::kRead))
				strcpy_s(mode_str, "r");
			else if (stream_has_mode(mode, StreamAccess::kWrite))
				strcpy_s(mode_str, "w");
			else if (stream_has_mode(mode, StreamAccess::kReadWrite))
				strcpy_s(mode_str, "r+");
			else if (stream_has_mode(mode, StreamAccess::kAppend))
				strcpy_s(mode_str, "a");
			else
			{
				// mode is not selected, choose read write
				strcpy_s(mode_str, "r+");
			}
			if (stream_has_mode(mode, StreamAccess::kText))
				strcat_s(mode_str, "t");

#ifdef _MFC_VER
			errno_t err = fopen_s(&file_, filename, mode_str);
            return err == 0;
#else
            file_ = fopen(filename, mode_str);
            return file_ != nullptr;
#endif
		}
		void FileStream::Close()
		{
			if (file_)
			{
				fclose(file_);
				file_ = nullptr;
			}
		}
		bool FileStream::Write(const void *buffer, size_t size)
		{
			return fwrite(buffer, size, 1, file_) == 1;
		}
		bool FileStream::Read(void *buffer, size_t size)
		{
			return fread(buffer, size, 1, file_) == 1;
		}
		bool FileStream::ReadString(void *buffer, size_t max_size)
		{
			return fgets(reinterpret_cast<char*>(buffer), static_cast<int>(max_size), file_) != NULL;
		}
		bool FileStream::Eof()
		{
			return feof(file_) != 0;
		}
		void FileStream::Seek(long offset, StreamOffsetOrigin origin)
		{
			//static const int origins[3] = {SEEK_SET, SEEK_CUR, SEEK_END};
			fseek(file_, offset, (int)origin);
		}
		void FileStream::Rewind()
		{
			fseek(file_, 0, SEEK_SET);
		}
		size_t FileStream::Length()
		{
			long pos = ftell(file_);
			fseek(file_, 0, SEEK_END);
			long size = ftell(file_);
			fseek(file_, pos, SEEK_SET);
			return static_cast<size_t>(size);
		}

	} // namespace system
} // namespace sht