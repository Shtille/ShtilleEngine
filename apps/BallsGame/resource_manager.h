#pragma once
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include "common/singleton.h"
#include "utility/include/string_id.h"

#include <unordered_map>

typedef int ResourceID;
typedef void * (* ResourceLoadingFunc)(void * /*user_data*/, ResourceID);
typedef void (* ResourceUnloadingFunc)(void * /*user_data*/, void * /*resource*/);

class ResourceManager : public sht::Singleton<ResourceManager> {
public:
	ResourceManager();
	virtual ~ResourceManager();

	ResourceID RegisterResource(StringId string_id, void* user_data, ResourceLoadingFunc loading_func, ResourceUnloadingFunc unloading_func);
	void UnregisterResource(ResourceID id);

	ResourceID GetResourceIdByName(StringId string_id);
	void * GetResource(ResourceID id);

	void RequestLoad(ResourceID id);
	void RequestUnload(ResourceID id);
	void Perform();
	bool PerformStep();
	int GetResourcesCountToProcess();

private:
	struct ResourceInfo {
		int counter;
		StringId string_id;
		void * resource;
		void * user_data;
		ResourceLoadingFunc loading_func;
		ResourceUnloadingFunc unloading_func;
	};
	typedef std::unordered_map<ResourceID, ResourceInfo> Container;
	Container container_;
	ResourceID current_resource_id_;
};

#endif