#pragma once
#ifndef __TEXTURE_DATABASE_H__
#define __TEXTURE_DATABASE_H__

#include "resource_database.h"

namespace sht {
	namespace graphics {
		class Texture;
		class Renderer;
	}
}

class TextureDatabase : public ResourceDatabase<sht::graphics::Texture> {
	typedef std::map<std::string, std::string> MatchingTable;
public:
	TextureDatabase(sht::graphics::Renderer * renderer);
	~TextureDatabase();

	void Load(const std::string& name) override;
	void Unload(const std::string& name) override;

	void LoadAll() override;
	void UnloadAll() override;

private:
	sht::graphics::Texture * GetTextureByName(const std::string& name);

	sht::graphics::Renderer * renderer_;
	MatchingTable table_;
};

#endif