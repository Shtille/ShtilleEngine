#include "../../include/ui/progress_bar.h"

namespace sht {
	namespace utility {
		namespace ui {

            ProgressBar::ProgressBar(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                    sht::graphics::Texture * texture, f32 progress_max,
                    f32 x, f32 y, f32 width, f32 height, u32 flags)
            : RectTextured(renderer, shader, texture, x, y, width, height, flags)
            , progress_(0.0f)
            , progress_end_(progress_max)
            {
            }
			void ProgressBar::PositionIncrease(f32 value)
			{
				progress_ += value;
			}
			void ProgressBar::PositionToBeginning()
			{
				progress_ = 0.0f;
			}
			void ProgressBar::PositionToEnd()
			{
				progress_ = progress_end_;
			}
			bool ProgressBar::IsDone() const
			{
				return progress_ >= progress_end_;
			}

		} // namespace ui
	} // namespace utility
} // namespace sht