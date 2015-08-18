#pragma once
#ifndef __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__
#define __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__

#include "../../../common/types.h"
#include "context.h"

namespace sht {
    namespace graphics {
        
        class VideoMemoryBuffer {
        public:
            VideoMemoryBuffer(Context * context);
            virtual ~VideoMemoryBuffer();
            
            virtual void Bind() = 0;
            virtual void Unbind() = 0;
            
            virtual void SetData(u32 size, void *data, BufferUsage usage) = 0;
            
            virtual void* Lock(DataAccessType access) = 0;
            virtual void  Unlock() = 0;
            
        protected:
            Context * context_;
        };
        
    } // namespace graphics
} // namespace sht

#endif