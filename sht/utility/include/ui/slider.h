#pragma once
#ifndef __SHT_UI_SLIDER_H__
#define __SHT_UI_SLIDER_H__

#include "widget.h"
#include "drawable.h"

namespace sht {
	namespace utility {
		namespace ui {

			/*! Standart slider class
			Radius of slider sides is equal to the half of height/width.
			*/
			class Slider : public Widget {
			public:
				Slider(f32 x, f32 y, f32 width, f32 height, u32 flags);

				void SetPinPosition(f32 pos);

				void OnTouchDown(const vec2& position); //!< position is in global coordinate system
				void OnTouchMove(const vec2& position);
				void OnTouchUp(const vec2& position);

				f32 width() const;
				f32 height() const;
				float pin_position() const;
				float pin_radius() const;
				bool is_vertical() const;
				bool is_touched() const;

			protected:
				bool IsInsidePin(const vec2& global_position);

				f32 width_;
				f32 height_;
				vec2 old_position_;
				float radius_;
				float pin_position_;
				float pin_radius_;
				bool is_touched_;
			};

			//! Colored slider class
			class SliderColored : public Slider, public Drawable {
			public:
				SliderColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& bar_color, const vec4& pin_color_normal, const vec4& pin_color_touch,
					f32 x, f32 y, f32 width, f32 height, u32 flags);

				virtual void Render() override;

			protected:
				vec4 bar_color_;
				vec4 pin_color_normal_;
				vec4 pin_color_touch_;
				const u32 num_bar_vertices_;
				const u32 num_pin_vertices_;

			private:
				void BindConstUniforms() override;
				void FillVertexAttribs() override;
				void FillVertices() override;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif