#pragma once
#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "game_settings.h"

#include "utility/include/scene/scene.h"
#include "utility/include/event_listener.h"
#include "utility/include/ui/board.h"
#include "utility/include/ui/rect.h"
#include "utility/include/ui/label.h"

#include "graphics/include/renderer/text.h"

class MenuScene : public sht::utility::Scene {
public:
	MenuScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener,
		GameSettings * game_settings);
	virtual ~MenuScene();

	void Update();
	void Render();

	void Load();
	void Unload();

	void OnMouseDown(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseUp(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseMove(float x, float y) final;

private:
	void CreateMenu();
	void UpdatePlayersLabel();

	sht::utility::EventListenerInterface * event_listener_;
	GameSettings * game_settings_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID gui_shader_id_;
	sht::utility::ResourceID font_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * gui_shader_;
	sht::graphics::Font * font_;
	sht::graphics::StaticText * text_;

	sht::utility::ui::ColoredBoard * main_board_;
	sht::utility::ui::Rect * new_game_rect_;
	sht::utility::ui::Rect * options_rect_;
	sht::utility::ui::Rect * exit_rect_;
	sht::utility::ui::ColoredBoard * options_board_;
	sht::utility::ui::Rect * options_players_increase_rect_;
	sht::utility::ui::Rect * options_players_rect_;
	sht::utility::ui::Label * options_players_label_;
	sht::utility::ui::Rect * options_players_decrease_rect_;
	sht::utility::ui::Rect * options_exit_rect_;
};

#endif