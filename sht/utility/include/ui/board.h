#pragma once
#ifndef __SHT_UI_BOARD_H__
#define __SHT_UI_BOARD_H__

#include "rect.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Colored board class
			class ColoredBoard : public RectColored {
			public:
				explicit ColoredBoard(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& color, f32 width, f32 height, f32 other_coord, f32 value_min, f32 value_max,
					f32 velocity, bool min_position, bool is_vertical, u32 flags);

				bool IsMoving() const;
				bool IsPosMin() const;
				bool IsPosMax() const;

				void Move();

				virtual void Update(f32 sec) override;

			protected:
				f32 min_value_;
				f32 max_value_;
				f32 velocity_;
				bool is_vertical_;
				bool move_positive_;
				bool move_active_;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif