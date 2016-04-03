#include "../include/console.h"

namespace sht {
	namespace utility {

        Console::Console(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
                         sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
                         f32 bottom, f32 text_height, f32 velocity, f32 aspect_ratio)
        : vertical_board_(renderer, gui_shader, vec4(0.2f, 0.2f, 0.2f, 0.8f), aspect_ratio, 1.0f-bottom, 0.0f, bottom, 1.0f, velocity, false, (u32)ui::Flags::kRenderAlways)
		, text_height_(text_height)
        , labels_(nullptr)
        , need_to_update_input_(false)
        , need_to_update_all_(false)
		{
            max_lines_ = static_cast<u32>(vertical_board_.height()/text_height + 0.5f);
            assert(max_lines_ > 0);
            labels_ = new ui::Label*[max_lines_];
            for (u32 i = 0; i < max_lines_; ++i)
            {
                lines_.push_back(L"");
                labels_[i] = new ui::Label(renderer, text_shader, font, vec4(1.0f), text_height, 256,
                                                   0.0f, text_height*i, (u32)ui::Flags::kRenderAlways);
                // Attach all labels to board widget
                vertical_board_.AttachWidget(labels_[i]);
            }
		}
		Console::~Console()
		{
			delete[] labels_;
		}
        bool Console::IsActive()
        {
            return vertical_board_.IsPosDown();
        }
		void Console::Move()
		{
            vertical_board_.Move();
		}
        void Console::AddString(const wchar_t* text)
        {
            for (u32 i = 0; i < max_lines_-2; ++i)
            {
                u32 index = max_lines_ - 1 - i;
                u32 index_next = index - 1;
                lines_[index] = lines_[index_next];
            }
            lines_[1] = text;
            need_to_update_all_ = true;
        }
        void Console::Update(f32 sec)
        {
            if (need_to_update_all_)
            {
                for (u32 i = 0; i < max_lines_; ++i)
                {
                    labels_[i]->SetText(lines_[i].c_str());
                }
                need_to_update_all_ = false;
            }
            else if (need_to_update_input_)
            {
                labels_[0]->SetText(input_string().c_str());
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
        std::wstring& Console::input_string()
        {
            return lines_[0];
        }
        void Console::InsertSymbol(wchar_t symbol)
        {
            input_string().push_back(symbol);
            need_to_update_input_ = true;
        }
        void Console::RemoveSymbol()
        {
            if (!input_string().empty())
            {
                input_string().pop_back();
                need_to_update_input_ = true;
            }
        }
        void Console::PushString()
        {
            RecognizeString();
            InsertString();
        }
        void Console::InsertString()
        {
            for (u32 i = 0; i < max_lines_-1; ++i)
            {
                u32 index = max_lines_ - 1 - i;
                u32 index_next = index - 1;
                lines_[index] = lines_[index_next];
            }
            lines_[0].clear();
            need_to_update_all_ = true;
        }
        void Console::RecognizeString()
        {
            // Need to add some script engine here
            // input_string is the one to recognize
        }

	} // namespace utility
} // namespace sht