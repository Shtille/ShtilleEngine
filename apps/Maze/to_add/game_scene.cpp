#include "game_scene.h"

#include "utility/include/event.h"
#include "physics/include/physics_object.h"

#include <cmath> // for sinf and cosf
#include <cstdio>
#include <cstring>
#include <cassert>

GameScene::GameScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener,
	const GameSettings * game_settings)
: Scene(renderer)
, event_listener_(event_listener)
, game_settings_(game_settings)
, ui_root_(nullptr)
, menu_board_(nullptr)
, victory_board_(nullptr)
, defeat_board_(nullptr)
, text_shader_(nullptr)
, object_shader_(nullptr)
, ball_shader_(nullptr)
, font_(nullptr)
, ball_mesh_(nullptr)
, maze_mesh_(nullptr)
, fps_text_(nullptr)
, camera_manager_(nullptr)
, physics_(nullptr)
, ball_(nullptr)
, maze_(nullptr)
{
	// Add timers
	const float kBallRequestInterval = 1.0f;
	pocket_entrance_timer_ = sht::system::TimeManager::GetInstance()->AddTimer(kBallRequestInterval);

	// Register resources to load automatically on scene change
	text_shader_id_   					= AddResourceIdByName(ConstexprStringId("shader_text"));
	gui_shader_id_   					= AddResourceIdByName(ConstexprStringId("shader_gui"));
	maze_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_maze"));
	ball_shader_id_ 					= AddResourceIdByName(ConstexprStringId("shader_ball"));
	font_id_          					= AddResourceIdByName(ConstexprStringId("font_good_dog"));
	ball_mesh_id_     					= AddResourceIdByName(ConstexprStringId("mesh_ball"));
	maze_mesh_id_    					= AddResourceIdByName(ConstexprStringId("mesh_maze"));
}
GameScene::~GameScene()
{
	Unload();

	// Remove timers
	sht::system::TimeManager::GetInstance()->RemoveTimer(pocket_entrance_timer_);
}
void GameScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFixedFrameTime();

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

	maze_shader_->Bind();
	maze_shader_->Uniform3fv("u_light.color", kLightColor);

	ball_shader_->Bind();
	ball_shader_->Uniform1i("u_texture", 0);
	ball_shader_->Uniform3fv("u_light.color", kLightColor);

	ball_shader_->Unbind();
}
void GameScene::BindShaderVariables()
{
	maze_shader_->Bind();
	maze_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	maze_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ball_shader_->Bind();
	ball_shader_->UniformMatrix4fv("u_projection_view", projection_view_matrix_);
	ball_shader_->Uniform3fv("u_eye_position", *camera_manager_->position());

	ball_shader_->Unbind();
}
void GameScene::RenderMaze()
{
	maze_shader_->Bind();
	maze_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	maze_mesh_->Render();
}
void GameScene::RenderBall()
{
	renderer_->ChangeTexture(ball_textures_[i]);

	renderer_->PushMatrix();
	renderer_->MultMatrix(ball_->matrix());
	ball_shader_->Bind();
	ball_shader_->UniformMatrix4fv("u_model", renderer_->model_matrix());
	ball_mesh_->Render();
	renderer_->PopMatrix();

	renderer_->ChangeTexture(nullptr);
}
void GameScene::RenderObjects()
{
	RenderMaze();
	RenderBall();
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

	// Render UI
	gui_shader_->Bind();
	if (!menu_board_->IsPosMax())
	{
		if (menu_board_->IsPosMin())
			menu_board_->RenderAll(); // render entire tree
		else
			menu_board_->Render(); // render only board rect (smart hack for labels :D)
	}
	if (!victory_board_->IsPosMax())
	{
		if (victory_board_->IsPosMin())
			victory_board_->RenderAll(); // render entire tree
		else
			victory_board_->Render(); // render only board rect (smart hack for labels :D)
	}
	if (!defeat_board_->IsPosMax())
	{
		if (defeat_board_->IsPosMin())
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
	maze_mesh_ = dynamic_cast<sht::graphics::ComplexMesh *>(resource_manager->GetResource(maze_mesh_id_));

	// Create text objects
	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
	status_text_ = sht::graphics::DynamicText::Create(renderer_, 64);
	SetStatus(L"start");

	// Create physics
	physics_ = new sht::physics::Engine(20 /* max sub steps */, 0.002f /* fixed time step */);
	//physics_->SetGravity(vec3(0.0f, -9800.0f, 0.0f));

	// Setup physics objects
	{
		ball_ = physics_->AddSphere(vec3(0.0f, 0.0f, 0.0f), 0.150f, ball_size_);
		ball_->SetFriction(0.28f);
		ball_->SetRollingFriction(0.2f);
		ball_->SetSpinningFriction(0.5f);
		ball_->SetRestitution(0.98f);
	}
	{
		sht::graphics::MeshVerticesEnumerator enumerator(maze_mesh_);
		sht::physics::Object * object = physics_->AddMesh(vec3(0.0f, 0.0f, 0.0f), 0.0f, &enumerator);
		object->SetFriction(0.71f);
		object->SetRestitution(0.5f);
	}

	// Create meshes that have been loaded earlier
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	ball_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
	ball_mesh_->MakeRenderable();
	maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
	maze_mesh_->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
	maze_mesh_->MakeRenderable();

	SetupCamera();

	CreateMenu();

	PrepareBeginning();
	RequestRackRemove(); // later can be moved after table look overview

	BindShaderConstants();
}
void GameScene::Unload()
{
	if (ui_root_)
	{
		delete ui_root_;
		ui_root_ = nullptr;
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
}
void GameScene::SetStatus(const wchar_t* text)
{
	status_text_->SetText(font_, 0.0f, 0.9f, 0.05f, text);
}
void GameScene::PrepareBeginning()
{
	// Some parameters initialization here
}
void GameScene::CheckTimerEvents()
{
	if (pocket_entrance_timer_->enabled() && pocket_entrance_timer_->HasExpired())
	{
		// TODO: check victory condition here
		pocket_entrance_timer_->Reset();
	}
}
void GameScene::OnPlayerWon()
{
	victory_board_->Move();
}
void GameScene::OnPlayerLost()
{
	defeat_board_->Move();
}
void GameScene::OnKeyDown(sht::PublicKey key, int mods)
{
	if (sht::PublicKey::kEscape == key)
	{
		menu_board_->Move();
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
void GameScene::OnMouseUp(sht::MouseButton button, int modifiers, float x, float y)
{
}
void GameScene::OnMouseMove(float x, float y)
{
	if (menu_board_->IsPosMin())
		menu_board_->SelectAll(x, y);
	if (victory_board_->IsPosMin())
		victory_board_->SelectAll(x, y);
	if (defeat_board_->IsPosMin())
		defeat_board_->SelectAll(x, y);
}
void GameScene::CreateMenu()
{
	sht::utility::ui::Rect * rect;
	sht::utility::ui::Label * label;

	ui_root_ = new sht::utility::ui::Widget();

	// Main menu
	menu_board_ = new sht::utility::ui::ColoredBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		true, // bool is vertical
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	ui_root_->AttachWidget(menu_board_);
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
	victory_board_ = new sht::utility::ui::ColoredBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		true, // bool is vertical
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	ui_root_->AttachWidget(victory_board_);
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
		wcscpy(text, L"You have won");
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
	defeat_board_ = new sht::utility::ui::ColoredBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.25f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		true, // bool is vertical
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	ui_root_->AttachWidget(defeat_board_);
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
		wcscpy(text, L"You have lost");
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
	camera_manager_->MakeFree(vec3(0.0f, 10.0f, 0.0f), quat(vec3(0.0f, 0.0f, -1.0f), sht::math::kPi * 0.5f));
}