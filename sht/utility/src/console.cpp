#include "../include/console.h"

namespace sht {
	namespace utility {

        Console::Console(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
                         sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
                         f32 bottom, f32 text_height, f32 velocity, f32 aspect_ratio)
        : vertical_board_(renderer, gui_shader, vec4(0.2f, 0.2f, 0.2f, 0.8f), aspect_ratio, 1.0f-bottom, 0.0f, bottom, 1.0f, velocity, false, (u32)ui::Flags::kRenderAlways)
		, text_height_(text_height)
        , need_to_update_input_(false)
		{
            input_text_ = new ui::Label(renderer, text_shader, font, vec4(1.0f), text_height, 256,
                                                   0.0f, 0.0f, (u32)ui::Flags::kRenderAlways);
            vertical_board_.AttachWidget(input_text_);
            max_lines_ = static_cast<u32>(vertical_board_.height()/text_height + 0.5f);
            assert(max_lines_ > 0);
		}
		Console::~Console()
		{
		}
        bool Console::IsActive()
        {
            return vertical_board_.IsPosDown();
        }
		void Console::Move()
		{
            vertical_board_.Move();
		}
        void Console::Update(f32 sec)
        {
            if (need_to_update_input_)
            {
                input_text_->SetText(input_string_.c_str());
                need_to_update_input_ = false;
            }
            vertical_board_.UpdateAll(sec);
        }
		void Console::Render()
		{
            if (vertical_board_.IsPosDown())
                vertical_board_.RenderAll(); // render entire tree
            else
                vertical_board_.Render(); // render only console rect (smart hack for labels :D)
		}
        void Console::ProcessKeyInput(sht::PublicKey key, int mods)
        {
            if ((key == sht::PublicKey::kGraveAccent) && !(mods & sht::ModifierKey::kShift))
            {
                Move();
            }
            else if (key == sht::PublicKey::kBackspace)
            {
                RemoveSymbol();
            }
            else if (key == sht::PublicKey::kEnter)
            {
                PushString();
            }
        }
        void Console::ProcessCharInput(unsigned short code)
        {
            wchar_t symbol = static_cast<wchar_t>(code);
            InsertSymbol(symbol);
        }
        void Console::InsertSymbol(wchar_t symbol)
        {
            input_string_.push_back(symbol);
            need_to_update_input_ = true;
        }
        void Console::RemoveSymbol()
        {
            if (!input_string_.empty())
            {
                input_string_.pop_back();
                need_to_update_input_ = true;
            }
        }
        void Console::PushString()
        {
            
        }

	} // namespace utility
} // namespace sht