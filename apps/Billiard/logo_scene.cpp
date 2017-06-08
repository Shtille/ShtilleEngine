#include "logo_scene.h"

#include "utility/include/event.h"

namespace {
	const float kTimerInterval = 7.0f;
}

LogoScene::LogoScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener)
: Scene(renderer)
, event_listener_(event_listener)
, opacity_(0.0f)
, text_shader_(nullptr)
, font_(nullptr)
, fps_text_(nullptr)
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	timer_ = time_manager->AddTimer(kTimerInterval);

	// Register resources to load automatically on scene change
	text_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_text"));
	font_id_ = AddResourceIdByName(ConstexprStringId("font_good_dog"));
}
LogoScene::~LogoScene()
{
	Unload();
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	time_manager->RemoveTimer(timer_);
}
void LogoScene::Update()
{
	// Set opacity value
	const float kFadeInInterval = 2.0f;
	const float kFadeOutInterval = 2.0f;
	if (timer_->time() < kFadeInInterval)
		opacity_ = timer_->time() / kFadeInInterval;
	else if (timer_->time() >= kFadeInInterval && timer_->time() < kTimerInterval - kFadeOutInterval)
		opacity_ = 1.0f;
	else if (timer_->time() >= kTimerInterval - kFadeOutInterval && timer_->time() < kTimerInterval)
		opacity_ = (kTimerInterval - timer_->time()) / kFadeOutInterval;
	else
		opacity_ = 0.0f;

	if (!timer_->enabled())
		timer_->Start();
	if (timer_->HasExpired())
	{
		timer_->Stop();
		sht::utility::Event event(ConstexprStringId("logotype_scene_finished"));
		event_listener_->OnEvent(&event);
	}
}
void LogoScene::Render()
{
	renderer_->DisableDepthTest();

	// Draw FPS
    text_shader_->Bind();
    text_shader_->Uniform1i("u_texture", 0);
    text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, opacity_);
    fps_text_->SetText(font_, 0.0f, 0.8f, 0.05f, L"logotype");
    fps_text_->Render();

    renderer_->EnableDepthTest();
}
void LogoScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));

	fps_text_ = sht::graphics::DynamicText::Create(renderer_, 30);
}
void LogoScene::Unload()
{
	if (fps_text_)
	{
		delete fps_text_;
		fps_text_ = nullptr;
	}
}