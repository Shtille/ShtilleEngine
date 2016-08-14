#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_BASEMAP_DATA_PROVIDER_H__
#define __SHT_SATELLITE_IMAGERY_BASEMAP_DATA_PROVIDER_H__

#include "data_provider_interface.h"

#include <stdio.h>

namespace sht {
    namespace satellite_imagery {
    
        //! Base map data provider class
        class BasemapDataProvider : public DataProviderInterface {
        public:
            //! Converts data key to URL
            std::string KeyToUrl(const DataKey& key)
            {
                char buffer[150];
                int bytes_written = sprintf(buffer, "http://basemap.nationalmap.gov/arcgis/rest/services/USGSImageryOnly/MapServer/tile/%i/%i/%i",
                    key.z(), key.y(), key.x());
                if (bytes_written > 0)
                {
                    return std::string(buffer);
                }
                else
                    return std::string();
            }
            std::string HashName()
            {
                return "Basemap";
            }
        };

    } // namespace satellite_imagery
} // namespace sht

#endif