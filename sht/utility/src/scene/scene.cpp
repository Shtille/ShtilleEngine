#include "../../include/scene/scene.h"

namespace sht {
	namespace utility {

		Scene::Scene(sht::graphics::Renderer * renderer)
		: renderer_(renderer)
		, next_(nullptr)
		{

		}
		Scene::~Scene()
		{

		}
		void Scene::SetNextScene(Scene * scene)
		{
			next_ = scene;
		}
		Scene * Scene::next()
		{
			return next_;
		}
		void Scene::UpdatePhysics(float sec)
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
		ResourceID Scene::AddResourceIdByName(sht::utility::StringId string_id)
		{
			ResourceManager * resource_manager = ResourceManager::GetInstance();
			ResourceID id = resource_manager->GetResourceIdByName(string_id);
			resources_.push_back(id);
			return id;
		}
		void Scene::OnChar(unsigned short code)
		{

		}
		void Scene::OnKeyDown(sht::PublicKey key, int mods)
		{

		}
		void Scene::OnMouseDown(sht::MouseButton button, int modifiers)
		{

		}
		void Scene::OnMouseUp(sht::MouseButton button, int modifiers)
		{

		}
		void Scene::OnMouseMove(float x, float y)
		{
			
		}

	} // namespace utility
} // namespace sht