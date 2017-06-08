#include "loading_scene.h"

#include "utility/include/event.h"

#include <cstdio>

LoadingScene::LoadingScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener)
: Scene(renderer)
, event_listener_(event_listener)
, count_(0)
, counter_(0)
, text_shader_(nullptr)
, font_(nullptr)
, text_(nullptr)
{
	// Register resources to load automatically on scene change
	text_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_text"));
	font_id_ = AddResourceIdByName(ConstexprStringId("font_good_dog"));
}
LoadingScene::~LoadingScene()
{
	Unload();
}
void LoadingScene::Update()
{
}
void LoadingScene::Render()
{
	renderer_->DisableDepthTest();

	// Draw text for debug purposes
    text_shader_->Bind();
    text_shader_->Uniform1i("u_texture", 0);
    text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
    text_->Render();

    renderer_->EnableDepthTest();
}
void LoadingScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));

	text_ = sht::graphics::StaticText::Create(renderer_, font_, 0.05f, 0.1f, 0.8f, L"loading scene");

	sht::utility::Event event(ConstexprStringId("loading_scene_loaded"));
	event_listener_->OnEvent(&event);
}
void LoadingScene::Unload()
{
	if (text_)
	{
		delete text_;
		text_ = nullptr;
	}
}
void LoadingScene::OnObtainCount(int count)
{
	count_ = count;
	counter_ = 0;
	printf("resources to load: %i\n", count_);
}
void LoadingScene::OnStep()
{
	++counter_;
	printf("loaded: %i/%i\n", counter_, count_);
}
void LoadingScene::OnFinish()
{
	// Nothing to do
}