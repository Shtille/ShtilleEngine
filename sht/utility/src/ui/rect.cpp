#include "../../include/ui/rect.h"

namespace sht {
	namespace utility {
		namespace ui {

            Rect::Rect(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                       sht::graphics::Texture * texture,
                       f32 x, f32 y, f32 width, f32 height, u32 flags)
			: DrawableWidget(renderer, shader, texture, x, y, flags)
			, width_(width)
			, height_(height)
			{
			}
			f32 Rect::width() const
			{
				return width_;
			}
			f32 Rect::height() const
			{
				return height_;
			}
			void Rect::set_width(f32 width)
			{
				width_ = width;
			}
			void Rect::set_height(f32 height)
			{
				height_ = height;
			}
			bool Rect::IsInside(f32 x, f32 y)
			{
				return (position_.x < x) && (position_.y < y) &&
					(x < position_.x + width_) && (y < position_.y + height_);
			}
            RectColored::RectColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                                     const vec4& color, f32 x, f32 y, f32 width, f32 height, u32 flags)
            : Rect(renderer, shader, nullptr, x, y, width, height, flags)
            , color_(color)
            {
                FillVertexAttribs();
                FillVertices();
                MakeRenderable();
                BindConstUniforms();
            }
            void RectColored::Render()
            {
                vec2 position;
                ObtainGlobalPosition(&position);
                shader_->Bind();
                shader_->Uniform2fv("u_position", position);
                DrawableWidget::Render();
                shader_->Unbind();
            }
            void RectColored::BindConstUniforms()
            {
                shader_->Bind();
                shader_->Uniform4fv("u_color", color_);
                shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
                shader_->Unbind();
            }
            void RectColored::FillVertexAttribs()
            {
                sht::graphics::VertexAttribute attrib(sht::graphics::VertexAttribute::kVertex, 2);
                attribs_.push_back(attrib);
            }
            void RectColored::FillVertices()
            {
                num_vertices_ = 4;
                vertices_array_ = new u8[num_vertices_ * sizeof(vec2)]; // 4 * 2 * s(float)
                vec2 * vertices = reinterpret_cast<vec2*>(vertices_array_);
                
                // Lower left
                vertices[0].x = 0.0f;
                vertices[0].y = 0.0f;
                
                // Lower right
                vertices[1].x = width_;
                vertices[1].y = 0.0f;
                
                // Upper left
                vertices[2].x = 0.0f;
                vertices[2].y = height_;
                
                // Upper right
                vertices[3].x = width_;
                vertices[3].y = height_;
            }
            RectTextured::RectTextured(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                	sht::graphics::Texture * texture, f32 x, f32 y, f32 width, f32 height, u32 flags)
            : Rect(renderer, shader, texture, x, y, width, height, flags)
            {
                FillVertexAttribs();
                FillVertices();
                MakeRenderable();
                BindConstUniforms();
            }
            void RectTextured::Render()
            {
            	vec2 position;
                ObtainGlobalPosition(&position);
                shader_->Bind();
                shader_->Uniform2fv("u_position", position);
                DrawableWidget::Render();
                shader_->Unbind();
            }
            void RectTextured::BindConstUniforms()
            {
            	shader_->Bind();
                shader_->Uniform1f("u_texture", 0);
                shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
                shader_->Unbind();
            }
            void RectTextured::FillVertexAttribs()
            {
                sht::graphics::VertexAttribute attrib(sht::graphics::VertexAttribute::kVertex, 4);
                attribs_.push_back(attrib);
            }
            void RectTextured::FillVertices()
            {
                num_vertices_ = 4;
                vertices_array_ = new u8[num_vertices_ * sizeof(vec4)]; // x, y, tx, ty
                vec4 * vertices = reinterpret_cast<vec4*>(vertices_array_);
                
                // Lower left
                vertices[0].x = 0.0f;
                vertices[0].y = 0.0f;
                vertices[0].z = 0.0f;
                vertices[0].w = 0.0f;
                
                // Lower right
                vertices[1].x = width_;
                vertices[1].y = 0.0f;
                vertices[1].z = 1.0f;
                vertices[1].w = 0.0f;
                
                // Upper left
                vertices[2].x = 0.0f;
                vertices[2].y = height_;
                vertices[2].z = 0.0f;
                vertices[2].w = 1.0f;
                
                // Upper right
                vertices[3].x = width_;
                vertices[3].y = height_;
                vertices[3].z = 1.0f;
                vertices[3].w = 1.0f;
            }

		} // namespace ui
	} // namespace utility
} // namespace sht