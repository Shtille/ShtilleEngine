#include "tile_service_task.h"

#include "../include/providers/data_provider_interface.h"
#include "content_notification.h"
#include "content.h"
#include "storage.h"

#include "../../common/notification.h"
#include "../../utility/include/curl_wrapper.h"
#include "../../system/include/stream/log_stream.h"

namespace sht {
    namespace satellite_imagery {

        TileServiceTask::TileServiceTask(const DataKey& key, NotificationCenter * notification_center,
                Storage * storage, utility::CurlWrapper * curl_wrapper, DataProviderInterface * provider)
        : ServiceTask(key, notification_center, storage, curl_wrapper, provider)
        , is_load_(false)
        {

        }
        TileServiceTask::~TileServiceTask()
        {

        }
        bool TileServiceTask::Execute()
        {
            StorageInfo * storage_info;
            KeyPair * pair = storage_->GetKeyPair(key_, &storage_info);
            is_load_ = (pair != 0);
            if (is_load_) // load
            {
                return storage_->Load(pair, content_.data(), storage_info);
            }
            else // download
            {
                std::string url = data_provider_->KeyToUrl(key_);
                if (url.empty())
                {
                    LOG_ERROR("Online Map Downloader: provider failed with key(%i,%i,%i)",
                                  key_.z(), key_.x(), key_.y());
                    return false;
                }
                return curl_wrapper_->Download(url.c_str(), 
                                              reinterpret_cast<void*>(content_.data()),
                                              OnDataReceived);
            }
        }
        void TileServiceTask::Notify(bool success)
        {
            if (success)
            {
                if (is_load_) // load
                {
                    // Then notify main observer (to let copy or move data)
                    ContentNotification notification(key_, content_, success);
                    notification_center_->NotifyWithoutDispatch(&notification);
                }
                else // download
                {
                    // Save data to the main storage
                    if (storage_->SaveMain(key_, content_.data()))
                    {
                        // Then notify main observer (to let copy or move data)
                        ContentNotification notification(key_, content_, success);
                        notification_center_->NotifyWithoutDispatch(&notification);
                    }
                    else
                    {
                        LOG_ERROR("Online Map Downloader: failed to save data with key(%i,%i,%i)",
                                      key_.z(), key_.x(), key_.y());
                    }
                }
            }
            else // fail notification
            {
                ContentNotification notification(key_, content_, success);
                notification_center_->NotifyWithoutDispatch(&notification);
            }
        }
        
    } // namespace satellite_imagery
} // namespace sht