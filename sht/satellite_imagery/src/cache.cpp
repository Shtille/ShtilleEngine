#include "../include/cache.h"

#include "tile_service.h"
//#include "region_service.h"

#include "tile_service_task.h"
//#include "region_service_task.h"

#include "storage.h"
#include "../include/providers/data_provider_interface.h"

namespace sht {
    namespace satellite_imagery {

        Cache::Cache(DataProviderInterface * provider, const std::string& path)
            : provider_(provider)
            , tile_service_(nullptr)
            , region_service_(nullptr)
        {
            tile_service_ = new TileService();
            tile_service_->RunService();

            // region_service_ = new RegionService();
            // region_service_->RunService();

            storage_ = new Storage(provider_->HashName(), path);
        }
        Cache::~Cache()
        {
            // region_service_->StopService();
            // delete region_service_;

            tile_service_->StopService();
            delete tile_service_; // should be first to let thread finish

            delete storage_;
        }
        bool Cache::Initialize()
        {
            return storage_->Initialize();
        }
        bool Cache::IsExist(const DataKey& key)
        {
            return storage_->IsExist(key);
        }
        void Cache::AddTileLoadObserver(ObserverInterface * tile_load_observer, NotificationFunc function)
        {
			tile_load_notification_center_.AddObserver(tile_load_observer, function);
        }
        void Cache::RemoveTileLoadObserver(ObserverInterface * tile_load_observer)
        {
            tile_load_notification_center_.RemoveObserver(tile_load_observer);
        }
        void Cache::TileServiceLoadQuery(const DataKey& key)
        {
            ServiceTask * task = new TileServiceTask(key,
                                              &tile_load_notification_center_,
                                              storage_,
                                              tile_service_->GetCurlWrapper(),
                                              provider_);
            tile_service_->AddTask(task);
        }
        unsigned int Cache::GetKeyCount() const
        {
            return static_cast<unsigned int>(storage_->GetKeyCount());
        }
        BigFileSizeType Cache::GetEstimatedStorageSize(unsigned int num_tiles, unsigned int data_size)
        {
            return Storage::GetEstimatedFileSize(num_tiles, data_size);
        }

    } // namespace satellite_imagery
} // namespace sht