#include "model_database.h"

#include "graphics/include/model/sphere_model.h"
#include "graphics/include/model/cube_model.h"

ModelDatabase::ModelDatabase(sht::graphics::Renderer * renderer)
: renderer_(renderer)
{
}
ModelDatabase::~ModelDatabase()
{
	UnloadAll();
}
void ModelDatabase::Load(const std::string& name)
{
	sht::graphics::Model * model = GetModelByName(name);
	assert(model && "model doesn't exist in the table");
	Pair pair(0, model);
	container_.insert(std::make_pair(name, pair));
}
void ModelDatabase::Unload(const std::string& name)
{
	auto it = container_.find(name);
	auto& pair = it->second;
	delete pair.second;
	container_.erase(it);
}
void ModelDatabase::LoadAll()
{
	assert(!"there's no need in this");
}
void ModelDatabase::UnloadAll()
{
	for (auto& c_pair : container_)
	{
		auto& pair = c_pair.second;
		delete pair.second;
	}
	container_.clear();
}
sht::graphics::Model * ModelDatabase::GetModelByName(const std::string& name)
{
	sht::graphics::Model * model;

	// Hardcoded shit
	if (name == "sphere")
	{
		model = new sht::graphics::SphereModel(renderer_, 20, 10);
		model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
		model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
		model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
		model->Create();
	}
	else if (name == "box")
	{
		model = new sht::graphics::CubeModel(renderer_);
        model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kVertex, 3));
        model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kNormal, 3));
        model->AddFormat(sht::graphics::VertexAttribute(sht::graphics::VertexAttribute::kTexcoord, 2));
        model->Create();
	}
	else
	{
		model = nullptr;
	}
	if (model && !model->MakeRenderable())
	{
		delete model;
		return nullptr;
	}
	return model;
}