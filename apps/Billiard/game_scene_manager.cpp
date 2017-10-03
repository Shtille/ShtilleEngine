#include "game_scene_manager.h"

#include "logo_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"

#include "material_binder.h"

#include "graphics/include/model/complex_mesh.h"
#include "utility/include/ini_file.h"
#include "utility/include/event.h"

#include <cstdio>

static sht::graphics::Resource * TextureLoadingFunc(void * user_data, sht::utility::ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Texture * texture;
	const char* filename = scene_manager->GetResourcePath(id);
	printf("loading texture (id = %i): %s\n", id, filename);
	renderer->AddTexture(texture, filename,
		sht::graphics::Texture::Wrap::kClampToEdge,
		sht::graphics::Texture::Filter::kTrilinearAniso);
	return texture;
}
static void TextureUnloadingFunc(void * user_data, sht::graphics::Resource * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Texture * texture = dynamic_cast<sht::graphics::Texture *>(resource);
	printf("unloading texture\n");
	renderer->DeleteTexture(texture);
}
static sht::graphics::Resource * ShaderLoadingFunc(void * user_data, sht::utility::ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Shader * shader;
	const char* filename = scene_manager->GetResourcePath(id);
	printf("loading shader (id = %i): %s\n", id, filename);
	renderer->AddShader(shader, filename);
	return shader;
}
static void ShaderUnloadingFunc(void * user_data, sht::graphics::Resource * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Shader * shader = dynamic_cast<sht::graphics::Shader *>(resource);
	printf("unloading shader\n");
	renderer->DeleteShader(shader);
}
static sht::graphics::Resource * ModelLoadingFunc(void * user_data, sht::utility::ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	MaterialBinder * material_binder = scene_manager->material_binder();
	const char* filename = scene_manager->GetResourcePath(id);
	printf("loading mesh (id = %i): %s\n", id, filename);
	sht::graphics::ComplexMesh * mesh = new sht::graphics::ComplexMesh(renderer, material_binder);
	if (!mesh->LoadFromFile(filename))
	{
		delete mesh;
		mesh = nullptr;
		// Mesh loading has failed
	}
	return mesh;
}
static void ModelUnloadingFunc(void * user_data, sht::graphics::Resource * resource)
{
	printf("unloading mesh\n");
	sht::graphics::ComplexMesh * mesh = dynamic_cast<sht::graphics::ComplexMesh *>(resource);
	delete mesh;
}
static sht::graphics::Resource * FontLoadingFunc(void * user_data, sht::utility::ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Font * font;
	const char* filename = scene_manager->GetResourcePath(id);
	printf("loading font (id = %i): %s\n", id, filename);
	renderer->AddFont(font, filename);
	return font;
}
static void FontUnloadingFunc(void * user_data, sht::graphics::Resource * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Font * font = dynamic_cast<sht::graphics::Font *>(resource);
	printf("unloading font\n");
	renderer->DeleteFont(font);
}

class IniFileReader final : public sht::utility::IniFileReaderInterface {
public:
	enum SectionType {
		kTextures,
		kShaders,
		kModels,
		kFonts,
		kUnknown
	};

	IniFileReader(GameSceneManager * scene_manager)
	: scene_manager_(scene_manager)
	, section_type_(kUnknown)
	{
	}
	void OnSection(const char* section_name) final
	{
		if (strcmp(section_name, "textures") == 0)
			section_type_ = kTextures;
		else if (strcmp(section_name, "shaders") == 0)
			section_type_ = kShaders;
		else if (strcmp(section_name, "models") == 0)
			section_type_ = kModels;
		else if (strcmp(section_name, "fonts") == 0)
			section_type_ = kFonts;
		else
			section_type_ = kUnknown;
	}
	void OnPair(const char* key, const char* value) final
	{
		sht::utility::ResourceID id = -1;
		sht::utility::StringId string_id = RuntimeStringId(key);
		void * user_data = reinterpret_cast<void*>(scene_manager_);
		sht::utility::ResourceManager * resource_manager = sht::utility::ResourceManager::GetInstance();
		switch (section_type_)
		{
		case kTextures:
			id = resource_manager->RegisterResource(string_id, user_data, TextureLoadingFunc, TextureUnloadingFunc);
			break;
		case kShaders:
			id = resource_manager->RegisterResource(string_id, user_data, ShaderLoadingFunc, ShaderUnloadingFunc);
			break;
		case kModels:
			id = resource_manager->RegisterResource(string_id, user_data, ModelLoadingFunc, ModelUnloadingFunc);
			break;
		case kFonts:
			id = resource_manager->RegisterResource(string_id, user_data, FontLoadingFunc, FontUnloadingFunc);
			break;
		default:
			// Unrecognized section
			break;
		}
		if (id >= 0)
			scene_manager_->AddResourcePath(id, value);
	}
private:
	GameSceneManager * scene_manager_;
	SectionType section_type_;
};

