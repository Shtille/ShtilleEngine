#pragma once
#ifndef __SHT_GRAPHICS_OPENGL_CONTEXT_H__
#define __SHT_GRAPHICS_OPENGL_CONTEXT_H__

#include "../context.h"

namespace sht {
    namespace graphics {
        
        class OpenGlContext final : public Context {
        public:
            OpenGlContext();
            ~OpenGlContext();
            
            void ClearColor(f32 r, f32 g, f32 b, f32 a);
            void ClearColorBuffer();
            void ClearDepthBuffer();
            void ClearColorAndDepthBuffers();
            
            void Viewport(int w, int h);
            
            void EnableBlend();
            void DisableBlend();
            
            void EnableDepthTest();
            void DisableDepthTest();
            void EnableDepthWrite();
            void DisableDepthWrite();
            
            void EnableWireframeMode();
            void DisableWireframeMode();
            
            void DrawArrays(u32 mode, s32 first, u32 count);
            void DrawElements(u32 mode, u32 num_indices, u32 index_type);
        };
        
    }
}

#endif