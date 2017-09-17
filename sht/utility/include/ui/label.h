#pragma once
#ifndef __SHT_UI_LABEL_H__
#define __SHT_UI_LABEL_H__

#include "widget.h"
#include "../../../graphics/include/renderer/renderer.h"
#include "../../../graphics/include/renderer/text.h"

namespace sht {
	namespace utility {
		namespace ui {

			//! Label class, holds DynamicText object
			class Label : public Widget {
			public:
                Label(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                      sht::graphics::Font * font, const vec4& color, f32 text_height, u32 buffer_size,
                      f32 x, f32 y, u32 flags);
				virtual ~Label();

                void set_constant_position(bool use);
                
                void Update(f32 sec) override;
                void Render() override;
                
                void AppendSymbol(wchar_t symbol);
                void SetText(const wchar_t* text);

                void AlignCenter(f32 rect_width, f32 rect_height);

			protected:
                sht::graphics::Shader * shader_;
                sht::graphics::Font * font_;
                vec4 color_;
                sht::graphics::DynamicText * text_;
                f32 text_height_;
                bool constant_position_; //!< used for optimization
                
            private:
                void BindConstUniforms() override;
			};

		} // namespace ui
	} // namespace utility
} // namespace sht

#endif