#include "game_scene.h"

#include "utility/include/event.h"
#include "system/include/time/time_manager.h"

GameScene::GameScene(sht::graphics::Renderer * renderer)
: Scene(renderer)
, text_shader_(nullptr)
, font_(nullptr)
, fps_text_(nullptr)
, camera_manager_(nullptr)
, physics_(nullptr)
{
	// Register resources to load automatically on scene change
	text_shader_id_ = AddResourceIdByName(SID("shader_text"));
	font_id_ = AddResourceIdByName(SID("font_good_dog"));
}
GameScene::~GameScene()
{
	Unload();
}
void GameScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFrameTime();

	// Update physics engine
	physics_->Update(frame_time);

	// Camera should be updated after physics
	camera_manager_->Update(frame_time);

	// Update view matrix
	renderer_->SetViewMatrix(camera_manager_->view_matrix());
}
void GameScene::RenderObjects()
{

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
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));

	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);

	// Create camera attached to the controlled ball
	camera_manager_ = new sht::utility::CameraManager();
	auto cam_id = camera_manager_->Add(vec3(5.0f, 5.0f, 0.0f), vec3(0.0f));
	camera_manager_->SetCurrent(cam_id);
	camera_manager_->SetManualUpdate();

	// Create physics
	physics_ = new sht::physics::Engine();
}
void GameScene::Unload()
{
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
	if (fps_text_)
	{
		delete fps_text_;
		fps_text_ = nullptr;
	}
}