#pragma once
#ifndef __SHT_GRAPHICS_CONTEXT_H__
#define __SHT_GRAPHICS_CONTEXT_H__

#include "../../../common/types.h"
#include "../../../common/table.h"

namespace sht {
    namespace graphics {
        
        enum class PrimitiveType {
            kLines,
            kLineStrip,
            kTriangles,
            kTriangleStrip,
            kQuads,
            kCount
        };
        
        enum class DataType {
            kUnsignedShort,
            kUnsignedInt,
            kFloat,
            kCount
        };
        
        enum class DataAccessType {
            kRead,
            kWrite,
            kReadWrite,
            kCount
        };
        
        enum class BufferUsage {
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
        
        enum class CullFaceType {
            kBack,
            kFront,
            kCount
        };
        
        class Context {
        public:
            Context();
            virtual ~Context();
            
            void ErrorHandler(const char *message);
            
            virtual bool CheckForErrors() = 0;
			virtual bool CheckFrameBufferStatus() = 0;
            
            virtual void ClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
            virtual void ClearColorBuffer() = 0;
            virtual void ClearDepthBuffer() = 0;
            virtual void ClearColorAndDepthBuffers() = 0;
            virtual void ClearStencil(s32 value) = 0;
            virtual void ClearStencilBuffer() = 0;
            
            virtual void Viewport(int w, int h) = 0;
            
            virtual void EnableBlend() = 0;
            virtual void DisableBlend() = 0;
            
            virtual void EnableDepthTest() = 0;
            virtual void DisableDepthTest() = 0;
            virtual void EnableDepthWrite() = 0;
            virtual void DisableDepthWrite() = 0;
            
            virtual void EnableStencilTest() = 0;
            virtual void DisableStencilTest() = 0;
            virtual void StencilMask(u32 mask) = 0;
            //virtual void StencilFunc() = 0;
            //virtual void StencilOp() = 0;
            
            virtual void EnableWireframeMode() = 0;
            virtual void DisableWireframeMode() = 0;
            
            virtual void CullFace(CullFaceType mode) = 0;
            
            virtual void DrawArrays(PrimitiveType mode, s32 first, u32 count) = 0;
            virtual void DrawElements(PrimitiveType mode, u32 num_indices, DataType index_type) = 0;
            
            // Vertex array object
            virtual void GenVertexArrayObject(u32 &obj) = 0;
            virtual void DeleteVertexArrayObject(u32 &obj) = 0;
            virtual void BindVertexArrayObject(u32 obj) = 0;
            
            // Vertex buffer object
            virtual void GenVertexBuffer(u32& obj) = 0;
            virtual void DeleteVertexBuffer(u32& obj) = 0;
            virtual void BindVertexBuffer(u32 obj) = 0;
            virtual void VertexBufferData(u32 size, const void *data, BufferUsage usage) = 0;
            virtual void VertexBufferSubData(u32 size, const void *data) = 0;
            virtual void* MapVertexBufferData(DataAccessType access) = 0;
            virtual void UnmapVertexBufferData() = 0;
            
            // Index buffer object
            virtual void GenIndexBuffer(u32& obj) = 0;
            virtual void DeleteIndexBuffer(u32& obj) = 0;
            virtual void BindIndexBuffer(u32 obj) = 0;
            virtual void IndexBufferData(u32 size, const void *data, BufferUsage usage) = 0;
            virtual void IndexBufferSubData(u32 size, const void *data) = 0;
            virtual void* MapIndexBufferData(DataAccessType access) = 0;
            virtual void UnmapIndexBufferData() = 0;
            
            // Vertex attribs
            virtual void VertexAttribPointer(u32 index, s32 size, DataType type, u32 stride, const void* ptr) = 0;
            virtual void EnableVertexAttribArray(u32 index) = 0;
            
            // Shader
            virtual void DeleteProgram(u32 program) = 0;
            virtual void BindProgram(u32 program) = 0;
            virtual void BindAttribLocation(u32 program, const char *name) = 0;
            virtual void Uniform1i(u32 program, const char *name, int x) = 0;
            virtual void Uniform2i(u32 program, const char *name, int x, int y) = 0;
            virtual void Uniform3i(u32 program, const char *name, int x, int y, int z) = 0;
            virtual void Uniform4i(u32 program, const char *name, int x, int y, int z, int w) = 0;
            virtual void Uniform1f(u32 program, const char *name, float x) = 0;
            virtual void Uniform2f(u32 program, const char *name, float x, float y) = 0;
            virtual void Uniform3f(u32 program, const char *name, float x, float y, float z) = 0;
            virtual void Uniform4f(u32 program, const char *name, float x, float y, float z, float w) = 0;
            virtual void Uniform1fv(u32 program, const char *name, const float *v, int n = 1) = 0;
            virtual void Uniform2fv(u32 program, const char *name, const float *v, int n = 1) = 0;
            virtual void Uniform3fv(u32 program, const char *name, const float *v, int n = 1) = 0;
            virtual void Uniform4fv(u32 program, const char *name, const float *v, int n = 1) = 0;
            virtual void UniformMatrix2fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
            virtual void UniformMatrix3fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
            virtual void UniformMatrix4fv(u32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
            
        protected:
            virtual void FillTables() = 0;
            
            EnumTable<PrimitiveType, u32> primitive_type_map_;  //!< primitive type map
            EnumTable<DataType, u32> data_type_map_;            //!< data type map
            EnumTable<DataAccessType, u32> data_access_map_;    //!< data access map
            EnumTable<BufferUsage, u32> buffer_usage_map_;      //!< buffer usage map
            EnumTable<CullFaceType, u32> cull_face_map_;        //!< cull face map
        };
        
    }
} // namespace sht

#endif