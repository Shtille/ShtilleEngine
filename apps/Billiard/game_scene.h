#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "game_mode.h"

#include "utility/include/scene/scene.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "graphics/include/model/complex_mesh.h"
#include "system/include/time/time_manager.h"
#include "utility/include/camera.h"
#include "physics/include/physics_engine.h"

class MaterialBinder;

class GameScene : public sht::utility::Scene {
public:
	GameScene(sht::graphics::Renderer * renderer, MaterialBinder * material_binder, GameMode game_mode);
	virtual ~GameScene();

	void SetGameMode(GameMode game_mode);

	void Update() override;
	void UpdatePhysics(float sec) override;
	void Render() override;

	void Load() override;
	void Unload() override;

private:
	void BindShaderConstants();
	void BindShaderVariables();

	void RenderTable();
	void RenderBalls();
	void RenderRack(); //!< the thing that restricts balls movement during setup
	void RenderCue();
	void RenderObjects();
	void RenderInterface();

	void RespawnCueBall(const vec3& position);
	void CheckBallsStatus();

	void OnKeyDown(sht::PublicKey key, int mods) override;

	MaterialBinder * material_binder_;
	GameMode game_mode_;

	sht::system::Timer * spawn_timer_;
	sht::system::Timer * pocket_entrance_timer_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID object_shader_id_;
	sht::utility::ResourceID ball_shader_id_;
	sht::utility::ResourceID font_id_;
	sht::utility::ResourceID ball_mesh_id_;
	sht::utility::ResourceID cue_mesh_id_;
	sht::utility::ResourceID rack_mesh_id_;
	sht::utility::ResourceID table_mesh_id_;
	sht::utility::ResourceID table_bed_mesh_id_;
	sht::utility::ResourceID table_cushions_graphics_mesh_id_;
	sht::utility::ResourceID table_cushions_physics_mesh_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * object_shader_;
	sht::graphics::Shader * ball_shader_;
	sht::graphics::Font * font_;
	sht::graphics::ComplexMesh * ball_mesh_;
	sht::graphics::ComplexMesh * cue_mesh_;
	sht::graphics::ComplexMesh * rack_mesh_;
	sht::graphics::ComplexMesh * table_mesh_;
	sht::graphics::ComplexMesh * table_bed_mesh_;
	sht::graphics::ComplexMesh * table_cushions_graphics_mesh_;
	sht::graphics::ComplexMesh * table_cushions_physics_mesh_;

	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	sht::physics::Engine * physics_;
	sht::physics::Object * *balls_;
	sht::graphics::Texture * *ball_textures_;
	unsigned int balls_count_;

	// Light parameters
	float light_angle_;
	float light_distance_;
	sht::math::Vector3 light_position_;

	sht::math::Matrix4 projection_view_matrix_;
};

#endif