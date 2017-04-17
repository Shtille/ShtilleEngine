#include "texture_database.h"

#include "graphics/include/renderer/renderer.h"
#include "utility/include/ini_file.h"

#include <cstring>

class TextureIniFileReader final : public sht::utility::IniFileReaderInterface {
public:
	TextureIniFileReader(TextureDatabase * database)
	: database_(database)
	, interesed_section_(false)
	{
	}
    void OnSection(const char* section_name) final
    {
        interesed_section_ = (strcmp(section_name, "textures") == 0);
    }
    void OnPair(const char* key, const char* value) final
    {
    	if (interesed_section_)
    		database_->table_.insert(std::make_pair(key, value));
    }
private:
	TextureDatabase * database_;
	bool interesed_section_;
};

TextureDatabase::TextureDatabase(sht::graphics::Renderer * renderer)
: renderer_(renderer)
{
	// Fill the matching table
	sht::utility::IniFile file;
	if (file.OpenForRead("data/ini/apps/BallsGame/app.ini"))
	{
		TextureIniFileReader reader(this);
		file.Read(&reader);
		file.Close();
	}
}
TextureDatabase::~TextureDatabase()
{
	UnloadAll();
}
void TextureDatabase::Load(const std::string& name)
{
	sht::graphics::Texture * texture = GetTextureByName(name);
	assert(texture && "texture doesn't exist in table");
	Pair pair(0, texture);
	container_.insert(std::make_pair(name, pair));
}
void TextureDatabase::Unload(const std::string& name)
{
	auto it = container_.find(name);
	auto& pair = it->second;
	renderer_->DeleteTexture(pair.second);
	container_.erase(it);
}
void TextureDatabase::LoadAll()
{
	for (auto& table_pair : table_)
	{
		const std::string& name = table_pair.first;
		const char * filename = table_pair.second.c_str();
		sht::graphics::Texture * texture;
		renderer_->AddTexture(texture, filename,
			sht::graphics::Texture::Wrap::kClampToEdge,
        	sht::graphics::Texture::Filter::kTrilinearAniso);
		assert(texture);
		Pair pair(0, texture);
		container_.insert(std::make_pair(name, pair));
	}
}
void TextureDatabase::UnloadAll()
{
	for (auto& c_pair : container_)
	{
		auto& pair = c_pair.second;
		renderer_->DeleteTexture(pair.second);
	}
	container_.clear();
}
sht::graphics::Texture * TextureDatabase::GetTextureByName(const std::string& name)
{
	auto it = table_.find(name);
	if (it == table_.end())
		return nullptr;
	const char * filename = it->second.c_str();
	sht::graphics::Texture * texture;
	renderer_->AddTexture(texture, filename,
		sht::graphics::Texture::Wrap::kClampToEdge,
        sht::graphics::Texture::Filter::kTrilinearAniso);
	return texture;
}