#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_TILE_SERVICE_TASK_H__
#define __SHT_SATELLITE_IMAGERY_TILE_SERVICE_TASK_H__

#include "service_task.h"

namespace sht {
	namespace satellite_imagery {

		class TileServiceTask : public ServiceTask {
		public:
			explicit TileServiceTask(const DataKey& key, NotificationCenter * notification_center,
				Storage * storage, utility::CurlWrapper * curl_wrapper, DataProviderInterface * provider);
			virtual ~TileServiceTask();

			bool Execute() final;
			void Notify(bool success) final;

		protected:
			bool is_load_; //!< load or download
		};

	} // namespace satellite_imagery
} // namespace sht

#endif