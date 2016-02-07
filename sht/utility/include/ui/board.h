#pragma once
#ifndef __SHT_UI_BOARD_H__
#define __SHT_UI_BOARD_H__

#include "rect.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Standart board class
			class VerticalBoard : public RectColored {
			public:
				explicit VerticalBoard(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                    const vec4& color,
                    f32 width, f32 height, f32 left, f32 hmin, f32 hmax, f32 velocity, bool is_down,
					u32 flags);

				bool IsMoving() const;
				bool IsPosDown() const;
				bool IsPosUp() const;

				void Move();

				virtual void Update(f32 sec) override;

			protected:
				f32 min_height_;
				f32 max_height_;
				f32 velocity_;
				bool move_up_;
				bool move_active_;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif