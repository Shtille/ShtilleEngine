#include "menu_scene.h"

#include "utility/include/event.h"

MenuScene::MenuScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener)
: Scene(renderer)
, event_listener_(event_listener)
, text_shader_(nullptr)
, font_(nullptr)
, text_(nullptr)
{
	// Register resources to load automatically on scene change
	text_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_text"));
	font_id_ = AddResourceIdByName(ConstexprStringId("font_good_dog"));
}
MenuScene::~MenuScene()
{
	Unload();
}
void MenuScene::Update()
{
}
void MenuScene::Render()
{
	renderer_->DisableDepthTest();

	// Draw text for debug purposes
    text_shader_->Bind();
    text_shader_->Uniform1i("u_texture", 0);
    text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
    text_->Render();

    renderer_->EnableDepthTest();
}
void MenuScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));

	text_ = sht::graphics::StaticText::Create(renderer_, font_, 0.05f, 0.1f, 0.8f, L"menu");
}
void MenuScene::Unload()
{
	if (text_)
	{
		delete text_;
		text_ = nullptr;
	}
}
void MenuScene::OnKeyDown(sht::PublicKey key, int mods)
{
	if (key == sht::PublicKey::kSpace)
	{
		sht::utility::Event event(ConstexprStringId("menu_scene_finished"));
		event_listener_->OnEvent(&event);
	}
}