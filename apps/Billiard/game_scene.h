#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "game_mode.h"
#include "game_phase.h"
#include "albedo_material_binder.h"
#include "animation.h"
#include "pose_listener.h"

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
	void BuildAnimationClips();

	void RenderTable();
	void RenderBalls();
	void RenderRack(); //!< the thing that restricts balls movement during setup
	void RenderCue();
	void RenderObjects();
	void RenderInterface();

	void SetStatus(const wchar_t* text);
	void PrepareBeginning();
	void UpdateCueMatrix();
	void UpdateCueCollision();
	void RespawnCueBall(const vec3& position);
	void CheckTimerEvents();
	void OnBallsStopMoving();
	void OnBallFallIntoThePocket(unsigned int index);
	void OnPlayerWon();
	void OnPlayerLost();
	void IncreaseScore();
	void SwitchToTheNextPlayer();
	void SwitchToCueTargeting();
	void RequestCueHit();
	void RequestRackRemove();
	void HitCueBall();

	void OnKeyDown(sht::PublicKey key, int mods) override;

	MaterialBinder * material_binder_;
	AlbedoMaterialBinder albedo_material_binder_;
	GameMode game_mode_;
	GamePhase phase_;

	PoseListener rack_pose_listener_;
	PoseListener cue_pose_listener_;

	AnimationClip * rack_animation_clip_;
	AnimationClip * cue_animation_clip_;
	AnimationController * rack_animation_controller_;
	AnimationController * cue_animation_controller_;

	sht::system::Timer * spawn_timer_;
	sht::system::Timer * pocket_entrance_timer_;
	sht::system::Timer * cue_animation_timer_;
	sht::system::Timer * rack_animation_timer_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID object_shader_id_;
	sht::utility::ResourceID ball_shader_id_;
	sht::utility::ResourceID ghost_shader_id_;
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
	sht::graphics::Shader * ghost_shader_;
	sht::graphics::Font * font_;
	sht::graphics::ComplexMesh * ball_mesh_;
	sht::graphics::ComplexMesh * cue_mesh_;
	sht::graphics::ComplexMesh * rack_mesh_;
	sht::graphics::ComplexMesh * table_mesh_;
	sht::graphics::ComplexMesh * table_bed_mesh_;
	sht::graphics::ComplexMesh * table_cushions_graphics_mesh_;
	sht::graphics::ComplexMesh * table_cushions_physics_mesh_;

	sht::graphics::DynamicText * fps_text_;
	sht::graphics::DynamicText * status_text_;
	sht::utility::CameraManager * camera_manager_;
	sht::physics::Engine * physics_;
	sht::physics::Object * *balls_;
	sht::physics::GhostObject * cue_;
	sht::graphics::Texture * *ball_textures_;
	bool *ball_active_;
	unsigned int balls_count_;
	float ball_size_;

	float cue_alpha_;
	float cue_theta_;

	// Light parameters
	float light_angle_;
	float light_distance_;
	sht::math::Vector3 light_position_;

	sht::math::Matrix4 projection_view_matrix_;

	bool need_render_cue_;
	bool need_render_rack_;
	bool cue_collides_;
};

#endif