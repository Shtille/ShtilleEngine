#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_SERVICE_H__
#define __SHT_SATELLITE_IMAGERY_SERVICE_H__

#include "../../system/include/tasks/service.h"

namespace sht {
	namespace utility {
		class CurlWrapper;
	}
}

namespace sht {
    namespace satellite_imagery {

        //! Base service class
        class Service : public system::Service {
        public:
			Service();
			virtual ~Service();

			utility::CurlWrapper * GetCurlWrapper();

        private:
			utility::CurlWrapper * curl_wrapper_; //!< each service should have own instance of curl wrapper
        };

    } // namespace satellite_imagery
} // namespace sht

#endif