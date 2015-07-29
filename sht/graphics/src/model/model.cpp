#include "../../include/model/model.h"

namespace sht {
    namespace graphics {
        
        Model::Model(Renderer * renderer)
        : renderer_(renderer)
        , vertex_format_(nullptr)
        , vertex_buffer_(nullptr)
        , index_buffer_(nullptr)
        , primitive_mode_(PrimitiveType::kTriangleStrip)
        , num_vertices_(0)
        , vertices_(nullptr)
        , num_indices_(0)
        , indices_(nullptr)
        {
            
        }
        Model::~Model()
        {
            if (vertex_format_)
                renderer_->DeleteVertexFormat(vertex_format_);
            if (vertex_buffer_)
                renderer_->DeleteVertexBuffer(vertex_buffer_);
            if (index_buffer_)
                renderer_->DeleteIndexBuffer(index_buffer_);
            FreeArrays();
        }
        void Model::SetFormat(VertexAttribute *attribs, u32 num_attribs)
        {
            renderer_->AddVertexFormat(vertex_format_, attribs, num_attribs);
        }
        void Model::FreeArrays()
        {
            if (vertices_)
            {
                delete [] vertices_;
                vertices_ = nullptr;
            }
            if (indices_)
            {
                delete [] indices_;
                indices_ = nullptr;
            }
        }
        bool Model::MakeRenderable()
        {
            if (vertex_format_ == nullptr)
            {
                assert(!"Vertex format hasn't been set.");
                return false;
            }
            u32 index_size = (num_indices_ > 0xffff) ? sizeof(u32) : sizeof(u16);
            renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_size(), vertices_, BufferUsage::kStaticDraw);
            if (vertex_buffer_ == nullptr) return false;
            renderer_->AddIndexBuffer(index_buffer_, num_indices_, index_size, indices_, BufferUsage::kStaticDraw);
            if (index_buffer_ == nullptr) return false;
            FreeArrays();
            return true;
        }
        void Model::Render()
        {
            renderer_->ChangeVertexFormat(vertex_format_);
            renderer_->ChangeVertexBuffer(vertex_buffer_);
            renderer_->ChangeIndexBuffer(index_buffer_);
            renderer_->DrawElements(primitive_mode_);
        }
        u32 Model::vertex_size() const
        {
            return vertex_format_->vertex_size();
        }

    } // namespace graphics
} // namespace sht