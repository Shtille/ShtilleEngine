#pragma once
#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "utility/include/scene/scene.h"
#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"

class MenuScene : public sht::utility::Scene {
public:
	MenuScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener);
	virtual ~MenuScene();

	void Update();
	void Render();

	void Load();
	void Unload();

	void OnKeyDown(sht::PublicKey key, int mods) final;

private:
	sht::utility::EventListenerInterface * event_listener_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID font_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Font * font_;
	sht::graphics::StaticText * text_;
};

#endif