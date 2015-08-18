# include "../../../include/renderer/opengl/opengl_context.h"
#include "opengl_include.h"

namespace sht {
    namespace graphics {
        
        OpenGlContext::OpenGlContext()
        {
            FillTables();
        }
        OpenGlContext::~OpenGlContext()
        {
            
        }
        void OpenGlContext::FillTables()
        {
            primitive_type_map_[PrimitiveType::kLines] = GL_LINES;
            primitive_type_map_[PrimitiveType::kLineStrip] = GL_LINE_STRIP;
            primitive_type_map_[PrimitiveType::kTriangles] = GL_TRIANGLES;
            primitive_type_map_[PrimitiveType::kTriangleStrip] = GL_TRIANGLE_STRIP;
            assert(primitive_type_map_.size() == (size_t)PrimitiveType::kCount);
            
            data_type_map_[DataType::kUnsignedShort] = GL_UNSIGNED_SHORT;
            data_type_map_[DataType::kUnsignedInt] = GL_UNSIGNED_INT;
            data_type_map_[DataType::kFloat] = GL_FLOAT;
            assert(data_type_map_.size() == (size_t)DataType::kCount);
            
            data_access_map_[DataAccessType::kRead] = GL_READ_ONLY;
            data_access_map_[DataAccessType::kWrite] = GL_WRITE_ONLY;
            data_access_map_[DataAccessType::kReadWrite] = GL_READ_WRITE;
            assert(data_access_map_.size() == (size_t)DataAccessType::kCount);
            
            buffer_usage_map_[BufferUsage::kStaticDraw] = GL_STATIC_DRAW;
            buffer_usage_map_[BufferUsage::kStaticRead] = GL_STATIC_READ;
            buffer_usage_map_[BufferUsage::kStaticCopy] = GL_STATIC_COPY;
            buffer_usage_map_[BufferUsage::kDynamicDraw] = GL_DYNAMIC_DRAW;
            buffer_usage_map_[BufferUsage::kDynamicRead] = GL_DYNAMIC_READ;
            buffer_usage_map_[BufferUsage::kDynamicCopy] = GL_DYNAMIC_COPY;
            buffer_usage_map_[BufferUsage::kStreamDraw] = GL_STREAM_DRAW;
            buffer_usage_map_[BufferUsage::kStreamRead] = GL_STREAM_READ;
            buffer_usage_map_[BufferUsage::kStreamCopy] = GL_STREAM_COPY;
            assert(buffer_usage_map_.size() == (size_t)BufferUsage::kCount);
        }
        void OpenGlContext::ClearColor(f32 r, f32 g, f32 b, f32 a)
        {
            glClearColor(r, g, b, a);
        }
        void OpenGlContext::ClearColorBuffer()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }
        void OpenGlContext::ClearDepthBuffer()
        {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        void OpenGlContext::ClearColorAndDepthBuffers()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void OpenGlContext::Viewport(int w, int h)
        {
            glViewport(0, 0, w, h);
        }
        void OpenGlContext::EnableBlend()
        {
            glEnable(GL_BLEND);
        }
        void OpenGlContext::DisableBlend()
        {
            glDisable(GL_BLEND);
        }
        void OpenGlContext::EnableDepthTest()
        {
            glEnable(GL_DEPTH_TEST);
        }
        void OpenGlContext::DisableDepthTest()
        {
            glDisable(GL_DEPTH_TEST);
        }
        void OpenGlContext::EnableDepthWrite()
        {
            glDepthMask(GL_TRUE);
        }
        void OpenGlContext::DisableDepthWrite()
        {
            glDepthMask(GL_FALSE);
        }
        void OpenGlContext::EnableWireframeMode()
        {
            glPolygonMode(GL_FRONT, GL_LINE);
        }
        void OpenGlContext::DisableWireframeMode()
        {
            glPolygonMode(GL_FRONT, GL_FILL);
        }
        void OpenGlContext::DrawArrays(PrimitiveType mode, s32 first, u32 count)
        {
            u32 primitive_type = primitive_type_map_[mode];
            glDrawArrays(primitive_type, first, count);
        }
        void OpenGlContext::DrawElements(PrimitiveType mode, u32 num_indices, DataType index_type)
        {
            u32 primitive_type = primitive_type_map_[mode];
            u32 data_type = data_type_map_[index_type];
            glDrawElements(primitive_type, num_indices, data_type, 0);
        }
        void OpenGlContext::GenVertexArrayObject(u32 &obj)
        {
            glGenVertexArrays(1, &obj);
        }
        void OpenGlContext::DeleteVertexArrayObject(u32 &obj)
        {
            glDeleteVertexArrays(1, &obj);
        }
        void OpenGlContext::BindVertexArrayObject(u32 obj)
        {
            glBindVertexArray(obj);
        }
        void OpenGlContext::GenVertexBuffer(u32& obj)
        {
            glGenBuffers(1, &obj);
        }
        void OpenGlContext::DeleteVertexBuffer(u32& obj)
        {
            glDeleteBuffers(1, &obj);
        }
        void OpenGlContext::BindVertexBuffer(u32 obj)
        {
            glBindBuffer(GL_ARRAY_BUFFER, obj);
        }
        void OpenGlContext::VertexBufferData(u32 size, const void *data, BufferUsage usage)
        {
            u32 usage_type = buffer_usage_map_[usage];
            glBufferData(GL_ARRAY_BUFFER, size, data, usage_type);
        }
        void* OpenGlContext::MapVertexBufferData(DataAccessType access)
        {
            u32 access_type = data_access_map_[access];
            return glMapBuffer(GL_ARRAY_BUFFER, access_type);
        }
        void OpenGlContext::UnmapVertexBufferData()
        {
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        void OpenGlContext::GenIndexBuffer(u32& obj)
        {
            glGenBuffers(1, &obj);
        }
        void OpenGlContext::DeleteIndexBuffer(u32& obj)
        {
            glDeleteBuffers(1, &obj);
        }
        void OpenGlContext::BindIndexBuffer(u32 obj)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj);
        }
        void OpenGlContext::IndexBufferData(u32 size, const void *data, BufferUsage usage)
        {
            u32 usage_type = buffer_usage_map_[usage];
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage_type);
        }
        void* OpenGlContext::MapIndexBufferData(DataAccessType access)
        {
            u32 access_type = data_access_map_[access];
            return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, access_type);
        }
        void OpenGlContext::UnmapIndexBufferData()
        {
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }
        void OpenGlContext::VertexAttribPointer(u32 index, s32 size, DataType type, u32 stride, const void* ptr)
        {
            u32 data_type = data_type_map_[type];
            glVertexAttribPointer(index, size, data_type, GL_FALSE, stride, ptr);
        }
        void OpenGlContext::EnableVertexAttribArray(u32 index)
        {
            glEnableVertexAttribArray(index);
        }
        
    } // namespace graphics
} // namespace sht