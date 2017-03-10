#include "shader_database.h"

#include "graphics/include/renderer/renderer.h"

ShaderDatabase::ShaderDatabase(sht::graphics::Renderer * renderer)
: renderer_(renderer)
{
	// Fill the matching table
	table_.insert(std::make_pair("object_shader", "data/shaders/apps/PhysicsTest/shader"));
}
ShaderDatabase::~ShaderDatabase()
{
	UnloadAll();
}
void ShaderDatabase::Load(const std::string& name)
{
	sht::graphics::Shader * shader = GetShaderByName(name);
	assert(shader && "shader doesn't exist in table");
	Pair pair(0, shader);
	container_.insert(std::make_pair(name, pair));
}
void ShaderDatabase::Unload(const std::string& name)
{
	auto it = container_.find(name);
	auto& pair = it->second;
	renderer_->DeleteShader(pair.second);
	container_.erase(it);
}
void ShaderDatabase::LoadAll()
{
	for (auto& table_pair : table_)
	{
		const std::string& name = table_pair.first;
		const char * filename = table_pair.second.c_str();
		sht::graphics::Shader * shader;
		renderer_->AddShader(shader, filename);
		assert(shader);
		Pair pair(0, shader);
		container_.insert(std::make_pair(name, pair));
	}
}
void ShaderDatabase::UnloadAll()
{
	for (auto& c_pair : container_)
	{
		auto& pair = c_pair.second;
		renderer_->DeleteShader(pair.second);
	}
	container_.clear();
}
sht::graphics::Shader * ShaderDatabase::GetShaderByName(const std::string& name)
{
	auto it = table_.find(name);
	if (it == table_.end())
		return nullptr;
	const char * filename = it->second.c_str();
	sht::graphics::Shader * shader;
	renderer_->AddShader(shader, filename);
	return shader;
}