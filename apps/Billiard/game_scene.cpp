#include "game_scene.h"

#include "material_binder.h"

#include "utility/include/event.h"
#include "physics/include/physics_object.h"
#include "physics/include/physics_ghost_object.h"

#include <cmath> // for sinf and cosf
#include <cstdio>
#include <cstring>
#include <cassert>

namespace {
	const float kRackAnimationTime = 10.0f;
	const float kCueAnimationTime = 3.0f;
}

GameScene::GameScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener,
	MaterialBinder * material_binder, const GameSettings * game_settings)
: Scene(renderer)
, event_listener_(event_listener)
, material_binder_(material_binder)
, game_settings_(game_settings)
, phase_(GamePhase::kRackRemoving)
, rack_animation_clip_(nullptr)
, cue_animation_clip_(nullptr)
, rack_animation_controller_(nullptr)
, cue_animation_controller_(nullptr)
, menu_board_(nullptr)
, victory_board_(nullptr)
, defeat_board_(nullptr)
, text_shader_(nullptr)
, object_shader_(nullptr)
, ball_shader_(nullptr)
, font_(nullptr)
, ball_mesh_(nullptr)
, cue_mesh_(nullptr)
, rack_mesh_(nullptr)
, table_mesh_(nullptr)
, fps_text_(nullptr)
, status_text_(nullptr)
, camera_manager_(nullptr)
, physics_(nullptr)
, balls_(nullptr)
, cue_(nullptr)
, ball_active_(nullptr)
, balls_count_(1)
, cue_alpha_(0.0f)
, cue_theta_(0.0f)
, light_angle_(0.0f)
, light_distance_(10000.0f)
, current_player_index_(0)
, scores_(nullptr)
, need_render_cue_(false)
, need_render_rack_(false)
, cue_collides_(false)
{
	// Add timers
	const float kBallRequestInterval = 1.0f;
	spawn_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kBallRequestInterval);
	pocket_entrance_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kBallRequestInterval);
	cue_animation_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kCueAnimationTime);
	rack_animation_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kRackAnimationTime);

	// Register resources to load automatically on scene change
	text_shader_id_   					= AddResourceIdByName(ConstexprStringId("shader_text"));
	gui_shader_id_   					= AddResourceIdByName(ConstexprStringId("shader_gui"));
	object_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_object"));
	ball_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_ball"));
	ghost_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_ghost"));
	font_id_          					= AddResourceIdByName(ConstexprStringId("font_good_dog"));
	ball_mesh_id_     					= AddResourceIdByName(ConstexprStringId("mesh_ball"));
	cue_mesh_id_      					= AddResourceIdByName(ConstexprStringId("mesh_cue"));
	rack_mesh_id_     					= AddResourceIdByName(ConstexprStringId("mesh_rack"));
	table_mesh_id_    					= AddResourceIdByName(ConstexprStringId("mesh_table"));
	table_bed_mesh_id_					= AddResourceIdByName(ConstexprStringId("mesh_table_bed"));
	table_cushions_graphics_mesh_id_	= AddResourceIdByName(ConstexprStringId("mesh_table_cushions_graphics"));
	table_cushions_physics_mesh_id_		= AddResourceIdByName(ConstexprStringId("mesh_table_cushions_physics"));
}
GameScene::~GameScene()
{
	Unload();

	// Remove timers
	sht::system::TimeManager::GetInstance()->RemoveTimer(rack_animation_timer_);
	sht::system::TimeManager::GetInstance()->RemoveTimer(cue_animation_timer_);
	sht::system::TimeManager::GetInstance()->RemoveTimer(pocket_entrance_timer_);
	sht::system::TimeManager::GetInstance()->RemoveTimer(spawn_timer_);
}
void GameScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFixedFrameTime();

	// Update light parameters
	light_angle_ += 0.2f * frame_time;
	light_position_.Set(light_distance_*cosf(light_angle_), light_distance_, light_distance_*sinf(light_angle_));

	// Camera should be updated after physics
	camera_manager_->Update(frame_time);

	// Update UI
	menu_board_->UpdateAll(frame_time);
	victory_board_->UpdateAll(frame_time);
	defeat_board_->UpdateAll(frame_time);

	// Check timers' events
	CheckTimerEvents();

	// Update view matrix
	renderer_->SetViewMatrix(camera_manager_->view_matrix());
	projection_view_matrix_ = renderer_->projection_matrix() * renderer_->view_matrix();

	// Finally update shader variables
	BindShaderVariables();
}
void GameScene::UpdatePhysics(float sec)
{
	// Update physics engine
	physics_->Update(sec);
}
void GameScene::BindShaderConstants()
{
	const sht::math::Vector3 kLightColor(1.0f);

	object_shader_->Bind();
	object_shader_->Uniform3fv("u_light.color", kLightColor);

	ball_shader_->Bind();
	ball_shader_->Uniform1i("u_texture", 0);
	ball_shader_->Uniform3fv("u_light.color", kLightColor);

	ghost_shader_->Bind();
	// Since ghost shader used for cue rendering in case of block we may set up color just once.
	ghost_shader_->Uniform4f("u_color", 1.0f, 0.0f, 0.0f, 0.5f);

	ghost_shader_->Unbind();
}
void GameScene::BindShaderVariables()
{
	object_shader_->Bind();
	object_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	object_shader_->Uniform3fv("u_light.position", light_position_);
	object_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ball_shader_->Bind();
	ball_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	ball_shader_->Uniform3fv("u_light.position", light_position_);
	ball_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ghost_shader_->Bind();
	ghost_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);

	ghost_shader_->Unbind();
}
void GameScene::BuildAnimationClips()
{
	// Rack animation clip
	rack_animation_clip_ = new AnimationClip();
	rack_animation_clip_->keys.resize(3);
	rack_animation_clip_->keys[0].time = 0.0f;
	rack_animation_clip_->keys[0].pose.rotation.Set(UNIT_Y, sht::math::kPi);
	rack_animation_clip_->keys[0].pose.position.Set(0.0f, 0.0f, 0.0f);
	rack_animation_clip_->keys[1].time = 0.5f * kRackAnimationTime;
	rack_animation_clip_->keys[1].pose.rotation.Set(UNIT_Y, sht::math::kPi);
	rack_animation_clip_->keys[1].pose.position.Set(0.0f, 0.0f, 0.0f);
	rack_animation_clip_->keys[2].time = kRackAnimationTime;
	rack_animation_clip_->keys[2].pose.rotation.Set(UNIT_Y, sht::math::kPi);
	rack_animation_clip_->keys[2].pose.position.Set(0.0f, 1000.0f, 0.0f);
	rack_animation_clip_->duration = kRackAnimationTime;
	rack_animation_clip_->playback_rate = 1.0f;
	rack_animation_clip_->flags =
		(unsigned short)AnimationClip::Flags::kRotateFlag |
		(unsigned short)AnimationClip::Flags::kTranslateFlag;
	rack_animation_clip_->is_looping = false;

	// Cue animation clip
	/*
	We need to calculate duration of hit animation.
	Ball after hit has velocity 2.5 m/s. Ball mass is 150 g. Cue mass is 540 g.
	So cue should have velocity of 150 * 2.5 / 540 = 0.694 m/s = 694 mm/s.
	Distance of hit is about 200 mm.
	So hit time should be 200 / 694 = 0.288 s.
	*/
	cue_animation_clip_ = new AnimationClip();
	cue_animation_clip_->keys.resize(4);
	cue_animation_clip_->keys[0].time = 0.0f;
	cue_animation_clip_->keys[0].pose.position.Set(0.0f, 0.0f, 0.0f);
	cue_animation_clip_->keys[1].time = 1.0f;
	cue_animation_clip_->keys[1].pose.position.Set(-200.0f, 0.0f, 0.0f);
	cue_animation_clip_->keys[2].time = kCueAnimationTime - 0.288f;
	cue_animation_clip_->keys[2].pose.position.Set(-200.0f, 0.0f, 0.0f);
	cue_animation_clip_->keys[3].time = kCueAnimationTime;
	cue_animation_clip_->keys[3].pose.position.Set(0.0f, 0.0f, 0.0f);
	cue_animation_clip_->duration = kCueAnimationTime;
	cue_animation_clip_->playback_rate = 1.0f;
	cue_animation_clip_->flags = (unsigned short)AnimationClip::Flags::kTranslateFlag;
	cue_animation_clip_->is_looping = false;

	// Rack animation controller
	rack_animation_controller_ = new AnimationController(&rack_pose_listener_);
	rack_animation_controller_->SetClip(rack_animation_clip_);

	// Cue animation controller
	cue_animation_controller_ = new AnimationController(&cue_pose_listener_);
	cue_animation_controller_->SetClip(cue_animation_clip_);
}
void GameScene::RenderTable()
{
	object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	table_mesh_->Render();
	table_bed_mesh_->Render();
	table_cushions_graphics_mesh_->Render();
}
void GameScene::RenderBalls()
{
	for (unsigned int i = 0; i < balls_count_; ++i)
	{
		renderer_->ChangeTexture(ball_textures_[i]);
		renderer_->PushMatrix();
		renderer_->MultMatrix(balls_[i]->matrix());
		ball_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		ball_mesh_->Render();
		renderer_->PopMatrix();
	}
	renderer_->ChangeTexture(nullptr);
}
void GameScene::RenderRack()
{
	if (need_render_rack_)
	{
		renderer_->PushMatrix();
		renderer_->MultMatrix(rack_pose_listener_.world_matrix());
		object_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
		rack_mesh_->Render();
		renderer_->PopMatrix();
	}
}
void GameScene::RenderCue()
{
	if (need_render_cue_)
	{
		sht::graphics::Shader * shader;
		if (cue_collides_)
		{
			shader = ghost_shader_;
			shader->Bind();
		}
		else
			shader = object_shader_;
		renderer_->PushMatrix();
		renderer_->MultMatrix(cue_pose_listener_.world_matrix());
		shader->UniformMatrix4fv("u_model", renderer_->model_matrix());
		cue_mesh_->Render();
		renderer_->PopMatrix();
	}
}
void GameScene::RenderObjects()
{
	object_shader_->Bind();
	RenderTable();
	RenderRack();
	RenderCue();

	ball_shader_->Bind();
	RenderBalls();
}
void GameScene::RenderInterface()
{
	renderer_->DisableDepthTest();

	// Draw FPS
	text_shader_->Bind();
	text_shader_->Uniform1i("u_texture", 0);
	text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_rate = time_manager->GetFrameRate();
	fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"fps: %.2f", frame_rate);
	fps_text_->Render();

	// Draw status text
	status_text_->Render();

	// Render UI
	gui_shader_->Bind();
	if (!menu_board_->IsPosUp())
	{
		if (menu_board_->IsPosDown())
			menu_board_->RenderAll(); // render entire tree
		else
			menu_board_->Render(); // render only board rect (smart hack for labels :D)
	}
	if (!victory_board_->IsPosUp())
	{
		if (victory_board_->IsPosDown())
			victory_board_->RenderAll(); // render entire tree
		else
			victory_board_->Render(); // render only board rect (smart hack for labels :D)
	}
	if (!defeat_board_->IsPosUp())
	{
		if (defeat_board_->IsPosDown())
			defeat_board_->RenderAll(); // render entire tree
		else
			defeat_board_->Render(); // render only board rect (smart hack for labels :D)
	}

	renderer_->EnableDepthTest();
}
void GameScene::Render()
{
	RenderObjects();
	RenderInterface();
}
void GameScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();

	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	gui_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(gui_shader_id_));
	object_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(object_shader_id_));
	ball_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(ball_shader_id_));
	ghost_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(ghost_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));
	ball_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(ball_mesh_id_));
	cue_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(cue_mesh_id_));
	rack_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(rack_mesh_id_));
	table_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_mesh_id_));
	table_bed_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_bed_mesh_id_));
	table_cushions_graphics_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_cushions_graphics_mesh_id_));
	table_cushions_physics_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(table_cushions_physics_mesh_id_));

	// Bind shader to material binder
	material_binder_->SetShader(object_shader_);
	albedo_material_binder_.SetShader(ball_shader_);

	// Assign custom material binders to meshes
	ball_mesh_->SetMaterialBinder(&albedo_material_binder_);

	// Create text objects
	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
	status_text_ = sht::graphics::DynamicText::Create(renderer_, 64);
	SetStatus(L"start");

	// Create physics
	physics_ = new sht::physics::Engine(20 /* max sub steps */, 0.002f /* fixed time step */);
	physics_->SetGravity(vec3(0.0f, -9800.0f, 0.0f));

	constexpr unsigned int kBallCount = 16;
	balls_count_ = kBallCount;
	ball_size_ = ball_mesh_->bounding_box().extent.x;
	ball_active_ = new bool[balls_count_];
	for (unsigned int i = 0; i < balls_count_; ++i)
		ball_active_[i] = true;
	balls_ = new sht::physics::Object *[balls_count_];

	// Setup physics objects
	sht::physics::Object * object;
	{
		const float lx = 2.0f * ball_size_ * cosf(sht::math::kPi / 6.0);
		const float lz = 2.0f * ball_size_ * sinf(sht::math::kPi / 6.0);
		const float bx = 0.5f * table_bed_mesh_->bounding_box().extent.x;
		const vec3 ball_positions[16] = {
			vec3(-bx, ball_size_, 0.0f),
			vec3(bx - 2.0f * lx, ball_size_, 0.0f), // *
			vec3(bx - 1.0f * lx, ball_size_, - 1.0f * lz),
			vec3(bx - 1.0f * lx, ball_size_, - 1.0f * lz + 2.0f * ball_size_),
			vec3(bx, ball_size_, 0.0f),
			vec3(bx, ball_size_, -2.0f * ball_size_),
			vec3(bx, ball_size_,  2.0f * ball_size_),
			vec3(bx + 1.0f * lx, ball_size_, - 1.0f * lz - 2.0f * ball_size_),
			vec3(bx + 1.0f * lx, ball_size_, - 1.0f * lz),
			vec3(bx + 1.0f * lx, ball_size_, + 1.0f * lz),
			vec3(bx + 1.0f * lx, ball_size_, + 1.0f * lz + 2.0f * ball_size_),
			vec3(bx + 2.0f * lx, ball_size_, -4.0f * ball_size_),
			vec3(bx + 2.0f * lx, ball_size_, -2.0f * ball_size_),
			vec3(bx + 2.0f * lx, ball_size_, 0.0f),
			vec3(bx + 2.0f * lx, ball_size_, +2.0f * ball_size_),
			vec3(bx + 2.0f * lx, ball_size_, +4.0f * ball_size_),
		};
		static_assert(kBallCount <= sizeof(ball_positions)/sizeof(ball_positions[0]), "balls count mismatch");
		for (unsigned int i = 0; i < balls_count_; ++i)
		{
			object = physics_->AddSphere(ball_positions[i], 0.150f, ball_size_);
			object->SetFriction(0.28f);
			object->SetRollingFriction(0.2f);
			object->SetSpinningFriction(0.5f);
			object->SetRestitution(0.98f);
			balls_[i] = object;
		}
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetRestitution(0.5f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_bed_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetFriction(0.71f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(table_cushions_physics_mesh_);
		object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetFriction(0.2f);
		object->SetRestitution(0.8f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(cue_mesh_);
		cue_ = physics_->AddGhostObject(vec3(0.0f, 0.0f, 0.0f), &enumerator);
	}

	// Create ball textures
	ball_textures_ = new sht::graphics::Texture *[balls_count_];
	if (game_settings_->game_mode == GameMode::kSimplePool)
	{
		const vec3 ball_colors[16] = {
			vec3(1.0f, 1.0f, 1.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(1.0f, 0.0f, 0.0f),
			vec3(0.0f, 0.0f, 0.0f),
		};
		static_assert(kBallCount <= sizeof(ball_colors)/sizeof(ball_colors[0]), "enlarge ball_colors array");
		for (unsigned int i = 0; i < balls_count_; ++i)
		{
			const vec3& color = ball_colors[i];
			renderer_->CreateTextureColor(ball_textures_[i], color.x, color.y, color.z, 1.0f);
		}
	}

	// Create meshes that have been loaded earlier
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
	ball_mesh_->MakeRenderable();
	cue_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	cue_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	cue_mesh_->MakeRenderable();
	rack_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	rack_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	rack_mesh_->MakeRenderable();
	table_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_mesh_->MakeRenderable();
	table_bed_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_bed_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_bed_mesh_->MakeRenderable();
	table_cushions_graphics_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	table_cushions_graphics_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	table_cushions_graphics_mesh_->MakeRenderable();

	BuildAnimationClips();

	SetupCamera();

	CreateMenu();

	scores_ = new unsigned int[game_settings_->num_players];

	PrepareBeginning();
	RequestRackRemove(); // later can be moved after table look overview

	BindShaderConstants();
}
void GameScene::Unload()
{
	if (scores_)
	{
		delete[] scores_;
		scores_ = nullptr;
	}
	if (defeat_board_)
	{
		delete defeat_board_;
		defeat_board_ = nullptr;
	}
	if (victory_board_)
	{
		delete victory_board_;
		victory_board_ = nullptr;
	}
	if (menu_board_)
	{
		delete menu_board_;
		menu_board_ = nullptr;
	}
	if (ball_active_)
	{
		delete[] ball_active_;
		ball_active_ = nullptr;
	}
	if (ball_textures_)
	{
		if (game_settings_->game_mode == GameMode::kSimplePool)
		{
			for (unsigned int i = 0; i < balls_count_; ++i)
				if (ball_textures_[i])
					renderer_->DeleteTexture(ball_textures_[i]);
		}
		delete[] ball_textures_;
		ball_textures_ = nullptr;
	}
	if (balls_)
	{
		delete[] balls_;
		balls_ = nullptr;
	}
	if (physics_)
	{
		delete physics_;
		physics_ = nullptr;
	}
	if (camera_manager_)
	{
		delete camera_manager_;
		camera_manager_ = nullptr;
	}
	if (status_text_)
	{
		delete status_text_;
		status_text_ = nullptr;
	}
	if (fps_text_)
	{
		delete fps_text_;
		fps_text_ = nullptr;
	}
	if (cue_animation_controller_)
	{
		delete cue_animation_controller_;
		cue_animation_controller_ = nullptr;
	}
	if (rack_animation_controller_)
	{
		delete rack_animation_controller_;
		rack_animation_controller_ = nullptr;
	}
	if (cue_animation_clip_)
	{
		delete cue_animation_clip_;
		cue_animation_clip_ = nullptr;
	}
	if (rack_animation_clip_)
	{
		delete rack_animation_clip_;
		rack_animation_clip_ = nullptr;
	}
}
void GameScene::SetStatus(const wchar_t* text)
{
	status_text_->SetText(font_, 0.0f, 0.9f, 0.05f, text);
}
void GameScene::PrepareBeginning()
{
	cue_alpha_ = 0.0f;
	cue_theta_ = 0.0f;
	light_angle_ = 0.0f;
	light_distance_ = 10000.0f;

	current_player_index_ = 0;
	for (unsigned int i = 0; i < game_settings_->num_players; ++i)
		scores_[i] = 0;
	need_render_cue_ = false;
	need_render_rack_ = true;
	cue_collides_ = false;

	vec3 position(0.5f * table_bed_mesh_->bounding_box().extent.x, 0.5f * rack_mesh_->bounding_box().extent.y, 0.0f);
	rack_pose_listener_.SetLocalToWorldMatrix(sht::math::Translate(position));

	current_player_index_ = 0;
}
void GameScene::UpdateCueMatrix()
{
	sht::physics::Object * cue_ball = balls_[0];
	sht::math::Vector3 direction(
		cosf(cue_alpha_) * cosf(cue_theta_),
		sinf(cue_theta_),
		sinf(cue_alpha_) * cosf(cue_theta_));
	sht::math::Vector3 position = cue_ball->position() - direction * (ball_size_ * 1.1f);
	cue_pose_listener_.SetLocalToWorldMatrix(OrientationMatrix(RotationMatrix(direction), position));
	cue_->SetTransform(cue_pose_listener_.world_matrix());
	UpdateCueCollision();
	UpdateCamera();
}
void GameScene::UpdateCueCollision()
{
	cue_collides_ = physics_->ContactTest(cue_);
}
void GameScene::RespawnCueBall(const vec3& position)
{
	sht::physics::Object * cue_ball = balls_[0];
	cue_ball->Activate();
	cue_ball->SetPosition(position);
	cue_ball->SetLinearVelocity(vec3(0.0f));
	cue_ball->SetAngularVelocity(vec3(0.0f));
	ball_active_[0] = true;
	spawn_timer_->Start();
	UpdateCueMatrix();
}
void GameScene::CheckTimerEvents()
{
	if (spawn_timer_->enabled() && spawn_timer_->HasExpired())
	{
		sht::physics::Object * cue_ball = balls_[0];
		if (!cue_ball->IsActive())
		{
			// Ball has been deactivated
			spawn_timer_->Stop();
			SwitchToCueTargeting();
		}
		spawn_timer_->Reset();
	}
	if (pocket_entrance_timer_->enabled() && pocket_entrance_timer_->HasExpired())
	{
		bool any_active_ball_is_rolling = false;
		for (unsigned int i = 0; i < balls_count_; ++i)
		{
			if (ball_active_[i] && balls_[i]->IsActive() && balls_[i]->position().y > 0.0f)
			{
				any_active_ball_is_rolling = true;
				break;
			}
		}
		if (!any_active_ball_is_rolling)
		{
			// All the balls have stopped
			pocket_entrance_timer_->Stop();
			OnBallsStopMoving();
		}
		pocket_entrance_timer_->Reset();
	}
	if (cue_animation_timer_->enabled())
	{
		cue_animation_controller_->Update(cue_animation_timer_->time());
		if (cue_animation_timer_->HasExpired())
		{
			cue_animation_timer_->Stop();
			cue_animation_timer_->Reset();
			cue_animation_controller_->Stop();
			need_render_cue_ = false;
			HitCueBall();
		}
	}
	if (rack_animation_timer_->enabled())
	{
		rack_animation_controller_->Update(rack_animation_timer_->time());
		if (rack_animation_timer_->HasExpired())
		{
			rack_animation_timer_->Stop();
			rack_animation_timer_->Reset();
			rack_animation_controller_->Stop();
			need_render_rack_ = false;
			SwitchToCueTargeting();
		}
	}
}
void GameScene::OnBallsStopMoving()
{
	// Game mode based logics goes here
	if (game_settings_->game_mode == GameMode::kSimplePool)
	{
		const unsigned int kBlackBallIndex = 15;
		bool cue_ball_has_fallen = (ball_active_[0] && balls_[0]->position().y < 0.0f);
		bool black_ball_has_fallen = (ball_active_[kBlackBallIndex] && balls_[kBlackBallIndex]->position().y < 0.0f);
		if (black_ball_has_fallen)
		{
			ball_active_[kBlackBallIndex] = false;
			unsigned int number_of_rest_active_balls = 0;
			for (unsigned int i = 1; i < balls_count_; ++i)
				if (ball_active_[i] && balls_[i]->position().y > 0.0f)
					++number_of_rest_active_balls;
			if (number_of_rest_active_balls != 0) // not only cue ball has left
			{
				// Player has lost
				OnPlayerLost();
			}
			else // cue ball or none have left
			{
				// Player has won
				OnPlayerWon();
			}
		}
		else // black ball hasn't fallen
		{
			bool any_ball_has_fallen = false;
			for (unsigned int i = 1; i < balls_count_; ++i)
			{
				if (ball_active_[i] && balls_[i]->position().y < 0.0f)
				{
					ball_active_[i] = false;
					any_ball_has_fallen = true;
					IncreaseScore();
				}
			}
			if (cue_ball_has_fallen)
			{
				ball_active_[0] = false;
				RespawnCueBall(vec3(0.0f, 50.0f, 0.0f));
				SwitchToTheNextPlayer();
			}
			else if (!any_ball_has_fallen) // player hasn't scored any ball
			{
				SwitchToTheNextPlayer();
				SwitchToCueTargeting();
			}
			else // any colored ball has fallen
			{
				SwitchToCueTargeting();
			}
		}
	}
	else
	{
		//assert(false);
	}
}
void GameScene::OnPlayerWon()
{
	victory_board_->Move();
}
void GameScene::OnPlayerLost()
{
	defeat_board_->Move();
	// Update score label text
	wchar_t text[3] = {L'\0'};
	swprintf(text, _countof(text), L"%u", scores_[current_player_index_]);
	defeat_score_label_->SetText(text);
}
void GameScene::IncreaseScore()
{
	++scores_[current_player_index_];
}
void GameScene::SwitchToTheNextPlayer()
{
	// Increase player index
	if (current_player_index_ + 1 == game_settings_->num_players)
		current_player_index_ = 0;
	else
		++current_player_index_;
	wchar_t text[15] = L"Player 1 turn";
	text[7] += current_player_index_;
	SetStatus(text);
}
void GameScene::SwitchToCueTargeting()
{
	phase_ = GamePhase::kCueTargeting;
	need_render_cue_ = true;
	UpdateCueMatrix();
}
void GameScene::RequestCueHit()
{
	if (cue_collides_)
	{
		// Play some sound, etc.
		SetStatus(L"unable to do a hit");
	}
	else
	{
		cue_animation_timer_->Start();
		cue_animation_controller_->Play();
	}
}
void GameScene::RequestRackRemove()
{
	phase_ = GamePhase::kRackRemoving;
	rack_animation_timer_->Start();
	rack_animation_controller_->Play();
}
void GameScene::HitCueBall()
{
	// Impulse simply has the same direction like cue does
	const float kPushPower = 375.0f; // for velocity 2.5 m/s
	sht::math::Vector3 impulse(
		kPushPower * cosf(cue_alpha_) * cosf(cue_theta_),
		kPushPower * sinf(cue_theta_),
		kPushPower * sinf(cue_alpha_) * cosf(cue_theta_));
	sht::physics::Object * cue_ball = balls_[0];
	cue_ball->Activate(); // this body may be sleeping, thus we activate it
	cue_ball->ApplyCentralImpulse(impulse);
	phase_ = GamePhase::kBallsRolling;
	pocket_entrance_timer_->Start();
}
void GameScene::OnKeyDown(sht::PublicKey key, int mods)
{
	if (sht::PublicKey::kEscape == key)
	{
		menu_board_->Move();
	}

	// Cue targeting
	if (phase_ == GamePhase::kCueTargeting)
	{
		bool rotation_has_changed = false;
		if (key == sht::PublicKey::kSpace)
			RequestCueHit();
		else if (key == sht::PublicKey::kLeft)
		{
			rotation_has_changed = true;
			cue_alpha_ += 0.1f;
		}
		else if (key == sht::PublicKey::kRight)
		{
			rotation_has_changed = true;
			cue_alpha_ -= 0.1f;
		}
		else if (key == sht::PublicKey::kUp)
		{
			rotation_has_changed = true;
			cue_theta_ -= 0.1f;
		}
		else if (key == sht::PublicKey::kDown)
		{
			rotation_has_changed = true;
			cue_theta_ += 0.1f;
		}
		if (rotation_has_changed)
			UpdateCueMatrix();
	}

	// Camera rotation
	if (key == sht::PublicKey::kA)
		camera_manager_->RotateAroundTargetInY(-0.1f);
	else if (key == sht::PublicKey::kD)
		camera_manager_->RotateAroundTargetInY(0.1f);
	else if (key == sht::PublicKey::kW)
		camera_manager_->RotateAroundTargetInZ(0.1f);
	else if (key == sht::PublicKey::kS)
		camera_manager_->RotateAroundTargetInZ(-0.1f);
}
void GameScene::OnMouseDown(sht::MouseButton button, int modifiers, float x, float y)
{
	if (sht::MouseButton::kLeft == button)
	{
		if (menu_exit_rect_->active() ||
			victory_exit_rect_->active() ||
			defeat_exit_rect_->active())
		{
			sht::utility::Event event(ConstexprStringId("game_scene_exit_requested"));
			event_listener_->OnEvent(&event);
		}
		else if (menu_continue_rect_->active())
		{
			menu_continue_rect_->set_active(false);
			menu_board_->Move();
		}
		else if (menu_give_up_rect_->active())
		{
			menu_give_up_rect_->set_active(false);
			menu_board_->Move();
			OnPlayerLost();
		}
	}
}
void GameScene::OnMouseMove(float x, float y)
{
	if (menu_board_->IsPosDown())
		menu_board_->SelectAll(x, y);
	if (victory_board_->IsPosDown())
		victory_board_->SelectAll(x, y);
	if (defeat_board_->IsPosDown())
		defeat_board_->SelectAll(x, y);
}
void GameScene::CreateMenu()
{
	sht::utility::ui::Rect * rect;
	sht::utility::ui::Label * label;

	// Main menu
	menu_board_ = new sht::utility::ui::VerticalBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.4f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		menu_board_->AttachWidget(rect);
		menu_continue_rect_ = rect;
		const wchar_t * kText = L"Continue";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
		label->AlignCenter(rect->width(), rect->height());
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.3f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		menu_board_->AttachWidget(rect);
		menu_give_up_rect_ = rect;
		const wchar_t * kText = L"Give up";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
		label->AlignCenter(rect->width(), rect->height());
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.0f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		menu_board_->AttachWidget(rect);
		menu_exit_rect_ = rect;
		const wchar_t * kText = L"Exit";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
		label->AlignCenter(rect->width(), rect->height());
	}

	// Victory menu
	victory_board_ = new sht::utility::ui::VerticalBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.4f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderNever // u32 flags
			);
		victory_board_->AttachWidget(rect);
		wchar_t text[20] = {L'\0'};
		if (game_settings_->num_players == 1)
			wcscpy(text, L"You have won");
		else
		{
			wcscpy(text, L"Player 1 has won");
			text[7] += current_player_index_;
		}
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.10f, // text height
			wcslen(text)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(text);
		label->AlignCenter(rect->width(), rect->height());
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.0f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		victory_board_->AttachWidget(rect);
		victory_exit_rect_ = rect;
		const wchar_t * kText = L"Exit";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
		label->AlignCenter(rect->width(), rect->height());
	}

	// Defeat menu
	defeat_board_ = new sht::utility::ui::VerticalBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.4f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderNever // u32 flags
			);
		defeat_board_->AttachWidget(rect);
		wchar_t text[20] = {L'\0'};
		if (game_settings_->num_players == 1)
			wcscpy(text, L"You have lost");
		else
		{
			wcscpy(text, L"Player 1 has lost");
			text[7] += current_player_index_;
		}
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(text)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(text);
		label->AlignCenter(rect->width(), rect->height());
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.3f, // y
			0.2f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		defeat_board_->AttachWidget(rect);
		const wchar_t * kText = L"with score:";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.07f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.35f, // x
			0.3f, // y
			0.05f, // width
			0.05f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		defeat_board_->AttachWidget(rect);
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.07f, // text height
			4, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		defeat_score_label_ = label;
		rect->AttachWidget(label);
	}
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.0f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		defeat_board_->AttachWidget(rect);
		defeat_exit_rect_ = rect;
		const wchar_t * kText = L"Exit";
		label = new sht::utility::ui::Label(renderer_, text_shader_, font_,
			vec4(0.2f, 0.2f, 0.2f, 1.0f), // color
			0.1f, // text height
			wcslen(kText)+1, // buffer size
			0.0f, // x
			0.0f, // y
			(u32)sht::utility::ui::Flags::kRenderAlways // flags
			);
		rect->AttachWidget(label);
		label->SetText(kText);
		label->AlignCenter(rect->width(), rect->height());
	}
}
void GameScene::SetupCamera()
{
	// Should be set up after physics objects creation
	camera_manager_ = new sht::utility::CameraManager();
	switch (game_settings_->camera_mode)
	{
	case GameCameraMode::kOverview:
		camera_manager_->MakeFree(vec3(0.0f, 3000.0f, 0.0f), quat(vec3(0.0f, 0.0f, -1.0f), sht::math::kPi * 0.5f));
		break;
	case GameCameraMode::kAttached:
		UpdateCamera();
		break;
	default:
		assert(!"Camera mode hasn't been set");
		break;
	}
}
void GameScene::UpdateCamera()
{
	if (GameCameraMode::kAttached == game_settings_->camera_mode)
	{
		const float kCameraAngle = 0.5f; // radians
		const float kCameraDistance = 1000.0f;
		quat horizontal(vec3(0.0f, -1.0f, 0.0f), cue_alpha_);
		quat vertical(vec3(0.0f, 0.0f, -1.0f), kCameraAngle);
		quat orient(horizontal * vertical);
		sht::physics::Object * cue_ball = balls_[0];
		camera_manager_->MakeAttached(orient, cue_ball->GetPositionPtr(), kCameraDistance);
	}
}