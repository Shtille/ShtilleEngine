#include "resource_manager.h"

#include <assert.h>

ResourceManager::ResourceManager()
: current_resource_id_(0)
{

}
ResourceManager::~ResourceManager()
{

}
ResourceID ResourceManager::RegisterResource(StringId string_id, void* user_data, ResourceLoadingFunc loading_func, ResourceUnloadingFunc unloading_func)
{
	ResourceID id = current_resource_id_;
	++current_resource_id_;
	ResourceInfo info;
	info.counter = 0;
	info.string_id = string_id;
	info.resource = nullptr;
	info.user_data = user_data;
	info.loading_func = loading_func;
	info.unloading_func = unloading_func;
	container_.insert(std::make_pair<ResourceID, ResourceInfo>(id, info));
}
void ResourceManager::UnregisterResource(ResourceID id)
{
	auto it = container_.find(id);
	if (it != container_.end())
	{
		ResourceInfo& info = it->second;
		assert(info.resource == nullptr);
	}
	container_.erase(it);
}
ResourceID ResourceManager::GetResourceIdByName(StringId string_id)
{
	for (auto& pair : container_)
	{
		ResourceInfo& info = pair.second;
		if (info.string_id == string_id)
			return pair.first;
	}
	assert(!"Resource hasn't been registered yet");
	return -1;
}
void * ResourceManager::GetResource(ResourceID id)
{
	auto it = container_.find(id);
	if (it != container_.end())
	{
		ResourceInfo& info = it->second;
		return info.resource;
	}
	else
	{
		assert(!"Resource hasn't been registered yet");
		return nullptr;
	}
}
void ResourceManager::RequestLoad(ResourceID id)
{
	auto it = container_.find(id);
	if (it != container_.end())
	{
		ResourceInfo& info = it->second;
		++info.counter;
	}
	else
	{
		assert(!"Resource hasn't been registered yet");
	}
}
void ResourceManager::RequestUnload(ResourceID id)
{
	auto it = container_.find(id);
	if (it != container_.end())
	{
		ResourceInfo& info = it->second;
		--info.counter;
	}
	else
	{
		assert(!"Resource hasn't been registered yet");
	}
}
void ResourceManager::Perform()
{
	for (auto& pair : container_)
	{
		ResourceInfo& info = pair.second;
		if (info.counter == 0 && info.resource != nullptr)
		{
			// Unload resource
			info.unloading_func(info.user_data, info.resource);
			info.resource = nullptr;
		}
		else if (info.counter > 0 && info.resource == nullptr)
		{
			// Load resource
			ResourceID id = pair.first;
			info.resource = info.loading_func(info.user_data, id);
		}
	}
}
bool ResourceManager::PerformStep()
{
	for (auto& pair : container_)
	{
		ResourceInfo& info = pair.second;
		if (info.counter == 0 && info.resource != nullptr)
		{
			// Unload resource
			info.unloading_func(info.user_data, info.resource);
			info.resource = nullptr;
			return false;
		}
		else if (info.counter > 0 && info.resource == nullptr)
		{
			// Load resource
			ResourceID id = pair.first;
			info.resource = info.loading_func(info.user_data, id);
			return false;
		}
	}
	return true;
}
int ResourceManager::GetResourcesCountToProcess()
{
	int count = 0;
	for (auto& pair : container_)
	{
		ResourceInfo& info = pair.second;
		if (info.counter == 0 && info.resource != nullptr)
		{
			++count;
		}
		else if (info.counter > 0 && info.resource == nullptr)
		{
			++count;
		}
	}
	return count;
}