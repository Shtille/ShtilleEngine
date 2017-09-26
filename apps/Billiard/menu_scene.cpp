#include "menu_scene.h"

#include "system/include/time/time_manager.h"
#include "utility/include/event.h"
#include "utility/include/ui/label.h"

#include <cstring>

MenuScene::MenuScene(sht::graphics::Renderer * renderer, sht::utility::EventListenerInterface * event_listener,
	const GameSettings * game_settings)
: Scene(renderer)
, event_listener_(event_listener)
, game_settings_(game_settings)
, text_shader_(nullptr)
, font_(nullptr)
, text_(nullptr)
, main_board_(nullptr)
, options_board_(nullptr)
{
	// Register resources to load automatically on scene change
	text_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_text"));
	gui_shader_id_ = AddResourceIdByName(ConstexprStringId("shader_gui"));
	font_id_ = AddResourceIdByName(ConstexprStringId("font_good_dog"));
}
MenuScene::~MenuScene()
{
	Unload();
}
void MenuScene::Update()
{
	sht::system::TimeManager * time_manager = sht::system::TimeManager::GetInstance();
	float frame_time = time_manager->GetFixedFrameTime();

	main_board_->UpdateAll(frame_time);
	options_board_->UpdateAll(frame_time);
}
void MenuScene::Render()
{
	renderer_->DisableDepthTest();

	// Draw text for debug purposes
	text_shader_->Bind();
	text_shader_->Uniform1i("u_texture", 0);
	text_shader_->Uniform4f("u_color", 1.0f, 0.5f, 1.0f, 1.0f);
	text_->Render();

	// Render UI
	gui_shader_->Bind();
	if (main_board_->IsPosDown())
		main_board_->RenderAll(); // render entire tree
	else
		main_board_->Render(); // render only board rect (smart hack for labels :D)
	if (options_board_->IsPosDown())
		options_board_->RenderAll(); // render entire tree
	else
		options_board_->Render();

	renderer_->EnableDepthTest();
}
void MenuScene::Load()
{
	// Link resources IDs with its resource pointers
	sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
	text_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(text_shader_id_));
	gui_shader_ = dynamic_cast<sht::graphics::Shader *>(resource_manager->GetResource(gui_shader_id_));
	font_ = dynamic_cast<sht::graphics::Font *>(resource_manager->GetResource(font_id_));

	text_ = sht::graphics::StaticText::Create(renderer_, font_, 0.05f, 0.1f, 0.8f, L"menu");

	// Create menu
	CreateMenu();

	// Make main board to move down
	if (main_board_->IsPosUp())
		main_board_->Move();
}
void MenuScene::Unload()
{
	if (main_board_)
	{
		delete main_board_;
		main_board_ = nullptr;
	}
	if (options_board_)
	{
		delete options_board_;
		options_board_ = nullptr;
	}
	if (text_)
	{
		delete text_;
		text_ = nullptr;
	}
}
void MenuScene::OnMouseDown(sht::MouseButton button, int modifiers)
{
	if (sht::MouseButton::kLeft == button)
	{
		if (new_game_rect_->active())
		{
			sht::utility::Event event(ConstexprStringId("menu_scene_finished"));
			event_listener_->OnEvent(&event);
		}
		else if (options_rect_->active())
		{
			options_rect_->set_active(false);
			main_board_->Move();
			options_board_->Move();
		}
		else if (exit_rect_->active())
		{
			sht::utility::Event event(ConstexprStringId("application_exit_requested"));
			event_listener_->OnEvent(&event);
		}
		else if (options_exit_rect_->active())
		{
			options_exit_rect_->set_active(false);
			options_board_->Move();
			main_board_->Move();
		}
	}
}
void MenuScene::OnMouseMove(float x, float y)
{
	if (main_board_->IsPosDown())
		main_board_->SelectAll(x, y);
	if (options_board_->IsPosDown())
		options_board_->SelectAll(x, y);
}
void MenuScene::CreateMenu()
{
	sht::utility::ui::Rect * rect;
	sht::utility::ui::Label * label;

	// Main menu
	main_board_ = new sht::utility::ui::VerticalBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.2f, // f32 left
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
		main_board_->AttachWidget(rect);
		new_game_rect_ = rect;
		const wchar_t * kText = L"New Game";
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
			0.2f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		main_board_->AttachWidget(rect);
		options_rect_ = rect;
		const wchar_t * kText = L"Options";
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
		main_board_->AttachWidget(rect);
		exit_rect_ = rect;
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

	// Options menu
	options_board_ = new sht::utility::ui::VerticalBoard(renderer_, gui_shader_,
		vec4(0.5f, 0.5f, 0.3f, 0.3f), // vec4 color
		0.5f, // f32 width
		0.5f, // f32 height
		0.2f, // f32 left
		0.1f, // f32 hmin
		1.0f, // f32 hmax
		0.6f, // f32 velocity
		false, // bool is_down
		(u32)sht::utility::ui::Flags::kRenderAlways // u32 flags
		);
	{
		rect = new sht::utility::ui::RectColored(renderer_, gui_shader_, vec4(0.5f),
			0.05f, // x
			0.3f, // y
			0.4f, // width
			0.1f, // height
			(u32)sht::utility::ui::Flags::kRenderIfActive | (u32)sht::utility::ui::Flags::kSelectable // u32 flags
			);
		options_board_->AttachWidget(rect);
		options_exit_rect_ = rect;
		const wchar_t * kText = L"Number of players";
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
		options_board_->AttachWidget(rect);
		options_exit_rect_ = rect;
		const wchar_t * kText = L"Back";
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