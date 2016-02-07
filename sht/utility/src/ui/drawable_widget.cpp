#include "../../include/ui/drawable_widget.h"

namespace sht {
	namespace utility {
		namespace ui {

            DrawableWidget::DrawableWidget(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader, sht::graphics::Texture * texture,
                               f32 x, f32 y, u32 flags)
            : Widget(x, y, flags)
            , renderer_(renderer)
            , shader_(shader)
            , texture_(texture)
            , vertex_format_(nullptr)
            , vertex_buffer_(nullptr)
            , vertex_array_object_(0)
            , num_vertices_(0)
            , vertices_array_(nullptr)
            , can_render_(false)
			{

			}
			DrawableWidget::~DrawableWidget()
			{
                if (vertex_format_)
                    renderer_->DeleteVertexFormat(vertex_format_);
                if (vertex_buffer_)
                    renderer_->DeleteVertexBuffer(vertex_buffer_);
                if (vertex_array_object_)
                    renderer_->context()->DeleteVertexArrayObject(vertex_array_object_);
                FreeArrays();
			}
            void DrawableWidget::FreeArrays()
            {
                if (vertices_array_)
                {
                    delete[] vertices_array_;
                    vertices_array_ = nullptr;
                }
            }
            void DrawableWidget::MakeRenderable()
            {
                assert(!attribs_.empty());
                renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (u32)attribs_.size());
                renderer_->context()->CheckForErrors();
                
                renderer_->context()->GenVertexArrayObject(vertex_array_object_);
                renderer_->context()->BindVertexArrayObject(vertex_array_object_);
                renderer_->context()->CheckForErrors();
                
                renderer_->AddVertexBuffer(vertex_buffer_, num_vertices_ * vertex_format_->vertex_size(), vertices_array_, sht::graphics::BufferUsage::kStaticDraw);
                renderer_->context()->CheckForErrors();
                if (vertex_buffer_ == nullptr)
                {
                    can_render_ = false;
                    return;
                }
                
                // Free all data in memory
                FreeArrays();
                
                // There is only one attribute
                const char* base = (char*)0;
                for (u32 i = 0; i < attribs_.size(); ++i)
                {
                    const sht::graphics::VertexFormat::Attrib& vf_attrib = vertex_format_->generic(i);
                    renderer_->context()->VertexAttribPointer(i, vf_attrib.size, sht::graphics::DataType::kFloat, vertex_format_->vertex_size(), base + vf_attrib.offset);
                    renderer_->context()->CheckForErrors();
                    renderer_->context()->EnableVertexAttribArray(i);
                    renderer_->context()->CheckForErrors();
                }
                
                renderer_->context()->BindVertexArrayObject(0);
                renderer_->context()->CheckForErrors();
                
                attribs_.clear();
                attribs_.shrink_to_fit();
                
                can_render_ = true;
            }
            void DrawableWidget::Render()
            {
                if (!can_render_)
                    return;
                
                renderer_->ChangeTexture(texture_);
                renderer_->context()->BindVertexArrayObject(vertex_array_object_);
                renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangleStrip, 0, num_vertices_);
                renderer_->context()->BindVertexArrayObject(0);
            }
            sht::graphics::Renderer * DrawableWidget::renderer()
            {
                return renderer_;
            }

		} // namespace ui
	} // namespace utility
} // namespace sht