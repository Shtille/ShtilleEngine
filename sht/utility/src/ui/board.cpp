#include "../../include/ui/board.h"

namespace sht {
	namespace utility {
		namespace ui {

			ColoredBoard::ColoredBoard(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& color, f32 width, f32 height, f32 other_coord, f32 value_min, f32 value_max,
					f32 velocity, bool min_position, bool is_vertical, u32 flags)
			: RectColored(renderer, shader, color,
				(is_vertical) ? (other_coord) : (min_position) ? value_min : value_max,
				(!is_vertical) ? (other_coord) : (min_position) ? value_min : value_max,
				width, height, flags)
			, min_value_(value_min)
			, max_value_(value_max)
			, velocity_(velocity)
			, is_vertical_(is_vertical)
			, move_positive_(min_position)
			, move_active_(false)
			{

			}
			bool ColoredBoard::IsMoving() const
			{
				return move_active_;
			}
			bool ColoredBoard::IsPosMin() const
			{
				return !move_active_ && move_positive_;
			}
			bool ColoredBoard::IsPosMax() const
			{
				return !move_active_ && !move_positive_;
			}
			void ColoredBoard::Move()
			{
				move_active_ = true;
			}
			void ColoredBoard::Update(f32 sec)
			{
				if (!move_active_)
					return;

				if (move_positive_)
				{
					if (is_vertical_)
					{
						position_.y += velocity_ * sec;
						if (position_.y > max_value_)
						{
							position_.y = max_value_;
							move_positive_ = false;
							move_active_ = false;
						}
					}
					else // horizontal
					{
						position_.x += velocity_ * sec;
						if (position_.x > max_value_)
						{
							position_.x = max_value_;
							move_positive_ = false;
							move_active_ = false;
						}
					}
				}
				else // move negative
				{
					if (is_vertical_)
					{
						position_.y -= velocity_ * sec;
						if (position_.y < min_value_)
						{
							position_.y = min_value_;
							move_positive_ = true;
							move_active_ = false;
						}
					}
					else // horizontal
					{
						position_.x -= velocity_ * sec;
						if (position_.x < min_value_)
						{
							position_.x = min_value_;
							move_positive_ = true;
							move_active_ = false;
						}
					}
				}
			}

		} // namespace ui
	} // namespace utility
} // namespace sht