GameSceneManager::GameSceneManager(sht::graphics::Renderer * renderer)
: renderer_(renderer)
, exit_requested_(false)
, fullscreen_toggle_requested_(false)
{
	RegisterAllResources(); // should be done before scenes creation

	SetupDefaultSettings();

	material_binder_ = new MaterialBinder();

	logo_scene_ = new LogoScene(renderer, this);
	menu_scene_ = new MenuScene(renderer, this, &game_settings_);
	loading_scene_ = new LoadingScene(renderer, this);
	game_scene_ = new GameScene(renderer, this, material_binder_, &game_settings_);

	// Make loading scene to load with menu scene
	menu_scene_->SetNextScene(loading_scene_);

	RequestImmediateTransition(menu_scene_); // logo_scene_
}
GameSceneManager::~GameSceneManager()
{
	delete game_scene_;
	delete loading_scene_;
	delete menu_scene_;
	delete logo_scene_;

	delete material_binder_;
}
sht::graphics::Renderer * GameSceneManager::renderer()
{
	return renderer_;
}
MaterialBinder * GameSceneManager::material_binder()
{
	return material_binder_;
}
bool GameSceneManager::exit_requested() const
{
	return exit_requested_;
}
bool GameSceneManager::fullscreen_toggle_requested() const
{
	return fullscreen_toggle_requested_;
}
void GameSceneManager::set_exit_requested(bool requested)
{
	exit_requested_ = requested;
}
void GameSceneManager::set_fullscreen_toggle_requested(bool requested)
{
	fullscreen_toggle_requested_ = requested;
}
void GameSceneManager::RegisterAllResources()
{
	// Also fill the table
	sht::utility::IniFile file;
	if (file.OpenForRead("data/ini/apps/Billiard/app.ini"))
	{
		IniFileReader reader(this);
		file.Read(&reader);
		file.Close();
	}
}
const char* GameSceneManager::GetResourcePath(sht::utility::ResourceID id)
{
	return resources_paths_[id].c_str();
}
void GameSceneManager::AddResourcePath(sht::utility::ResourceID id, const char* path)
{
	resources_paths_.insert(std::make_pair(id, std::string(path)));
}
void GameSceneManager::OnEvent(const sht::utility::Event * event)
{
	switch (event->type())
	{
	case ConstexprStringId("logotype_scene_finished"):
		RequestImmediateTransition(menu_scene_);
		break;
	case ConstexprStringId("menu_scene_finished"):
		RequestDeferredTransition(game_scene_, loading_scene_, loading_scene_);
		break;
	case ConstexprStringId("loading_scene_loaded"):
		// Detach loading scene upon loading
		menu_scene_->SetNextScene(nullptr);
		break;
	case ConstexprStringId("game_scene_exit_requested"):
		RequestImmediateTransition(menu_scene_);
		break;
	case ConstexprStringId("application_exit_requested"):
		exit_requested_ = true;
		break;
	case ConstexprStringId("application_fullscreen_toggle_requested"):
		fullscreen_toggle_requested_ = true;
		break;
	}
}
void GameSceneManager::SetupDefaultSettings()
{
	// Setup default game settings
	game_settings_.game_mode = GameMode::kSimplePool;
	game_settings_.num_players = 1;
	game_settings_.max_players = 2;
}