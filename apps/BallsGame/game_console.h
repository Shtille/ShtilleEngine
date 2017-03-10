#pragma once
#ifndef __GAME_CONSOLE_H__
#define __GAME_CONSOLE_H__

#include "utility/include/console.h"

namespace console_script {
	class Parser;
}

//! Game console requires console_script library, thus i've moved it into separate module
class GameConsole : public sht::utility::Console {
public:
	GameConsole(sht::graphics::Renderer * renderer, sht::graphics::Font * font,
                    sht::graphics::Shader * gui_shader, sht::graphics::Shader * text_shader,
                    f32 bottom, f32 text_height, f32 velocity, f32 aspect_ratio);
	~GameConsole();

	void set_parser(console_script::Parser * parser);

protected:
	void RecognizeString() override;

private:
	console_script::Parser * parser_;
};

#endif