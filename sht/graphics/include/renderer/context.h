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
            kCount
        };
        
        enum class DataType {
            kUnsignedShort,
            kUnsignedInt,
            kFloat,
            kCount
        };
        
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
            virtual void VertexBufferData(u32 size, const void *data, u32 usage) = 0;
            
            virtual void VertexAttribPointer(u32 index, s32 size, DataType type, u32 stride, const void* ptr) = 0;
            virtual void EnableVertexAttribArray(u32 index) = 0;
            
        protected:
            virtual void FillTables() = 0;
            
            EnumTable<PrimitiveType, u32> primitive_type_map_;  //!< primitive type map
            EnumTable<DataType, u32> data_type_map_;            //!< primitive type map
        };
        
    }
} // namespace sht

#endif