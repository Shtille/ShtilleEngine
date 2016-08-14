#include "../../include/stream/memory_stream.h"
#include <memory.h>

namespace sht {
	namespace system {

		MemoryStream::MemoryStream() : size_(0U), buffer_(nullptr), ptr_(nullptr)
		{
		}
		MemoryStream::~MemoryStream()
		{
			Close();
		}
		bool MemoryStream::Open(size_t size, StreamAccess mode)
		{
			size_ = size;
			buffer_ = new char[size];
			ptr_ = buffer_;
			mode_ = mode;
			return true;
		}
		void MemoryStream::Close()
		{
			if (buffer_)
			{
				delete[] buffer_;
				buffer_ = nullptr;
				ptr_ = nullptr;
				size_ = 0U;
			}
		}
		bool MemoryStream::Write(const void *buffer, size_t size)
		{
			if (mode_ == StreamAccess::kWrite || mode_ == StreamAccess::kReadWrite)
			{
				memcpy(ptr_, buffer, size);
				ptr_ += size;
				return true;
			}
			else
				return false;
		}
		bool MemoryStream::Read(void *buffer, size_t size)
		{
			if (mode_ == StreamAccess::kRead || mode_ == StreamAccess::kReadWrite)
			{
				memcpy(buffer, ptr_, size);
				ptr_ += size;
				return true;
			}
			else
				return false;
		}
		bool MemoryStream::ReadString(void *buffer, size_t max_size)
		{
			if (mode_ == StreamAccess::kRead || mode_ == StreamAccess::kReadWrite)
			{
				memcpy(buffer, ptr_, max_size);
				ptr_ += max_size;
				return true;
			}
			else
				return false;
		}
		bool MemoryStream::Eof()
		{
			return ptr_ < buffer_ || ptr_ >= buffer_ + size_;
		}
		void MemoryStream::Seek(long offset, StreamOffsetOrigin origin)
		{
			switch (origin)
			{
			case StreamOffsetOrigin::kBeginning:
				ptr_ = buffer_ + static_cast<ptrdiff_t>(offset);
				break;
			case StreamOffsetOrigin::kCurrent:
				ptr_ += static_cast<ptrdiff_t>(offset);
				break;
			case StreamOffsetOrigin::kEnd:
				ptr_ = buffer_ + size_ + static_cast<ptrdiff_t>(offset);
				break;
			}
		}
		long MemoryStream::Tell()
		{
			return static_cast<long>(ptr_ - buffer_);
		}
		void MemoryStream::Rewind()
		{
			ptr_ = buffer_;
		}
		size_t MemoryStream::Length()
		{
			return size_;
		}

	} // namespace system
} // namespace sht