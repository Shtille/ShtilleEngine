#include "../../include/renderer/vertex_buffer.h"

namespace sht {
    namespace graphics {
        
        VertexBuffer::VertexBuffer(Context * context)
        : VideoMemoryBuffer(context)
        {
            context_->GenVertexBuffer(id_);
        }
        VertexBuffer::~VertexBuffer()
        {
            context_->DeleteVertexBuffer(id_);
        }
        u32 VertexBuffer::GetSize()
        {
            return size_;
        }
        void VertexBuffer::Bind()
        {
            context_->BindVertexBuffer(id_);
        }
        void VertexBuffer::Unbind()
        {
            context_->BindVertexBuffer(0);
        }
        void VertexBuffer::SetData(u32 size, void *data, BufferUsage usage)
        {
            context_->VertexBufferData(size, data, usage);
        }
        void* VertexBuffer::Lock(DataAccessType access)
        {
            return context_->MapVertexBufferData(access);
        }
        void VertexBuffer::Unlock()
        {
            context_->UnmapVertexBufferData();
        }
        
    } // namespace graphics
} // namespace sht