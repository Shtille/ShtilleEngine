#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_BING_DATA_PROVIDER_H__
#define __SHT_SATELLITE_IMAGERY_BING_DATA_PROVIDER_H__

#include "data_provider_interface.h"

#include <stdio.h>

namespace sht {
    namespace satellite_imagery {
    
        //! Bing data provider class
        class BingDataProvider : public DataProviderInterface {
        public:
            //! Converts data key to unique string
            std::string TileXYToQuadKey(int tile_x, int tile_y, int level_of_detail)
            {
                std::string str;
                for (int i = level_of_detail; i > 0; --i)
                {
                    char digit = '0';
                    int mask = 1 << (i - 1);
                    if ((tile_x & mask) != 0)
                    {
                        digit++;
                    }
                    if ((tile_y & mask) != 0)
                    {
                        digit++;
                        digit++;
                    }
                    str.push_back(digit);
                }
                return str;
            }
            //! Converts data key to URL
            std::string KeyToUrl(const DataKey& key)
            {
                char buffer[250];
                std::string str = TileXYToQuadKey(key.x(), key.y(), key.z());
                int bytes_written = sprintf(buffer, "http://ak.t3.tiles.virtualearth.net/tiles/a%s.jpeg?g=5160&n=z&c4w=1", str.c_str());
                if (bytes_written > 0)
                {
                    return std::string(buffer);
                }
                else
                    return std::string();
            }
            std::string HashName()
            {
                return "Bing";
            }

            virtual const int GetMinLod() { return 0; }
            virtual const int GetMaxLod() { return 19; }
        };

    } // namespace satellite_imagery
} // namespace sht

#endif
