#pragma once
#ifndef __MODEL_DATABASE_H__
#define __MODEL_DATABASE_H__

#include "resource_database.h"

namespace sht {
	namespace graphics {
		class Model;
		class Renderer;
	}
}

class ModelDatabase : public ResourceDatabase<sht::graphics::Model> {
public:
	ModelDatabase(sht::graphics::Renderer * renderer);
	~ModelDatabase();

	void Load(const std::string& name) override;
	void Unload(const std::string& name) override;

	void LoadAll() override;
	void UnloadAll() override;

private:
	sht::graphics::Model * GetModelByName(const std::string& name);

	sht::graphics::Renderer * renderer_;
};

#endif