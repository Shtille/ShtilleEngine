#include "../../include/renderer/index_buffer.h"

namespace sht {
    namespace graphics {
        
        IndexBuffer::IndexBuffer(Context * context)
        : VideoMemoryBuffer(context)
        {
        }
        IndexBuffer::~IndexBuffer()
        {
        }
        u32 IndexBuffer::GetSize()
        {
            return index_count_ * index_size_;
        }
        void* IndexBuffer::Lock(DataAccessType access)
        {
            return context_->MapIndexBufferData(access);
        }
        void IndexBuffer::Unlock()
        {
            context_->UnmapIndexBufferData();
        }
        
    } // namespace graphics
} // namespace sht