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
				Slider(f32 x, f32 y, f32 width, f32 height, f32 bar_radius, u32 flags);

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
				float bar_radius_;
				float pin_position_;
				float pin_radius_;
				bool is_touched_;
			};

			//! Colored slider class
			class SliderColored : public Slider, public Drawable {
			public:
				enum Form
				{
					kQuad,
					kCircle
				};
				SliderColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& bar_color, const vec4& pin_color_normal, const vec4& pin_color_touch,
					f32 x, f32 y, f32 width, f32 height, f32 bar_radius, u32 flags,
					Form bar_form = kQuad, Form pin_form = kQuad);

				virtual void Render() override;

			protected:
				vec4 bar_color_;
				vec4 pin_color_normal_;
				vec4 pin_color_touch_;
				u32 num_bar_vertices_;
				u32 num_pin_vertices_;
				Form bar_form_;
				Form pin_form_;

			private:
				void BindConstUniforms() override;
				void FillVertexAttribs() override;
				void FillVertices() override;
			};

			//! Textured slider class
			class SliderTextured : public Slider, public Drawable {
			public:
				enum Form
				{
					kQuad,
					kCircle
				};
				SliderTextured(sht::graphics::Renderer * renderer,
					sht::graphics::Shader * color_shader, sht::graphics::Shader * texture_shader,
					sht::graphics::Texture * texture_normal, sht::graphics::Texture * texture_touch,
					const vec4& bar_color, f32 x, f32 y, f32 width, f32 height, f32 bar_radius, u32 flags,
					Form bar_form = kQuad);

				virtual void Render() override;

			protected:
				sht::graphics::Shader * texture_shader_;
				sht::graphics::Texture * texture_touch_;
				vec4 bar_color_;
				u32 num_bar_vertices_;
				u32 num_pin_vertices_;
				Form bar_form_;

			private:
				void BindConstUniforms() override;
				void FillVertexAttribs() override;
				void FillVertices() override;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif