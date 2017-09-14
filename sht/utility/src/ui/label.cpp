#include "../../include/ui/label.h"

namespace sht {
	namespace utility {
		namespace ui {

            Label::Label(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
                         sht::graphics::Font * font, const vec4& color, f32 text_height, u32 buffer_size,
                         f32 x, f32 y, u32 flags)
            : Widget(x, y, flags)
            , shader_(shader)
            , font_(font)
            , color_(color)
            , text_height_(text_height)
            {
                BindConstUniforms();
                text_ = sht::graphics::DynamicText::Create(renderer, buffer_size);
                assert(text_);
                SetText(L""); // just to be able to render the text
            }
			Label::~Label()
			{
				delete text_;
			}
            void Label::Render()
            {
                shader_->Bind();
                shader_->Uniform4fv("u_color", color_);
                text_->Render();
                shader_->Unbind();
            }
            void Label::AppendSymbol(wchar_t symbol)
            {
                text_->AppendSymbol(symbol);
            }
            void Label::SetText(const wchar_t* text)
            {
                vec2 position;
                ObtainGlobalPosition(&position);
                text_->SetTextSimple(font_, position.x, position.y, text_height_, text);
            }
            void Label::AlignCenter(f32 rect_width, f32 rect_height)
            {
                if (parent_ == nullptr) return;

                vec2 parent_position;
                parent_->ObtainGlobalPosition(&parent_position);

                float min_x, min_y, max_x, max_y;
                text_->GetTextBoundingBox(&min_x, &min_y, &max_x, &max_y);
                vec2 position;
                position.x = parent_position.x + 0.5f * rect_width - 0.5f * (max_x - min_x);
                position.y = parent_position.y + 0.5f * rect_height - 0.5f * (max_y - min_y);
                text_->SetPosition(position);
            }
            void Label::BindConstUniforms()
            {
                shader_->Bind();
                shader_->Uniform1i("u_texture", 0);
                shader_->Unbind();
            }

		} // namespace ui
	} // namespace utility
} // namespace sht