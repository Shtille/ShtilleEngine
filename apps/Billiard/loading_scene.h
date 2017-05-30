#pragma once
#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "utility/include/scene/scene.h"
#include "utility/include/scene/scene_transition_listener.h"
#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"

class LoadingScene
	: public sht::utility::Scene
	, public sht::utility::SceneTransitionListenerInterface
{
public:
	LoadingScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener);
	virtual ~LoadingScene();

	void Update();
	void Render();

	void Load();
	void Unload();

private:
	// Derived from SceneTransitionListenerInterface:
	void OnObtainCount(int count) final;
	void OnStep() final;
	void OnFinish() final;

	sht::utility::EventListenerInterface * event_listener_;

	int count_;
	int counter_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID font_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Font * font_;
	sht::graphics::StaticText * text_;
};

#endif