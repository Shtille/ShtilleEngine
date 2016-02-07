#pragma once
#ifndef __SHT_UI_LABEL_H__
#define __SHT_UI_LABEL_H__

#include "widget.h"
#include "../../../graphics/include/renderer/renderer.h"
#include "../../../graphics/include/renderer/text.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Standart rectangle class
			class Label : public Widget {
			public:
                Label(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                      sht::graphics::Font * font, const vec4& color, f32 text_height, u32 buffer_size,
                      f32 x, f32 y, u32 flags);
                
                void Render() override;
                
                void AppendSymbol(wchar_t symbol);
                void SetText(const wchar_t* text);

			protected:
                sht::graphics::Shader * shader_;
                sht::graphics::Font * font_;
                vec4 color_;
                sht::graphics::DynamicText * text_;
                f32 text_height_;
                
            private:
                void BindConstUniforms() override;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif