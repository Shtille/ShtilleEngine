#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_REGIONS_HEADER_FILE_H__
#define __SHT_SATELLITE_IMAGERY_REGIONS_HEADER_FILE_H__

#include "file.h"
#include "../include/region_info.h"

namespace sht {
    namespace satellite_imagery {

        //! Regions header file
        class RegionsHeaderFile : public File {
            friend class Storage;
        public:
            RegionsHeaderFile(const std::string& filename);
            ~RegionsHeaderFile();

            unsigned int GetUniqueID() const;

            bool AddRegion(const RegionMap& region_map);
            bool RemoveRegion(const RegionMap& region_map);
            bool Update(const RegionMap& region_map); //!< simply rewrite entire file

            void ReadAllRegions(RegionMap& region_map);

            bool Flush();

            void MarkOperationsBegin();
            void MarkOperationsEnd();

            // Write operations
            void WriteHeader();
            void WriteRegionInfo(const RegionInfo& region);
            void WriteRegionStoredInfo(const StoredRegionInfo& region);

            // Read operations
            bool ReadHeader();
            void ReadRegionInfo(RegionInfo& region);
            void ReadRegionStoredInfo(StoredRegionInfo& region);

        private:
            unsigned int unique_id_;     //!< unique region identifier (counter)
            unsigned int num_regions_;   //!< number of regions
        };

    } // namespace satellite_imagery
} // namespace sht

#endif