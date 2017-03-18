#include "game_console.h"

#include "thirdparty/script/src/script.h"

static inline void WideToAnsi(const std::wstring& wide_string, std::string& ansi_string)
{
	ansi_string.assign(wide_string.begin(), wide_string.end());
}
static inline void AnsiToWide(const std::string& ansi_string, std::wstring& wide_string)
{
	wide_string.assign(ansi_string.begin(), ansi_string.end());
}

GameConsole::GameConsole(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
            sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
            f32 bottom, f32 text_height, f32 velocity, f32 aspect_ratio)
: Console(renderer, font, gui_shader, text_shader, bottom, text_height, velocity, aspect_ratio)
, parser_(nullptr)
{

}
GameConsole::~GameConsole()
{

}
void GameConsole::set_parser(console_script::Parser * parser)
{
	parser_ = parser;
}
void GameConsole::RecognizeString()
{
	if (parser_)
	{
		// TODO: add normal transforms here

		// Transform input from wide to ansi
		std::string result_string;
		std::string ansi_string;
		std::wstring wide_string;
		WideToAnsi(input_string(), ansi_string);
		if (parser_->Evaluate(ansi_string, &result_string))
		{
			// Print result string
			AnsiToWide(result_string, wide_string);
			AddString(wide_string);
		}
		else
		{
			// Transform error message from ansi to wide
			AnsiToWide(parser_->error(), wide_string);
			AddString(wide_string);
		}
	}
}