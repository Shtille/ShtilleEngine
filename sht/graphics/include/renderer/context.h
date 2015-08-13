#pragma once
#ifndef __SHT_GRAPHICS_CONTEXT_H__
#define __SHT_GRAPHICS_CONTEXT_H__

#include "../../../common/types.h"

namespace sht {
    namespace graphics {
        
        class Context {
        public:
            Context();
            virtual ~Context();
            
            virtual void ClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
            virtual void ClearColorBuffer() = 0;
            virtual void ClearDepthBuffer() = 0;
            virtual void ClearColorAndDepthBuffers() = 0;
            
            virtual void Viewport(int w, int h) = 0;
            
            virtual void EnableBlend() = 0;
            virtual void DisableBlend() = 0;
            
            virtual void EnableDepthTest() = 0;
            virtual void DisableDepthTest() = 0;
            virtual void EnableDepthWrite() = 0;
            virtual void DisableDepthWrite() = 0;
            
            virtual void EnableWireframeMode() = 0;
            virtual void DisableWireframeMode() = 0;
            
            virtual void DrawArrays(u32 mode, s32 first, u32 count) = 0;
            virtual void DrawElements(u32 mode, u32 num_indices, u32 index_type) = 0;
        };
        
    }
} // namespace sht

#endif