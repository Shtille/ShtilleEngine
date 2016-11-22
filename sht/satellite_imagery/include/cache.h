#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CACHE_H__
#define __SHT_SATELLITE_IMAGERY_CACHE_H__

#include "key.h"
#include "defines.h"

#include "../../common/non_copyable.h"
#include "../../common/notification.h"

#include <string>

namespace sht {
	namespace satellite_imagery {

    	// Forward declarations
    	class DataProviderInterface;
        class Service;
    	class Storage;

    	//! Interface class for satellite imagery
    	class Cache : public NonCopyable {
    	public:
    		explicit Cache(DataProviderInterface * provider, const std::string& path);
    		virtual ~Cache();

			bool Initialize();

			//! Whether key exists in our database
			bool IsExist(const DataKey& key);

            void AddTileLoadObserver(ObserverInterface * tile_load_observer, NotificationFunc function);
            void RemoveTileLoadObserver(ObserverInterface * tile_load_observer);

			//! Query to load a single tile using tile service
			void TileServiceLoadQuery(const DataKey& key);

			unsigned int GetKeyCount() const;

			//! Estimated storage file size for specified number of tiles and average data size
			static BigFileSizeType GetEstimatedStorageSize(unsigned int num_tiles, unsigned int data_size);

    	private:
    		DataProviderInterface * const provider_;
    		Service * tile_service_;
    		Service * region_service_;
    		Storage * storage_;
            NotificationCenter tile_load_notification_center_;
    	};

	} // namespace satellite_imagery
} // namespace sht

#endif