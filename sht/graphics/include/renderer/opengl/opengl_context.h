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
            
            bool CheckForErrors();
            
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
            void VertexBufferData(u32 size, const void *data, BufferUsage usage);
            void VertexBufferSubData(u32 size, const void *data);
            void* MapVertexBufferData(DataAccessType access);
            void UnmapVertexBufferData();
            
            // Index buffer object
            void GenIndexBuffer(u32& obj);
            void DeleteIndexBuffer(u32& obj);
            void BindIndexBuffer(u32 obj);
            void IndexBufferData(u32 size, const void *data, BufferUsage usage);
            void IndexBufferSubData(u32 size, const void *data);
            void* MapIndexBufferData(DataAccessType access);
            void UnmapIndexBufferData();
            
            // Vertex attribs
            void VertexAttribPointer(u32 index, s32 size, DataType type, u32 stride, const void* ptr);
            void EnableVertexAttribArray(u32 index);
            
            // Shader
            void DeleteProgram(u32 program);
            void BindProgram(u32 program);
            void BindAttribLocation(u32 program, const char *name);
            void Uniform1i(u32 program, const char *name, int x);
            void Uniform2i(u32 program, const char *name, int x, int y);
            void Uniform3i(u32 program, const char *name, int x, int y, int z);
            void Uniform4i(u32 program, const char *name, int x, int y, int z, int w);
            void Uniform1f(u32 program, const char *name, float x);
            void Uniform2f(u32 program, const char *name, float x, float y);
            void Uniform3f(u32 program, const char *name, float x, float y, float z);
            void Uniform4f(u32 program, const char *name, float x, float y, float z, float w);
            void Uniform1fv(u32 program, const char *name, const float *v, int n = 1);
            void Uniform2fv(u32 program, const char *name, const float *v, int n = 1);
            void Uniform3fv(u32 program, const char *name, const float *v, int n = 1);
            void Uniform4fv(u32 program, const char *name, const float *v, int n = 1);
            void UniformMatrix2fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1);
            void UniformMatrix3fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1);
            void UniformMatrix4fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1);
            
        protected:
            void FillTables();
            bool CheckFrameBufferStatus();
        };
        
    }
}

#endif