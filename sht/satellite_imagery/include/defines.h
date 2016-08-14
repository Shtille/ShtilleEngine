#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_DEFINES_H__
#define __SHT_SATELLITE_IMAGERY_DEFINES_H__

namespace sht {
    namespace satellite_imagery {

        //! File offset type
        // Note: the file can't be larger than 2 Gb to let offsets work.
        typedef int FileOffsetType;

        //! File size type
        typedef unsigned int FileSizeType;
        typedef unsigned long long BigFileSizeType;

        //! Counter type
        typedef unsigned int CounterType;
        
    } // namespace satellite_imagery
} // namespace sht

#endif