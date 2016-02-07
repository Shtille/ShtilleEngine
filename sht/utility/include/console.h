#pragma once
#ifndef __SHT_UTILITY_CONSOLE_H__
#define __SHT_UTILITY_CONSOLE_H__

#include "ui/board.h"
#include "../include/ui/label.h"
#include "../../system/include/keys.h"

#include <string>
#include <vector>

namespace sht {
	namespace utility {

		//! Standart console class
		// Has initial state at top of the window (lower bound at top)
        // Supports only english charaters in input
        class Console {
		public:
            //! Requires gui colored shader to render
            Console(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
                    sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
                    f32 bottom, f32 text_height, f32 velocity, f32 aspect_ratio);
			~Console();

            bool IsActive(); //!< down position
			void Move();
            
            void Update(f32 sec);
			void Render();
            
            void ProcessKeyInput(sht::PublicKey key, int mods);
            void ProcessCharInput(unsigned short code);

		private:
            Console(const Console&) = delete;
            Console& operator =(const Console&) = delete;
            
            void InsertSymbol(wchar_t symbol);
            void RemoveSymbol();
            void PushString();
            
            ui::VerticalBoard vertical_board_;
            ui::Label * input_text_; // pointer, dont need to delete
			f32 text_height_; //!< height of text in screen coordinates
            u32 max_lines_; //!< maximum number of lines
			std::vector<std::string> lines_; //!< lines of text
            std::wstring input_string_;
            bool need_to_update_input_;
		};

	} // namespace utility
} // namespace sht

#endif