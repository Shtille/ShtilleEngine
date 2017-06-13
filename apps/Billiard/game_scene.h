#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "utility/include/scene/scene.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "graphics/include/model/complex_mesh.h"
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
	void RenderTable();
	void RenderBalls();
	void RenderRack(); //!< the thing that restricts balls movement during setup
	void RenderCue();
	void RenderObjects();
	void RenderInterface();

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID object_shader_id_;
	sht::utility::ResourceID font_id_;
	sht::utility::ResourceID ball_mesh_id_;
	sht::utility::ResourceID cue_mesh_id_;
	sht::utility::ResourceID rack_mesh_id_;
	sht::utility::ResourceID table_mesh_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * object_shader_;
	sht::graphics::Font * font_;
	sht::graphics::ComplexMesh * ball_mesh_;
	sht::graphics::ComplexMesh * cue_mesh_;
	sht::graphics::ComplexMesh * rack_mesh_;
	sht::graphics::ComplexMesh * table_mesh_;

	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	sht::physics::Engine * physics_;

	// Light parameters
	float light_angle_;
	float light_distance_;
	sht::math::Vector3 light_position_;
};

#endif