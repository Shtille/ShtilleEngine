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
				 f32 x, f32 y, f32 width, f32 height, u32 flags,
				 Form bar_form, Form pin_form)
			: Slider(x, y, width, height, flags)
			, Drawable(renderer, shader, nullptr)
			, bar_color_(bar_color)
			, pin_color_normal_(pin_color_normal)
			, pin_color_touch_(pin_color_touch)
			, num_bar_vertices_(4)
			, num_pin_vertices_(4)
			, bar_form_(bar_form)
			, pin_form_(pin_form)
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
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangles, 0, num_bar_vertices_);

				shader_->Uniform2fv("u_position", pin_global_position);
				shader_->Uniform4fv("u_color", (is_touched_) ? pin_color_touch_ : pin_color_normal_);
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangles, num_bar_vertices_, num_pin_vertices_);

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
				const unsigned int kNumPinVertices = 40;
				const unsigned int kNumVerticesPerSemicircle = 20;

				if (bar_form_ == kQuad)
					num_bar_vertices_ = 6;
				else if (bar_form_ == kCircle)
					num_bar_vertices_ = 6 + kNumVerticesPerSemicircle * 6;
				if (pin_form_ == kQuad)
					num_pin_vertices_ = 6;
				else if (pin_form_ == kCircle)
					num_pin_vertices_ = kNumPinVertices * 3;
				num_vertices_ = num_bar_vertices_ + num_pin_vertices_;
				vertices_array_ = new u8[num_vertices_ * sizeof(vec2)]; // 4 * 2 * s(float)
				vec2 * vertices = reinterpret_cast<vec2*>(vertices_array_);

				// ----- Bar -----
				unsigned int index = 0;
				
				if (bar_form_ == kQuad)
				{
					// Lower left
					vertices[index].x = 0.0f;
					vertices[index].y = 0.0f;
					++index;
					
					// Lower right
					vertices[index].x = width_;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper left
					vertices[index].x = 0.0f;
					vertices[index].y = height_;
					++index;

					// Upper left
					vertices[index].x = 0.0f;
					vertices[index].y = height_;
					++index;

					// Lower right
					vertices[index].x = width_;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper right
					vertices[index].x = width_;
					vertices[index].y = height_;
					++index;
				}
				else if (bar_form_ == kCircle)
				{
					const float kAngle = sht::math::kPi / (float)kNumVerticesPerSemicircle;
					if (is_vertical())
					{
						// Lower left
						vertices[index].x = 0.0f;
						vertices[index].y = radius_;
						++index;
						
						// Lower right
						vertices[index].x = width_;
						vertices[index].y = radius_;
						++index;
						
						// Upper left
						vertices[index].x = 0.0f;
						vertices[index].y = height_ - radius_;
						++index;

						// Upper left
						vertices[index].x = 0.0f;
						vertices[index].y = height_ - radius_;
						++index;

						// Lower right
						vertices[index].x = width_;
						vertices[index].y = radius_;
						++index;
						
						// Upper right
						vertices[index].x = width_;
						vertices[index].y = height_ - radius_;
						++index;

						// Left semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = 0.5f * width_;
							vertices[index].y = radius_;
							++index;
							vertices[index].x = 0.5f * width_ - pin_radius_ * cosf(current_angle);
							vertices[index].y = radius_       - pin_radius_ * sinf(current_angle);
							++index;
							vertices[index].x = 0.5f * width_ - pin_radius_ * cosf(current_angle + kAngle);
							vertices[index].y = radius_       - pin_radius_ * sinf(current_angle + kAngle);
							++index;
						}

						// Right semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = 0.5f * width_;
							vertices[index].y = height_ - radius_;
							++index;
							vertices[index].x = 0.5f * width_     + pin_radius_ * cosf(current_angle);
							vertices[index].y = height_ - radius_ + pin_radius_ * sinf(current_angle);
							++index;
							vertices[index].x = 0.5f * width_     + pin_radius_ * cosf(current_angle + kAngle);
							vertices[index].y = height_ - radius_ + pin_radius_ * sinf(current_angle + kAngle);
							++index;
						}
					}
					else // horizontal
					{
						// Lower left
						vertices[index].x = radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Lower right
						vertices[index].x = width_ - radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Upper left
						vertices[index].x = radius_;
						vertices[index].y = height_;
						++index;

						// Upper left
						vertices[index].x = radius_;
						vertices[index].y = height_;
						++index;

						// Lower right
						vertices[index].x = width_ - radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Upper right
						vertices[index].x = width_ - radius_;
						vertices[index].y = height_;
						++index;

						// Left semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = radius_;
							vertices[index].y = 0.5f * height_;
							++index;
							vertices[index].x = radius_        - pin_radius_ * sinf(current_angle);
							vertices[index].y = 0.5f * height_ + pin_radius_ * cosf(current_angle);
							++index;
							vertices[index].x = radius_        - pin_radius_ * sinf(current_angle + kAngle);
							vertices[index].y = 0.5f * height_ + pin_radius_ * cosf(current_angle + kAngle);
							++index;
						}

						// Right semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = width_ - radius_;
							vertices[index].y = 0.5f * height_;
							++index;
							vertices[index].x = width_ - radius_ + pin_radius_ * sinf(current_angle);
							vertices[index].y = 0.5f * height_   - pin_radius_ * cosf(current_angle);
							++index;
							vertices[index].x = width_ - radius_ + pin_radius_ * sinf(current_angle + kAngle);
							vertices[index].y = 0.5f * height_   - pin_radius_ * cosf(current_angle + kAngle);
							++index;
						}
					}
				}

				// ----- Pin -----
				if (pin_form_ == kQuad)
				{
					// Lower left
					vertices[index].x = -pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					
					// Lower right
					vertices[index].x = pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					
					// Upper left
					vertices[index].x = -pin_radius_;
					vertices[index].y = pin_radius_;
					++index;

					// Upper left
					vertices[index].x = -pin_radius_;
					vertices[index].y = pin_radius_;
					++index;

					// Lower right
					vertices[index].x = pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					
					// Upper right
					vertices[index].x = pin_radius_;
					vertices[index].y = pin_radius_;
					++index;
				}
				else if (pin_form_ == kCircle)
				{
					const float kAngle = sht::math::kTwoPi / (float)kNumPinVertices;
					for (unsigned int i = 0; i < kNumPinVertices; ++i)
					{
						float current_angle = (float)i * kAngle;
						vertices[index].x = 0.0f;
						vertices[index].y = 0.0f;
						++index;
						vertices[index].x = pin_radius_ * cosf(current_angle);
						vertices[index].y = pin_radius_ * sinf(current_angle);
						++index;
						vertices[index].x = pin_radius_ * cosf(current_angle + kAngle);
						vertices[index].y = pin_radius_ * sinf(current_angle + kAngle);
						++index;
					}
				}
			}

			SliderTextured::SliderTextured(sht::graphics::Renderer * renderer,
				sht::graphics::Shader * color_shader, sht::graphics::Shader * texture_shader,
				sht::graphics::Texture * texture_normal, sht::graphics::Texture * texture_touch,
				const vec4& bar_color, f32 x, f32 y, f32 width, f32 height, u32 flags,
				Form bar_form)
			: Slider(x, y, width, height, flags)
			, Drawable(renderer, color_shader, texture_normal)
			, texture_shader_(texture_shader)
			, texture_touch_(texture_touch)
			, bar_color_(bar_color)
			, num_bar_vertices_(4)
			, num_pin_vertices_(4)
			, bar_form_(bar_form)
			{
				FillVertexAttribs();
				FillVertices();
				MakeRenderable();
				BindConstUniforms();
			}
			void SliderTextured::Render()
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

				renderer_->ChangeTexture(nullptr);

				renderer_->context()->BindVertexArrayObject(vertex_array_object_);

				shader_->Bind();
				shader_->Uniform2fv("u_position", position);
				shader_->Uniform4fv("u_color", bar_color_);
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangles, 0, num_bar_vertices_);

				renderer_->ChangeTexture((is_touched_) ? texture_touch_ : texture_);

				texture_shader_->Bind();
				texture_shader_->Uniform2fv("u_position", pin_global_position);
				renderer_->context()->DrawArrays(sht::graphics::PrimitiveType::kTriangles, num_bar_vertices_, num_pin_vertices_);

				renderer_->context()->BindVertexArrayObject(0);

				renderer_->ChangeTexture(nullptr);

				texture_shader_->Unbind();
			}
			void SliderTextured::BindConstUniforms()
			{
				shader_->Bind();
				shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());

				texture_shader_->Bind();
				texture_shader_->Uniform1f("u_aspect_ratio", renderer_->aspect_ratio());
				texture_shader_->Uniform1i("u_texture", 0);
				texture_shader_->Unbind();
			}
			void SliderTextured::FillVertexAttribs()
			{
				sht::graphics::VertexAttribute attrib(sht::graphics::VertexAttribute::kVertex, 2);
				attribs_.push_back(attrib);
			}
			void SliderTextured::FillVertices()
			{
				const unsigned int kNumVerticesPerSemicircle = 20;

				if (bar_form_ == kQuad)
					num_bar_vertices_ = 6;
				else if (bar_form_ == kCircle)
					num_bar_vertices_ = 6 + kNumVerticesPerSemicircle * 6;
				num_pin_vertices_ = 6;
				num_vertices_ = num_bar_vertices_ + num_pin_vertices_ * 2; // because pin uses vec4 instead of vec2
				vertices_array_ = new u8[num_vertices_ * sizeof(vec2)]; // 4 * 2 * s(float)
				vec2 * vertices = reinterpret_cast<vec2*>(vertices_array_);

				// ----- Bar -----
				unsigned int index = 0;
				
				if (bar_form_ == kQuad)
				{
					// Lower left
					vertices[index].x = 0.0f;
					vertices[index].y = 0.0f;
					++index;
					
					// Lower right
					vertices[index].x = width_;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper left
					vertices[index].x = 0.0f;
					vertices[index].y = height_;
					++index;

					// Upper left
					vertices[index].x = 0.0f;
					vertices[index].y = height_;
					++index;

					// Lower right
					vertices[index].x = width_;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper right
					vertices[index].x = width_;
					vertices[index].y = height_;
					++index;
				}
				else if (bar_form_ == kCircle)
				{
					const float kAngle = sht::math::kPi / (float)kNumVerticesPerSemicircle;
					if (is_vertical())
					{
						// Lower left
						vertices[index].x = 0.0f;
						vertices[index].y = radius_;
						++index;
						
						// Lower right
						vertices[index].x = width_;
						vertices[index].y = radius_;
						++index;
						
						// Upper left
						vertices[index].x = 0.0f;
						vertices[index].y = height_ - radius_;
						++index;

						// Upper left
						vertices[index].x = 0.0f;
						vertices[index].y = height_ - radius_;
						++index;

						// Lower right
						vertices[index].x = width_;
						vertices[index].y = radius_;
						++index;
						
						// Upper right
						vertices[index].x = width_;
						vertices[index].y = height_ - radius_;
						++index;

						// Left semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = 0.5f * width_;
							vertices[index].y = radius_;
							++index;
							vertices[index].x = 0.5f * width_ - pin_radius_ * cosf(current_angle);
							vertices[index].y = radius_       - pin_radius_ * sinf(current_angle);
							++index;
							vertices[index].x = 0.5f * width_ - pin_radius_ * cosf(current_angle + kAngle);
							vertices[index].y = radius_       - pin_radius_ * sinf(current_angle + kAngle);
							++index;
						}

						// Right semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = 0.5f * width_;
							vertices[index].y = height_ - radius_;
							++index;
							vertices[index].x = 0.5f * width_     + pin_radius_ * cosf(current_angle);
							vertices[index].y = height_ - radius_ + pin_radius_ * sinf(current_angle);
							++index;
							vertices[index].x = 0.5f * width_     + pin_radius_ * cosf(current_angle + kAngle);
							vertices[index].y = height_ - radius_ + pin_radius_ * sinf(current_angle + kAngle);
							++index;
						}
					}
					else // horizontal
					{
						// Lower left
						vertices[index].x = radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Lower right
						vertices[index].x = width_ - radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Upper left
						vertices[index].x = radius_;
						vertices[index].y = height_;
						++index;

						// Upper left
						vertices[index].x = radius_;
						vertices[index].y = height_;
						++index;

						// Lower right
						vertices[index].x = width_ - radius_;
						vertices[index].y = 0.0f;
						++index;
						
						// Upper right
						vertices[index].x = width_ - radius_;
						vertices[index].y = height_;
						++index;

						// Left semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = radius_;
							vertices[index].y = 0.5f * height_;
							++index;
							vertices[index].x = radius_        - pin_radius_ * sinf(current_angle);
							vertices[index].y = 0.5f * height_ + pin_radius_ * cosf(current_angle);
							++index;
							vertices[index].x = radius_        - pin_radius_ * sinf(current_angle + kAngle);
							vertices[index].y = 0.5f * height_ + pin_radius_ * cosf(current_angle + kAngle);
							++index;
						}

						// Right semicircle
						for (unsigned int i = 0; i < kNumVerticesPerSemicircle; ++i)
						{
							float current_angle = (float)i * kAngle;
							vertices[index].x = width_ - radius_;
							vertices[index].y = 0.5f * height_;
							++index;
							vertices[index].x = width_ - radius_ + pin_radius_ * sinf(current_angle);
							vertices[index].y = 0.5f * height_   - pin_radius_ * cosf(current_angle);
							++index;
							vertices[index].x = width_ - radius_ + pin_radius_ * sinf(current_angle + kAngle);
							vertices[index].y = 0.5f * height_   - pin_radius_ * cosf(current_angle + kAngle);
							++index;
						}
					}
				}

				// ----- Pin -----
				{
					// Lower left
					vertices[index].x = -pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					vertices[index].x = 0.0f;
					vertices[index].y = 0.0f;
					++index;
					
					// Lower right
					vertices[index].x = pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					vertices[index].x = 1.0f;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper left
					vertices[index].x = -pin_radius_;
					vertices[index].y = pin_radius_;
					++index;
					vertices[index].x = 0.0f;
					vertices[index].y = 1.0f;
					++index;

					// Upper left
					vertices[index].x = -pin_radius_;
					vertices[index].y = pin_radius_;
					++index;
					vertices[index].x = 0.0f;
					vertices[index].y = 1.0f;
					++index;

					// Lower right
					vertices[index].x = pin_radius_;
					vertices[index].y = -pin_radius_;
					++index;
					vertices[index].x = 1.0f;
					vertices[index].y = 0.0f;
					++index;
					
					// Upper right
					vertices[index].x = pin_radius_;
					vertices[index].y = pin_radius_;
					++index;
					vertices[index].x = 1.0f;
					vertices[index].y = 1.0f;
					++index;
				}
			}

		} // namespace ui
	} // namespace utility
} // namespace sht