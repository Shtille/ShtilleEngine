#pragma once
#ifndef __SHT_UTILITY_RESOURCE_MANAGER_H__
#define __SHT_UTILITY_RESOURCE_MANAGER_H__

#include "common/singleton.h"
#include "graphics/include/resource.h"
#include "utility/include/string_id.h"

#include <unordered_map>

namespace sht {
	namespace utility {

		typedef int ResourceID;
		typedef sht::graphics::Resource * (* ResourceLoadingFunc)(void * /*user_data*/, ResourceID);
		typedef void (* ResourceUnloadingFunc)(void * /*user_data*/, sht::graphics::Resource * /*resource*/);

		class ResourceManager : public sht::ManagedSingleton<ResourceManager> {
		public:
			ResourceManager();
			virtual ~ResourceManager();

			void CleanUp(); //!< releases all the resources

			ResourceID RegisterResource(sht::utility::StringId string_id, void* user_data, ResourceLoadingFunc loading_func, ResourceUnloadingFunc unloading_func);
			void UnregisterResource(ResourceID id);

			ResourceID GetResourceIdByName(sht::utility::StringId string_id);
			sht::graphics::Resource * GetResource(ResourceID id);

			void RequestLoad(ResourceID id);
			void RequestUnload(ResourceID id);
			void Perform();
			bool PerformStep();
			int GetResourcesCountToProcess();

		private:
			struct ResourceInfo {
				int counter;
				sht::utility::StringId string_id;
				sht::graphics::Resource * resource;
				void * user_data;
				ResourceLoadingFunc loading_func;
				ResourceUnloadingFunc unloading_func;
			};
			typedef std::unordered_map<ResourceID, ResourceInfo> Container;
			Container container_;
			ResourceID current_resource_id_;
		};

	} // namespace utility
} // namespace sht

#endif