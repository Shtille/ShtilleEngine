#include "scene.h"

Scene::Scene()
{

}
Scene::~Scene()
{

}
void Scene::Load()
{

}
void Scene::Unload()
{

}
void Scene::RequestLoad()
{
	ResourceManager * resource_manager = ResourceManager::GetInstance();
	for (auto resource_id : resources_)
		resource_manager->RequestLoad(resource_id);
}
void Scene::RequestUnload()
{
	ResourceManager * resource_manager = ResourceManager::GetInstance();
	for (auto resource_id : resources_)
		resource_manager->RequestUnload(resource_id);
}
ResourceID Scene::AddResourceIdByName(StringId string_id)
{
	ResourceManager * resource_manager = ResourceManager::GetInstance();
	ResourceID id = resource_manager->GetResourceIdByName(string_id);
	resources_.push_back(id);
	return id;
}