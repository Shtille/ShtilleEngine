#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_SERVICE_TASK_H__
#define __SHT_SATELLITE_IMAGERY_SERVICE_TASK_H__

#include "../../system/include/tasks/service_task_interface.h"
#include "../include/key.h"
#include "content.h"

#include <cstddef>
#include <memory>

namespace sht {
	class NotificationCenter;
	namespace utility {
		class CurlWrapper;
	}
}

namespace sht {
	namespace satellite_imagery {

		class Content;
		class Storage;
		class DataProviderInterface;

		class ServiceTask : public system::ServiceTaskInterface {
		public:
			explicit ServiceTask(const DataKey& key, NotificationCenter * notification_center,
				Storage * storage, utility::CurlWrapper * curl_wrapper, DataProviderInterface * provider);
			virtual ~ServiceTask();

			Content& content();
			const Content& content() const;
			const DataKey& key();

		protected:
			static size_t OnDataReceived(void* buffer, size_t size, size_t nmemb, void* userdata);

			DataKey key_;
			Content content_;
			Storage * const storage_;
			NotificationCenter * const notification_center_;
			utility::CurlWrapper * const curl_wrapper_;
			DataProviderInterface * const data_provider_;
		};

	} // namespace satellite_imagery
} // namespace sht

#endif