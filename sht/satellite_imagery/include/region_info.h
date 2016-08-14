#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_REGION_INFO_H__
#define __SHT_SATELLITE_IMAGERY_REGION_INFO_H__

#include <string>
#include <map>
#include <ctime>

namespace sht {
    namespace satellite_imagery {

        struct RegionStatus {
            enum {
                kStored = 0, // OK
                kDownloading = 1,
                kInvalid = 2
            };
        };

        //! Region information struct
        struct RegionInfo {
            double upper_latitude;  //!< upper latitude of bounding box
            double left_longitude;  //!< left longitude of bounding box
            double lower_latitude;  //!< lower latitude of bounding box
            double right_longitude; //!< right longitude of bounding box
            std::string name;       //!< name of a region
        };

        //! Stored region information
        struct StoredRegionInfo {
            RegionInfo info;        //!< region information
            unsigned int status;    //!< region status
            time_t time;            //!< region data time
            std::string filename;   //!< region storage filename
        };

        typedef std::map<std::string, StoredRegionInfo> RegionMap;
        
    } // namespace satellite_imagery
} // namespace sht

#endif