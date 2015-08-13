#pragma once
#ifndef __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__
#define __SHT_GRAPHICS_VIDEO_MEMORY_BUFFER_H__

#include "../../../common/types.h"

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
            VideoMemoryBuffer();
            virtual ~VideoMemoryBuffer();
        };
        
    } // namespace graphics
} // namespace sht

#endif