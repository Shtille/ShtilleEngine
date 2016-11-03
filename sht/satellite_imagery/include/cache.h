#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CACHE_H__
#define __SHT_SATELLITE_IMAGERY_CACHE_H__

#include "../../common/non_copyable.h"

namespace sht {
	namespace satellite_imagery {

    	// Forward declarations
    	class DataProviderInterface;
    	class Service;
    	class Storage;

    	//! Interface class for satellite imagery
    	class Cache : public NonCopyable {
    	public:
    		explicit Cache(DataProviderInterface * provider);
    		virtual ~Cache();

			bool Initialize();

			//! Whether key exists in our database
			bool IsExist(const DataKey& key);

			//! Query to load a single tile using tile service
			void TileServiceLoadQuery(const DataKey& key, boost::shared_ptr<IObserver> observer);

			//! Query to load a single tile using region service
			void RegionServiceLoadQuery(const DataKey& key, boost::shared_ptr<IObserver> observer, const char* name);

			//! Clear all regions download tasks
			void ClearRegionServiceTasks();

			//! Should be called before any RegionServiceLoadQuery call
			bool Region_Add(const RegionInfo& region_info);

            bool Region_Rename(const std::string& old_name, const std::string& new_name);

            bool Region_Delete(const std::string& name);

            bool Region_MarkStored(const std::string& name);

            bool Region_MarkInvalid(const std::string& name);

			bool GetRegionInfo(const std::string& name, StoredRegionInfo& info);

			bool GetRegionsInfo(std::vector<StoredRegionInfo>& regions_info);

            BigFileSizeType GetRegionFileSize(const std::string& name);

			unsigned int GetKeyCount() const;

			//! Estimated storage file size for specified number of tiles and average data size
			static BigFileSizeType GetEstimatedStorageSize(unsigned int num_tiles, unsigned int data_size);


    	private:
    		DataProviderInterface * const provider_;
    		Service * tile_service_;
    		Service * region_service_;
    		Storage * storage_;
    	};

	} // namespace satellite_imagery
} // namespace sht

#endif