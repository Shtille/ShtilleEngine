#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "game_settings.h"
#include "game_phase.h"
#include "albedo_material_binder.h"
#include "animation.h"
#include "pose_listener.h"

#include "utility/include/scene/scene.h"
#include "utility/include/event_listener.h"

#include "utility/include/event_listener.h"
#include "graphics/include/renderer/text.h"
#include "graphics/include/model/complex_mesh.h"
#include "system/include/time/time_manager.h"
#include "utility/include/camera.h"
#include "physics/include/physics_engine.h"
#include "utility/include/ui/board.h"
#include "utility/include/ui/rect.h"
#include "utility/include/ui/label.h"
#include "utility/include/ui/slider.h"
#include "utility/include/ui/button.h"

class MaterialBinder;

class GameScene : public sht::utility::Scene {
public:
	GameScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener,
		MaterialBinder * material_binder, const GameSettings * game_settings);
	virtual ~GameScene();

	void Update() override;
	void UpdatePhysics(float sec) override;
	void Render() override;

	void Load() override;
	void Unload() override;

private:
	void BindShaderConstants();
	void BindShaderVariables();
	void BuildAnimationClips();
	void CreateMenu();
	void SetupCamera();

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
	void UpdateCamera();
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
	void OnCueAimButtonTouched();
	void OnCueHitButtonTouched();
	void OnHorizontalSliderMoved();
	void OnVerticalSliderMoved();

	void OnKeyDown(sht::PublicKey key, int mods) override;
	void OnMouseDown(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseUp(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseMove(float x, float y) final;

	sht::utility::EventListenerInterface * event_listener_;
	MaterialBinder * material_binder_;
	AlbedoMaterialBinder albedo_material_binder_;
	const GameSettings * game_settings_;
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

	sht::utility::ui::Widget * ui_root_;
	sht::utility::ui::ColoredBoard * menu_board_;
	sht::utility::ui::Rect * menu_continue_rect_;
	sht::utility::ui::Rect * menu_give_up_rect_;
	sht::utility::ui::Rect * menu_exit_rect_;
	sht::utility::ui::ColoredBoard * victory_board_;
	sht::utility::ui::Rect * victory_exit_rect_;
	sht::utility::ui::ColoredBoard * defeat_board_;
	sht::utility::ui::Label * defeat_score_label_;
	sht::utility::ui::Rect * defeat_exit_rect_;
	sht::utility::ui::ColoredBoard * cue_aim_horizontal_board_;
	sht::utility::ui::SliderColored * cue_aim_horizontal_slider_;
	sht::utility::ui::ColoredBoard * cue_aim_vertical_board_;
	sht::utility::ui::SliderColored * cue_aim_vertical_slider_;
	sht::utility::ui::ButtonColored * cue_aim_button_;
	sht::utility::ui::ButtonColored * cue_hit_button_;

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID gui_shader_id_;
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
	sht::graphics::Shader * gui_shader_;
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

	unsigned int current_player_index_;
	unsigned int * scores_;

	bool need_render_cue_;
	bool need_render_rack_;
	bool cue_collides_;
};

#endif