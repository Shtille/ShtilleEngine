#include "../../include/ui/button.h"

namespace sht {
	namespace utility {
		namespace ui {

			ButtonColored::ButtonColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
				const vec4& normal_color, const vec4& touch_color,
				f32 x, f32 y, f32 width, f32 height, u32 flags)
			: RectColored(renderer, shader, normal_color, x, y, width, height, flags)
			, touch_color_(touch_color)
			, is_touched_(false)
			{
			}
			bool ButtonColored::is_touched() const
			{
				return is_touched_;
			}
			void ButtonColored::Render()
			{
				vec2 position;
				ObtainGlobalPosition(&position);
				shader_->Bind();
				shader_->Uniform2fv("u_position", position);
				shader_->Uniform4fv("u_color", (is_touched_) ? touch_color_ : color_);
				DrawableWidget::Render();
				shader_->Unbind();
			}
			void ButtonColored::OnTouchDown(const vec2& position)
			{
				if (IsInsideGlobal(position.x, position.y))
				{
					is_touched_ = true;
				}
			}
			void ButtonColored::OnTouchMove(const vec2& position)
			{
				if (is_touched_)
				{
					// If gesture has gone away from the pin disable the pin
					if (!IsInsideGlobal(position.x, position.y))
					{
						is_touched_ = false;
						return;
					}
				}
			}
			void ButtonColored::OnTouchUp(const vec2& position)
			{
				is_touched_ = false;
			}

		} // namespace ui
	} // namespace utility
} // namespace sht