#pragma once
#ifndef __SHADER_DATABASE_H__
#define __SHADER_DATABASE_H__

#include "resource_database.h"

namespace sht {
	namespace graphics {
		class Shader;
		class Renderer;
	}
}

class ShaderDatabase : public ResourceDatabase<sht::graphics::Shader> {
	typedef std::map<std::string, std::string> MatchingTable;
	friend class ShaderIniFileReader;
public:
	ShaderDatabase(sht::graphics::Renderer * renderer);
	~ShaderDatabase();

	void Load(const std::string& name) override;
	void Unload(const std::string& name) override;

	void LoadAll() override;
	void UnloadAll() override;

private:
	sht::graphics::Shader * GetShaderByName(const std::string& name);

	sht::graphics::Renderer * renderer_;
	MatchingTable table_;
};

#endif