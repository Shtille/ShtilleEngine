#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CONTENT_H__
#define __SHT_SATELLITE_IMAGERY_CONTENT_H__

#include <string>

namespace sht {
    namespace satellite_imagery {

    	class Content {
    	public:
			std::string* data();

    	private:
    		std::string data_;
    	};

    } // namespace satellite_imagery
} // namespace sht

#endif