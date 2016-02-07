#pragma once
#ifndef __SHT_UI_RECT_H__
#define __SHT_UI_RECT_H__

#include "drawable_widget.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Standart rectangle class
			class Rect : public DrawableWidget {
			public:
                Rect(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                     sht::graphics::Texture * texture,
                     f32 x, f32 y, f32 width, f32 height, u32 flags);

				f32 width() const;
				f32 height() const;

				void set_width(f32 width);
				void set_height(f32 height);

				bool IsInside(f32 x, f32 y);

			protected:
				f32 width_;
				f32 height_;
                
            private:
                void FillVertexAttribs() override;
                void FillVertices() override;
			};
            
            //! Colored rectangle class
            class RectColored : public Rect {
            public:
                RectColored(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader, const vec4& color,
                     f32 x, f32 y, f32 width, f32 height, u32 flags);
                
                virtual void Render() override;
                
            protected:
                vec4 color_;
                
            private:
                void BindConstUniforms() override;
            };

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif