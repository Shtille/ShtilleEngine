#include "game_scene_manager.h"

#include "logo_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game_scene.h"

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
	const char* filename = scene_manager->GetResourcePath(id);
	printf("loading mesh (id = %i): %s\n", id, filename);
	sht::graphics::ComplexMesh * mesh = new sht::graphics::ComplexMesh(renderer);
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
{
	RegisterAllResources(); // should be done before scenes creation

	logo_scene_ = new LogoScene(renderer, this);
	menu_scene_ = new MenuScene(renderer, this);
	loading_scene_ = new LoadingScene(renderer, this);
	game_scene_ = new GameScene(renderer);

	// Make loading scene to load with menu scene
	menu_scene_->SetNextScene(loading_scene_);

	RequestImmediateTransition(logo_scene_);
}
GameSceneManager::~GameSceneManager()
{
	delete game_scene_;
	delete loading_scene_;
	delete menu_scene_;
	delete logo_scene_;
}
sht::graphics::Renderer * GameSceneManager::renderer()
{
	return renderer_;
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
	}
}