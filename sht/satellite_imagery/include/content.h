#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_CONTENT_H__
#define __SHT_SATELLITE_IMAGERY_CONTENT_H__

#include <string>

namespace sht {
    namespace satellite_imagery {

    	class Content {
    	public:
    		const std::string& data() const { return data_; }
    		std::string& data() { return data_; }

    	private:
    		std::string data_;
    	};

    } // namespace satellite_imagery
} // namespace sht

#endif