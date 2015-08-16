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
            
            void DrawArrays(PrimitiveType mode, s32 first, u32 count);
            void DrawElements(PrimitiveType mode, u32 num_indices, DataType index_type);
            
            // Vertex array object
            void GenVertexArrayObject(u32 &obj);
            void DeleteVertexArrayObject(u32 &obj);
            void BindVertexArrayObject(u32 obj);
            
            // Vertex buffer object
            void GenVertexBuffer(u32& obj);
            void DeleteVertexBuffer(u32& obj);
            void BindVertexBuffer(u32 obj);
            void VertexBufferData(u32 size, const void *data, u32 usage);
            
            void VertexAttribPointer(u32 index, s32 size, DataType type, u32 stride, const void* ptr);
            void EnableVertexAttribArray(u32 index);
            
        protected:
            void FillTables();
        };
        
    }
}

#endif