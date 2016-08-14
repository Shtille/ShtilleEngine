#include "../include/data_info.h"

namespace sht {
    namespace satellite_imagery {

        StoredDataInfo::StoredDataInfo(const DataInfo& info)
            : offset(info.data_offset)
            , size(info.size)
            , counter(info.counter)
        {
        }
        DataInfo::DataInfo(FileOffsetType key_offset, const StoredDataInfo& info)
            : key_offset(key_offset)
            , data_offset(info.offset)
            , size(info.size)
            , counter(info.counter)
        {
        }
        
    } // namespace satellite_imagery
} // namespace sht