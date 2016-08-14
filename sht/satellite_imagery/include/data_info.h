#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_DATA_INFO_H__
#define __SHT_SATELLITE_IMAGERY_DATA_INFO_H__

#include "defines.h"

namespace sht {
    namespace satellite_imagery {

        struct DataInfo;

#pragma pack(push, 1)

        //! Data information to store in file
        struct StoredDataInfo {
            FileOffsetType offset;  //!< offset from the beginning of file
            FileSizeType size;      //!< size of stored data
            CounterType counter;    //!< usage counter for key replacement

            StoredDataInfo() = default;
            StoredDataInfo(const DataInfo& info);
        };

        //! Data information struct for inner use
        struct DataInfo {
            FileOffsetType key_offset;      //!< key offset from the beginning of file
            FileOffsetType data_offset;     //!< data offset from the beginning of file
            FileSizeType size;              //!< size of stored data
            CounterType counter;            //!< usage counter for key replacement

            DataInfo() = default;
            DataInfo(FileOffsetType key_offset, const StoredDataInfo& info);
        };

#pragma pack(pop)
        
    } // namespace satellite_imagery
} // namespace sht

#endif