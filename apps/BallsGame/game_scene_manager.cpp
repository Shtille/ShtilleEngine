#include "game_scene_manager.h"

#include "logo_scene.h"
#include "game_scene.h"

#include "utility/include/ini_file.h"
#include "utility/include/event.h"

static void * TextureLoadingFunc(void * user_data, ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Texture * texture;
	const char* filename = scene_manager->GetResourcePath(id);
	renderer->AddTexture(texture, filename,
		sht::graphics::Texture::Wrap::kClampToEdge,
        sht::graphics::Texture::Filter::kTrilinearAniso);
	return reinterpret_cast<void*>(texture);
}
static void TextureUnloadingFunc(void * user_data, void * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Texture * texture = reinterpret_cast<sht::graphics::Texture *>(resource);
	renderer->DeleteTexture(texture);
}
static void * ShaderLoadingFunc(void * user_data, ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Shader * shader;
	const char* filename = scene_manager->GetResourcePath(id);
	renderer->AddShader(shader, filename);
	return reinterpret_cast<void*>(shader);
}
static void ShaderUnloadingFunc(void * user_data, void * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Shader * shader = reinterpret_cast<sht::graphics::Shader *>(resource);
	renderer->DeleteShader(shader);
}
static void * ModelLoadingFunc(void * user_data, ResourceID id)
{

}
static void ModelUnloadingFunc(void * user_data, void * resource)
{

}
static void * FontLoadingFunc(void * user_data, ResourceID id)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Font * font;
	const char* filename = scene_manager->GetResourcePath(id);
	renderer->AddFont(font, filename);
	return reinterpret_cast<void*>(font);
}
static void FontUnloadingFunc(void * user_data, void * resource)
{
	GameSceneManager * scene_manager = reinterpret_cast<GameSceneManager *>(user_data);
	sht::graphics::Renderer * renderer = scene_manager->renderer();
	sht::graphics::Font * font = reinterpret_cast<sht::graphics::Font *>(resource);
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
    	ResourceID id = -1;
    	StringId string_id = MakeStringId(key);
    	void * user_data = reinterpret_cast<void*>(scene_manager_);
    	ResourceManager * resource_manager = ResourceManager::GetInstance();
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
	RegisterAllResources(); // should be done before scenes

	logo_scene_ = new LogoScene(this);
	game_scene_ = new GameScene();

	RequestImmediateTransition(logo_scene_);
}
GameSceneManager::~GameSceneManager()
{
	delete game_scene_;
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
	if (file.OpenForRead("data/ini/apps/BallsGame/app.ini"))
	{
		IniFileReader reader(this);
		file.Read(&reader);
		file.Close();
	}
}
const char* GameSceneManager::GetResourcePath(ResourceID id)
{
	return resources_paths_[id].c_str();
}
void GameSceneManager::AddResourcePath(ResourceID id, const char* path)
{
	resources_paths_.insert(std::make_pair(id, std::string(path)));
}
void GameSceneManager::OnEvent(const sht::utility::Event * event)
{
	switch (event->type())
    {
    case SID("logotype_scene_finished"):
        RequestImmediateTransition(game_scene_);
        break;
    }
}