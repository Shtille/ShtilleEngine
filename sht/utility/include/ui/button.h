#pragma once
#ifndef __SHT_UI_BUTTON_H__
#define __SHT_UI_BUTTON_H__

#include "rect.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Standart button class
			class Button : public Rect {
			public:
				Button(f32 x, f32 y, f32 width, f32 height, u32 flags);

				bool is_touched() const;

				void OnTouchDown(const vec2& position); //!< position is in global coordinate system
				void OnTouchMove(const vec2& position);
				void OnTouchUp(const vec2& position);
				
			protected:
				bool is_touched_;
			};
			
			//! Colored button class
			class ButtonColored : public Button, public Drawable {
			public:
				ButtonColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					const vec4& normal_color, const vec4& touch_color,
					f32 x, f32 y, f32 width, f32 height, u32 flags);
				
				virtual void Render() override;
				
			protected:
				vec4 normal_color_;
				vec4 touch_color_;

			private:
				virtual void BindConstUniforms() override;
				virtual void FillVertexAttribs() override;
				virtual void FillVertices() override;
			};

			//! Colored button class
			class ButtonTextured : public Button, public Drawable {
			public:
				ButtonTextured(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
					sht::graphics::Texture * normal_texture, sht::graphics::Texture * touch_texture,
					f32 x, f32 y, f32 width, f32 height, u32 flags);
				
				virtual void Render() override;

			protected:
				sht::graphics::Texture * touch_texture_;

			private:
				virtual void BindConstUniforms() override;
				virtual void FillVertexAttribs() override;
				virtual void FillVertices() override;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif