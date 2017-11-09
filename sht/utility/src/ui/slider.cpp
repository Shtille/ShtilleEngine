#include "../../include/ui/slider.h"

namespace sht {
	namespace utility {
		namespace ui {

			Slider::Slider(f32 x, f32 y, f32 width, f32 height, u32 flags)
			: Widget(x, y, flags)
			, width_(width)
			, height_(height)
			, old_position_(0.0f)
			, radius_((width < height) ? (0.5f * width) : (0.5f * height))
			, pin_position_(0.0f)
			, pin_radius_((width < height) ? (0.5f * width) : (0.5f * height))
			, is_touched_(false)
			{
			}
			void Slider::SetPinPosition(f32 pos)
			{
				pin_position_ = pos;
				if (pin_position_ < 0.0f)
					pin_position_ = 0.0f;
				if (pin_position_ > 1.0f)
					pin_position_ = 1.0f;
			}
			void Slider::OnTouchDown(const vec2& position)
			{
				if (IsInsidePin(position))
				{
					is_touched_ = true;
					old_position_ = position;
				}
			}
			void Slider::OnTouchMove(const vec2& position)
			{
				if (is_touched_)
				{
					// If gesture has gone away from the pin disable the pin
					if (!IsInsidePin(position))
					{
						is_touched_ = false;
						return;
					}

					float delta;
					if (is_vertical())
						delta = (position.y - old_position_.y) / (height_ - 2.0f * radius_);
					else
						delta = (position.x - old_position_.x) / (width_ - 2.0f * radius_);
					SetPinPosition(pin_position_ + delta);
					old_position_ = position;
				}
			}
			void Slider::OnTouchUp(const vec2& position)
			{
				is_touched_ = false;
			}
			f32 Slider::width() const
			{
				return width_;
			}
			f32 Slider::height() const
			{
				return height_;
			}
			float Slider::pin_position() const
			{
				return pin_position_;
			}
			float Slider::pin_radius() const
			{
				return pin_radius_;
			}
			bool Slider::is_vertical() const
			{
				return width_ < height_;
			}
			bool Slider::is_touched() const
			{
				return is_touched_;
			}
			bool Slider::IsInsidePin(const vec2& global_position)
			{
				vec2 position, pin_global_position;
				ObtainGlobalPosition(&position);
				if (is_vertical())
				{
					pin_global_position.x = position.x + width_ * 0.5f;
					pin_global_position.y = position.y + radius_ + pin_position_ * (height_ - 2.0f * radius_);
				}
				else
				{
					pin_global_position.x = position.x + radius_ + pin_position_ * (width_ - 2.0f * radius_);
					pin_global_position.y = position.y + height_ * 0.5f;
				}
				return
					(pin_global_position.x - pin_radius_ < global_position.x) &&
					(pin_global_position.y - pin_radius_ < global_position.y) &&
					(global_position.x < pin_global_position.x + pin_radius_) &&
					(global_position.y < pin_global_position.y + pin_radius_);
			}

			SliderColored::SliderColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
				 const vec4& bar_color, const vec4& pin_color_normal, const vec4& pin_color_touch,
				 f32 x, f32 y, f32 width, f32 height, u32 flags)
			: Slider(x, y, width, height, flags)
			, Drawable(renderer, shader, nullptr)
			, bar_color_(bar_color)
			, pin_color_normal_(pin_color_normal)
			, pin_color_touch_(pin_color_touch)
			, num_bar_vertices_(4)
			, num_pin_vertices_(4)
			{
				FillVertexAttribs();
				FillVertices();
				MakeRenderable();
				BindConstUniforms();
			}
			void SliderColored::Render()
			{
				vec2 position, pin_global_position;

				ObtainGlobalPosition(&position);
				if (is_vertical())
				{
					pin_global_position.x = position.x + width_ * 0.5f;
					pin_global_position.y = position.y + radius_ + pin_position_ * (height_ - 2.0f * radius_);
				}
				else
				{
					pin_global_position.x = position.x + radius_ + pin_position_ * (width_ - 2.0f * radius_);
					pin_global_position.y = position.y + height_ * 0.5f;
				}

				renderer_->ChangeTexture(texture_);

				shader_->Bind();

				renderer_->context()->BindVertexArrayObject(vertex_array_object_);

				shader_->Uniform2fv("u_position", position);
				shader_->Uniform4fv("u_color", bar_color_);
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangleStrip, 0, num_bar_vertices_);

				shader_->Uniform2fv("u_position", pin_global_position);
				shader_->Uniform4fv("u_color", (is_touched_) ? pin_color_touch_ : pin_color_normal_);
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangleStrip, num_bar_vertices_, num_pin_vertices_);

				renderer_->context()->BindVertexArrayObject(0);

				shader_->Unbind();
			}
			void SliderColored::BindConstUniforms()
			{
				shader_->Bind();
				shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
				shader_->Unbind();
			}
			void SliderColored::FillVertexAttribs()
			{
				sht::graphics::VertexAttribute attrib(sht::graphics::VertexAttribute::kVertex, 2);
				attribs_.push_back(attrib);
			}
			void SliderColored::FillVertices()
			{
				// const unsigned int kNumPinVertices = 40;
				// const unsigned int kNumVerticesPerSemicircle = 20;
				num_vertices_ = num_bar_vertices_ + num_pin_vertices_;
				vertices_array_ = new u8[num_vertices_ * sizeof(vec2)]; // 4 * 2 * s(float)
				vec2 * vertices = reinterpret_cast<vec2*>(vertices_array_);

				// Create pin vertices
				// unsigned int index = 0;
				// const float kAngle = sht::math::kTwoPi / (float)kNumPinVertices;
				// for (unsigned int i = 0; i < kNumPinVertices; ++i)
				// {
				// 	float current_angle = (float)i * kAngle;
				// 	vertices[index].x = pin_radius_ * cosf(current_angle);
				// 	vertices[index].y = pin_radius_ * sinf(current_angle);
				// 	++index;
				// 	vertices[index].x = pin_radius_ * cosf(current_angle + kAngle);
				// 	vertices[index].y = pin_radius_ * sinf(current_angle + kAngle);
				// 	++index;
				// 	vertices[index].x = 0.0f;
				// 	vertices[index].y = 0.0f;
				// 	++index;
				// }

				// ----- Bar -----
				
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

				// ----- Pin -----
				
				// Lower left
				vertices[4].x = -pin_radius_;
				vertices[4].y = -pin_radius_;
				
				// Lower right
				vertices[5].x = pin_radius_;
				vertices[5].y = -pin_radius_;
				
				// Upper left
				vertices[6].x = -pin_radius_;
				vertices[6].y = pin_radius_;
				
				// Upper right
				vertices[7].x = pin_radius_;
				vertices[7].y = pin_radius_;
			}

		} // namespace ui
	} // namespace utility
} // namespace sht