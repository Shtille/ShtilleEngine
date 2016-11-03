#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_SERVICE_TASK_H__
#define __SHT_SATELLITE_IMAGERY_SERVICE_TASK_H__

#include "../../system/include/tasks/service_task_interface.h"
#include "../include/key.h"
#include "content.h"

#include <cstddef>
#include <memory>

namespace sht {
	namespace system {
		class CurlWrapper;
	}
}

namespace sht {
	namespace satellite_imagery {

		class Content;
		class ObserverInterface;
		class Storage;
		class IDataProvider;

		class ServiceTask : public ServiceTaskInterface {
		public:
			explicit ServiceTask(const DataKey& key, std::shared_ptr<ObserverInterface> observer,
				Storage * storage, CurlWrapper * curl_wrapper, IDataProvider * provider);
			virtual ~ServiceTask();

			Content& content();
			const Content& content() const;
			const DataKey& key();

		protected:
			static size_t OnDataReceived(void* buffer, size_t size, size_t nmemb, void* userdata);

			DataKey key_;
			Content content_;
			Storage * const storage_;
			std::weak_ptr<ObserverInterface> observer_;
			CurlWrapper * const curl_wrapper_;
			IDataProvider * const data_provider_;
		};

	} // namespace satellite_imagery
} // namespace sht

#endif