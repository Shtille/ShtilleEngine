#include "../../include/renderer/vertex_buffer.h"

namespace sht {
    namespace graphics {
        
        VertexBuffer::VertexBuffer(Context * context)
        : VideoMemoryBuffer(context)
        {
        }
        VertexBuffer::~VertexBuffer()
        {
        }
        u32 VertexBuffer::GetSize()
        {
            return size_;
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