#pragma once
#ifndef __SHT_UTILITY_CONSOLE_H__
#define __SHT_UTILITY_CONSOLE_H__

#include "ui/board.h"
#include "../include/ui/label.h"
#include "../../system/include/keys.h"

#include <string>
#include <vector>
#include <mutex>

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
            void AddString(const wchar_t* text);
            
            void Update(f32 sec);
			void Render();
            
            void ProcessKeyInput(sht::PublicKey key, int mods);
            void ProcessCharInput(unsigned short code);

		private:
            Console(const Console&) = delete;
            Console& operator =(const Console&) = delete;
            
            std::wstring& input_string();
            
            void InsertSymbol(wchar_t symbol);
            void RemoveSymbol();
            void PushString();
            void InsertString();
            void RecognizeString();
            
            ui::VerticalBoard vertical_board_;
			f32 text_height_; //!< height of text in screen coordinates
            u32 max_lines_; //!< maximum number of lines
            ui::Label * * labels_; // pointers to labels
            std::vector<std::wstring> lines_;
            std::mutex mutex_;
            bool need_to_update_input_;
            bool need_to_update_all_;
		};

	} // namespace utility
} // namespace sht

#endif