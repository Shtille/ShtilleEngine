#pragma once
#ifndef __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__
#define __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__

#include "../../../common/types.h"
#include "context.h"

namespace sht {
    namespace graphics {
        
        enum class BufferUsage : int {
            kStaticDraw,
            kStaticRead,
            kStaticCopy,
            kStreamDraw,
            kStreamRead,
            kStreamCopy,
            kDynamicDraw,
            kDynamicRead,
            kDynamicCopy,
            kCount
        };
        
        class VideoMemoryBuffer {
        public:
            VideoMemoryBuffer(Context * context);
            virtual ~VideoMemoryBuffer();
            
            virtual void* Lock(DataAccessType access) = 0;
            virtual void  Unlock() = 0;
            
        protected:
            Context * context_;
        };
        
    } // namespace graphics
} // namespace sht

#endif