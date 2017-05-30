#pragma once
#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "scene.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/renderer.h"
#include "graphics/include/renderer/text.h"
#include "system/include/time/time_manager.h"

class LogoScene : public Scene {
public:
	LogoScene(EventListenerInterface * event_listener);
	virtual ~LogoScene();

	void Update();
	void Render();

	void Load();
	void Unload();

private:
	EventListenerInterface * event_listener_;
	sht::system::Timer * timer_;
	float opacity_; //!< opacity for rendering

	ResourceID text_shader_id_;
	ResourceID font_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Font * font_;
	sht::graphics::DynamicText * fps_text_;
};

#endif