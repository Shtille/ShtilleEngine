#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "utility/include/scene/scene.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "utility/include/camera.h"
#include "physics/include/physics_engine.h"

class GameScene : public sht::utility::Scene {
public:
	GameScene(sht::graphics::Renderer * renderer);
	virtual ~GameScene();

	void Update();
	void Render();

	void Load();
	void Unload();

private:
	void RenderObjects();
	void RenderInterface();

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID font_id_;
	sht::utility::ResourceID sphere_mesh_id_;
	sht::utility::ResourceID table_mesh_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Font * font_;
	//sht::graphics::Mesh * sphere_mesh_;
	//sht::graphics::Mesh * table_mesh_;

	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	sht::physics::Engine * physics_;
};

#endif