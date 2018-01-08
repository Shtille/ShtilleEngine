#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "game_settings.h"

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
		const GameSettings * game_settings);
	virtual ~GameScene();

	void Update() override;
	void UpdatePhysics(float sec) override;
	void Render() override;

	void Load() override;
	void Unload() override;

private:
	void BindShaderConstants();
	void BindShaderVariables();
	void CreateMenu();
	void SetupCamera();

	void RenderMaze();
	void RenderBall();
	void RenderObjects();
	void RenderInterface();

	void PrepareBeginning();
	void CheckTimerEvents();
	void OnPlayerWon();
	void OnPlayerLost();

	void OnKeyDown(sht::PublicKey key, int mods) override;
	void OnMouseDown(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseUp(sht::MouseButton button, int modifiers, float x, float y) final;
	void OnMouseMove(float x, float y) final;

	sht::utility::EventListenerInterface * event_listener_;
	const GameSettings * game_settings_;

	sht::system::Timer * pocket_entrance_timer_;

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

	sht::utility::ResourceID text_shader_id_;
	sht::utility::ResourceID gui_shader_id_;
	sht::utility::ResourceID maze_shader_id_;
	sht::utility::ResourceID ball_shader_id_;
	sht::utility::ResourceID font_id_;
	sht::utility::ResourceID ball_mesh_id_;
	sht::utility::ResourceID maze_mesh_id_;

	sht::graphics::Shader * text_shader_;
	sht::graphics::Shader * gui_shader_;
	sht::graphics::Shader * maze_shader_;
	sht::graphics::Shader * ball_shader_;
	sht::graphics::Font * font_;
	sht::graphics::ComplexMesh * ball_mesh_;
	sht::graphics::ComplexMesh * maze_mesh_;

	sht::graphics::DynamicText * fps_text_;
	sht::utility::CameraManager * camera_manager_;
	sht::physics::Engine * physics_;
	sht::physics::Object * ball_;
	sht::physics::Object * maze_;

	sht::math::Matrix4 projection_view_matrix_;
};

#endif