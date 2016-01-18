#include "../../include/renderer/index_buffer.h"

namespace sht {
    namespace graphics {
        
        IndexBuffer::IndexBuffer(Context * context)
        : VideoMemoryBuffer(context)
        {
            context_->GenIndexBuffer(id_);
        }
        IndexBuffer::~IndexBuffer()
        {
            context_->DeleteIndexBuffer(id_);
        }
        u32 IndexBuffer::GetSize()
        {
            return index_count_ * index_size_;
        }
        void IndexBuffer::Bind()
        {
            context_->BindIndexBuffer(id_);
        }
        void IndexBuffer::Unbind()
        {
            context_->BindIndexBuffer(0);
        }
        void IndexBuffer::SetData(u32 size, const void *data, BufferUsage usage)
        {
            context_->IndexBufferData(size, data, usage);
        }
        void IndexBuffer::SubData(u32 size, const void *data)
        {
            context_->IndexBufferSubData(size, data);
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