#include "../include/service_task.h"

namespace sht {
    namespace satellite_imagery {

        ServiceTask::ServiceTask(const DataKey& key, std::shared_ptr<ObserverInterface> observer,
                Storage * storage, CurlWrapper * curl_wrapper, IDataProvider * provider)
            : key_(key)
            , storage_(storage)
            , observer_(observer)
            , curl_wrapper_(curl_wrapper)
            , data_provider_(provider)
        {

        }
        ServiceTask::~ServiceTask()
        {

        }
        Content& ServiceTask::content()
        {
            return content_;
        }
        const Content& ServiceTask::content() const
        {
            return content_;
        }
        const DataKey& ServiceTask::key()
        {
            return key_;
        }
        size_t ServiceTask::OnDataReceived(void* buffer, size_t size, size_t nmemb, void* userdata)
        {
            std::string * str = reinterpret_cast<std::string*>(userdata);
            str->append(static_cast<char*>(buffer), nmemb * size);
            return nmemb * size;
        }
        
    } // namespace satellite_imagery
} // namespace sht