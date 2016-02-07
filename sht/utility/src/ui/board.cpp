#include "../../include/ui/board.h"

namespace sht {
	namespace utility {
		namespace ui {

            VerticalBoard::VerticalBoard(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                                         const vec4& color, f32 width, f32 height,
					f32 left, f32 hmin, f32 hmax, f32 velocity, bool is_down,
					u32 flags)
			: RectColored(renderer, shader, color, left, (is_down) ? hmin : hmax, width, height, flags)
			, min_height_(hmin)
			, max_height_(hmax)
			, velocity_(velocity)
			, move_up_(is_down)
			, move_active_(false)
			{

			}
			bool VerticalBoard::IsMoving() const
			{
				return move_active_;
			}
			bool VerticalBoard::IsPosDown() const
			{
				return !move_active_ && move_up_;
			}
			bool VerticalBoard::IsPosUp() const
			{
				return !move_active_ && !move_up_;
			}
			void VerticalBoard::Move()
			{
				move_active_ = true;
			}
			void VerticalBoard::Update(f32 sec)
			{
				if (!move_active_)
					return;

				if (move_up_)
				{
					position_.y += velocity_ * sec;
					if (position_.y > max_height_)
					{
						position_.y = max_height_;
						move_up_ = false;
						move_active_ = false;
					}
				}
				else // move down
				{
					position_.y -= velocity_ * sec;
					if (position_.y < min_height_)
					{
						position_.y = min_height_;
						move_up_ = true;
						move_active_ = false;
					}
				}
			}

		} // namespace ui
	} // namespace utility
} // namespace sht