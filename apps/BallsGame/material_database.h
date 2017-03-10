#pragma once
#ifndef __MATERIAL_DATABASE_H__
#define __MATERIAL_DATABASE_H__

#include "resource_database.h"

#include "math/vector.h"

class Material;
class ShaderDatabase;
class TextureDatabase;
namespace sht {
	namespace graphics {
		class Renderer;
	}
}

class MaterialDatabase : public ResourceDatabase<Material> {
public:
	MaterialDatabase(sht::graphics::Renderer * renderer, ShaderDatabase * shader_db, TextureDatabase * texture_db,
		const sht::math::Vector3 * light_pos_eye);
	~MaterialDatabase();

	void Load(const std::string& name) override;
	void Unload(const std::string& name) override;

	void LoadAll() override;
	void UnloadAll() override;

private:
	Material * GetMaterialByName(const std::string& name);

	sht::graphics::Renderer * renderer_;
	ShaderDatabase * shader_db_;
	TextureDatabase * texture_db_;
	const sht::math::Vector3 * light_pos_eye_;
};

#endif