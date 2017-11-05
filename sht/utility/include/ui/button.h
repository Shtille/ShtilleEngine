#pragma once
#ifndef __SHT_UI_BUTTON_H__
#define __SHT_UI_BUTTON_H__

#include "rect.h"

namespace sht {
	namespace utility {
		namespace ui {
			
			//! Colored button class
			class ButtonColored : public RectColored {
			public:
				ButtonColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& normal_color, const vec4& touch_color,
					f32 x, f32 y, f32 width, f32 height, u32 flags);

				bool is_touched() const;
				
				virtual void Render() override;

				void OnTouchDown(const vec2& position); //!< position is in global coordinate system
				void OnTouchMove(const vec2& position);
				void OnTouchUp(const vec2& position);
				
			protected:
				vec4 touch_color_;
				bool is_touched_;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif