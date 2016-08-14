#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_DIGITAL_GLOBE_DATA_PROVIDER_H__
#define __SHT_SATELLITE_IMAGERY_DIGITAL_GLOBE_DATA_PROVIDER_H__

#include "data_provider_interface.h"

#include <stdio.h>

namespace sht {
    namespace satellite_imagery {
    
        //! Data provider class interface
        class DigitalGlobeDataProvider : public DataProviderInterface {
        public:
            //! Converts data key to URL
            std::string KeyToUrl(const DataKey& key)
            {
                const char * kUrlFormat = "https://services.digitalglobe.com/mapservice/wmtsaccess?CONNECTID=2259778b-d0cf-4f2a-92cb-78fe9301e807&Service=WMTS&REQUEST=GetTile&Version=1.0.0&Format=image/jpeg&Layer=DigitalGlobe:ImageryTileService&TileMatrixSet=%s&TileMatrix=%s:%i&TileRow=%i&TileCol=%i";
                char buffer[500];
                int bytes_written = sprintf(buffer, kUrlFormat,
                    DataSource(), DataSource(),
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
                return std::string("DigitalGlobe-jpeg-") + std::string(DataSource());
            }
            const double GetMaxLatitude() { return  90.0; }
            const double GetMinLatitude() { return -90.0; }
            const double GetMaxLongitude() { return  180.0; }
            const double GetMinLongitude() { return -180.0; }
            const int GetMinLod() { return 0; }
            const int GetMaxLod() { return 20; }
        private:
            const char * DataSource()
            {
                return "EPSG:3857";
            }
        };

    } // namespace satellite_imagery
} // namespace sht

#endif