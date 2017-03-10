#include "game_console.h"

#include "thirdparty/script/src/script.h"

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
		if (parser_->Compile(input_string()))
		{
			parser_->Execute();
		}
		else
			AddString(parser_->error());
	}
}