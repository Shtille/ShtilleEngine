#pragma once
#ifndef __SHT_UI_PROGRESS_BAR_H__
#define __SHT_UI_PROGRESS_BAR_H__

#include "rect.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Standart horizontal progress bar class
			class ProgressBar : public RectTextured {
			public:
				explicit ProgressBar(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                    sht::graphics::Texture * texture, f32 progress_max,
                    f32 x, f32 y, f32 width, f32 height, u32 flags);

				void PositionIncrease(f32 value);
				void PositionToBeginning();
				void PositionToEnd();
				
				bool IsDone() const;

			protected:
				f32 progress_;
				f32 progress_end_;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif