#include "../../include/model/model.h"

namespace sht {
    namespace graphics {
        
        Model::Model(Renderer * renderer)
        : renderer_(renderer)
        , vertex_format_(nullptr)
        , vertex_buffer_(nullptr)
        , index_buffer_(nullptr)
        , primitive_mode_(PrimitiveType::kTriangleStrip)
        {
            
        }
        Model::~Model()
        {
            
        }
        void Model::SetFormat(VertexAttribute *attribs, u32 num_attribs)
        {
            renderer_->AddVertexFormat(vertex_format_, attribs, num_attribs);
        }
        void Model::Render()
        {
            renderer_->ChangeVertexFormat(vertex_format_);
            renderer_->ChangeVertexBuffer(vertex_buffer_);
            renderer_->ChangeIndexBuffer(index_buffer_);
            renderer_->DrawElements(primitive_mode_);
        }

    } // namespace graphics
} // namespace sht