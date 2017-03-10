#include "material_database.h"

#include "simple_material.h"

MaterialDatabase::MaterialDatabase(sht::graphics::Renderer * renderer, ShaderDatabase * shader_db, TextureDatabase * texture_db,
		const sht::math::Vector3 * light_pos_eye)
: renderer_(renderer)
, shader_db_(shader_db)
, texture_db_(texture_db)
, light_pos_eye_(light_pos_eye)
{
}
MaterialDatabase::~MaterialDatabase()
{
	UnloadAll();
}
void MaterialDatabase::Load(const std::string& name)
{
	Material * material = GetMaterialByName(name);
	assert(material && "material doesn't exist in table");
	Pair pair(0, material);
	container_.insert(std::make_pair(name, pair));
}
void MaterialDatabase::Unload(const std::string& name)
{
	auto it = container_.find(name);
	auto& pair = it->second;
	delete pair.second;
	container_.erase(it);
}
void MaterialDatabase::LoadAll()
{
	assert(!"there's no need in this");
}
void MaterialDatabase::UnloadAll()
{
	for (auto& c_pair : container_)
	{
		auto& pair = c_pair.second;
		delete pair.second;
	}
	container_.clear();
}
Material * MaterialDatabase::GetMaterialByName(const std::string& name)
{
	return new SimpleMaterial(renderer_, shader_db_, texture_db_, light_pos_eye_, name);
}