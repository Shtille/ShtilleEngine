#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "utility/include/scene/scene.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "graphics/include/model/complex_mesh.h"
#include "utility/include/camera.h"
#include "physics/include/physics_engine.h"

class MaterialBinder;

class GameScene : public sht::utility::Scene {
public:
	GameScene(sht::graphics::Renderer * renderer, MaterialBinder * material_binder);
	virtual ~GameScene();

	void Update() override;
	void Render() override;

	void Load() override;
	void Unload() override;

private:
	void RenderTable();
	void RenderBalls();
	void RenderRack(); //!< the thing that restricts balls movement during setup
	void RenderCue();
	void RenderObjects();
	void RenderInterface();

	void OnKeyDown(sht::PublicKey key, int mods) override;

	MaterialBinder * material_binder_;

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
	sht::physics::Object * *balls_;
	unsigned int balls_count_;
	sht::physics::Object * table_;
	sht::physics::Object * rack_;
	sht::physics::Object * cue_;

	// Light parameters
	float light_angle_;
	float light_distance_;
	sht::math::Vector3 light_position_;

	sht::math::Matrix4 projection_view_matrix_;
};

#endif