#include "base_service.h"

#include "../../utility/include/curl_wrapper.h"

namespace sht {
    namespace satellite_imagery {

        Service::Service()
        : system::Service()
        {
        	curl_wrapper_ = new utility::CurlWrapper();
        }
		Service::~Service()
		{
			delete curl_wrapper_;
		}
		utility::CurlWrapper * Service::GetCurlWrapper()
		{
			return curl_wrapper_;
		}

    } // namespace satellite_imagery
} // namespace sht