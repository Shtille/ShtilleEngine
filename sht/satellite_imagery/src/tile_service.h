#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_TILE_SERVICE_H__
#define __SHT_SATELLITE_IMAGERY_TILE_SERVICE_H__

#include "../../common/system/include/tasks/service.h"

namespace sht {
    namespace satellite_imagery {

        //! Base file class
        class TileService : public system::Service {
        public:
        };

    } // namespace satellite_imagery
} // namespace sht

#endif