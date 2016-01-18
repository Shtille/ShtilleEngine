#pragma once
#ifndef __SHT_GRAPHICS_VERTEX_BUFFER_H__
#define __SHT_GRAPHICS_VERTEX_BUFFER_H__

#include "video_memory_buffer.h"

namespace sht {
    namespace graphics {
        
        class VertexBuffer final : public VideoMemoryBuffer {
            friend class Renderer;
            friend class OpenGlRenderer;
            
            friend class DynamicText;
            
        protected:
            VertexBuffer(Context * context);
            ~VertexBuffer();
            VertexBuffer(const VertexBuffer&) = delete;
            void operator = (const VertexBuffer&) = delete;
            
            u32 GetSize();
            
            void Bind();
            void Unbind();
            
            void SetData(u32 size, const void *data, BufferUsage usage);
            void SubData(u32 size, const void *data);
            
            void* Lock(DataAccessType access);
            void Unlock();
            
        private:
            u32 id_;
            u32 size_;
        };
        
    } // namespace graphics
} // namespace sht

#endif