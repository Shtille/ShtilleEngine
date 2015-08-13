#include "../../include/renderer/vertex_buffer.h"

namespace sht {
    namespace graphics {
        
        VertexBuffer::VertexBuffer()
        {
        }
        VertexBuffer::~VertexBuffer()
        {
        }
        u32 VertexBuffer::GetSize()
        {
            return size_;
        }
        
    } // namespace graphics
} // namespace sht