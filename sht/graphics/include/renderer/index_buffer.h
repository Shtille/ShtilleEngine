#pragma once
#ifndef __SHT_GRAPHICS_INDEX_BUFFER_H__
#define __SHT_GRAPHICS_INDEX_BUFFER_H__

#include "video_memory_buffer.h"

namespace sht {
    namespace graphics {
        
        class IndexBuffer final : public VideoMemoryBuffer {
            friend class Renderer;
            friend class OpenGlRenderer;
            
        protected:
            IndexBuffer(Context * context);
            ~IndexBuffer();
            IndexBuffer(const IndexBuffer&) = delete;
            void operator = (const IndexBuffer&) = delete;
            
            u32 GetSize();
            
            void Bind();
            void Unbind();
            
            void SetData(u32 size, const void *data, BufferUsage usage);
            void SubData(u32 size, const void *data);
            
            void* Lock(DataAccessType access);
            void Unlock();
            
        private:
            u32 id_;
            u32 index_count_;
            u32 index_size_;
        };
        
    } // namespace graphics
} // namespace sht

#endif